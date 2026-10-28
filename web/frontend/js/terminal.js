// In-browser terminal: runs labs via /api/runs, polls output, Ctrl+C/Z/D, kill, ps; Terminal 2.
"use strict";

// ---------- terminal (used by each lab and by Terminal 2)
const RUNS = {};   // slot -> { rid, chunks, next, state, pid, done, result, log }
const cleanTerm = s => s.replace(/\x1b\[[0-?]*[ -\/]*[@-~]/g, "").replace(/\x1b\][^\x07]*(\x07|\x1b\\)/g, "").replace(/\r+\n/g, "\n").replace(/[\r\x07]/g, "");
const runText = r => r ? cleanTerm(r.chunks.filter(c => c[0] === "out").map(c => c[1]).join("")) : "";

async function poll(slot) {
  const r = RUNS[slot];
  if (!r || r.polling) return;
  r.polling = true;
  while (RUNS[slot] === r) {
    let d;
    try { d = await api(`/api/runs/${r.rid}?from=${r.next}`); } catch { d = null; }
    if (!d || d.error) { r.done = true; r.state = "done"; break; }
    r.chunks.push(...d.chunks);
    Object.assign(r, { next: d.next, pid: d.pid, state: d.state, done: d.done, result: d.result });
    if (d.chunks.length || d.done) refreshSlot(slot);
    if (d.done) break;
    await new Promise(ok => setTimeout(ok, d.chunks.length ? 60 : 200));
  }
  r.polling = false;
  renderNav();
}

const VIEWS = {};  // slot -> redraw function called on new output
function refreshSlot(slot) { VIEWS[slot]?.(); }

function termView(slot, opts) {
  const pre = h("pre", { tabindex: 0 });
  const stateEl = h("span", { class: "state" });
  const input = h("input", { class: "mono", placeholder: "type stdin, then Enter (sends the whole line)…", autocomplete: "off", spellcheck: "false" });
  const send = (path, body) => RUNS[slot] && api(`/api/runs/${RUNS[slot].rid}/${path}`, body).then(d => { if (d?.error) note(d.error); });
  const note = msg => { const r = RUNS[slot]; if (r) { r.chunks.push(["sys", `\n[${msg}]\n`]); refreshSlot(slot); } };
  const key = (k, label, title) => h("button", { class: "small", title, onclick: () => send("key", { key: k }) }, label);
  const sig = s => h("button", { class: "small", title: `kill -${s} <PID>`, onclick: () => send("signal", { sig: s }) }, s);

  input.addEventListener("keydown", e => {
    if (e.key === "Enter") { send("input", { text: input.value + "\n" }); input.value = ""; e.preventDefault(); }
    else if (e.ctrlKey && ["c", "z", "d", "\\"].includes(e.key.toLowerCase())) { send("key", { key: e.key.toLowerCase() }); e.preventDefault(); }
  });

  const ctl = h("div", { class: "ctl" },
    h("span", { class: "lbl" }, "stdin"), input,
    key("c", "^C", "Ctrl+C: SIGINT to the foreground job"), key("z", "^Z", "Ctrl+Z: SIGTSTP"),
    key("\\", "^\\", "Ctrl+\\: SIGQUIT"), key("d", "^D", "Ctrl+D: EOF on stdin"),
    h("button", { class: "small", title: "Resume the stopped job in the foreground", onclick: () => send("job", { cmd: "fg" }) }, "fg"));
  if (opts.signals) ctl.append(h("span", { class: "sep" }), h("span", { class: "lbl" }, "kill"), ...["TERM", "USR1", "STOP", "CONT", "KILL"].map(sig),
    h("button", { class: "small", title: "ps for the processes in this run's session", onclick: () => send("ps", {}) }, "ps"));
  ctl.append(h("span", { class: "sep" }), h("button", { class: "small", title: "kill -9 every process of this run", onclick: () => send("stop", {}) }, "Stop"));

  const box = h("div", { class: "term" }, pre, ctl);
  const paint = () => {
    const r = RUNS[slot];
    if (!r) { pre.innerHTML = `<span class="log">${esc(opts.empty || "Not run yet.")}</span>`; stateEl.textContent = ""; stateEl.className = "state"; ctl.hidden = true; return; }
    const stick = pre.scrollHeight - pre.scrollTop - pre.clientHeight < 30;
    pre.innerHTML = (r.log ? `<span class="${r.buildFailed ? "err" : "log"}">${esc(r.log)}\n</span>` : "") +
      r.chunks.map(([k, s]) => k === "out" ? esc(cleanTerm(s)) : `<span class="sys">${esc(s)}</span>`).join("");
    if (stick) pre.scrollTop = pre.scrollHeight;
    const label = { running: "running", stopped: "Stopped", exited: "exited, waiting for child processes", done: "done" }[r.state] || r.state;
    stateEl.textContent = (r.pid ? `PID ${r.pid} · ` : "") + label;
    stateEl.className = "state " + r.state;
    ctl.hidden = r.done || !r.rid;
    opts.onUpdate?.(r);
  };
  VIEWS[slot] = paint;
  paint();
  return { box, stateEl, focus: () => input.focus(), paint };
}

async function startRun(slot, body) {
  const old = RUNS[slot];
  if (old && !old.done) api(`/api/runs/${old.rid}/stop`, {});
  RUNS[slot] = { rid: null, chunks: [], next: 0, state: "running", log: "building…", done: false };
  refreshSlot(slot);
  const d = await api("/api/runs", body);
  if (!d.ok) {
    RUNS[slot] = { rid: null, chunks: [], next: 0, state: "done", done: true, buildFailed: true, log: d.build_log || d.error || "error" };
    refreshSlot(slot);
    return false;
  }
  RUNS[slot] = { rid: d.run, chunks: [], next: 0, state: "running", done: false, log: d.build_log ? d.build_log : "" };
  refreshSlot(slot);
  renderNav();
  poll(slot);
  return true;
}

// ---------- Terminal 2
let dockTerm = null;
async function openDock(cmd) {
  $("#dock").hidden = false;
  $("#dockToggle").hidden = true;
  if (!dockTerm) {
    dockTerm = termView("terminal", { signals: false, empty: "Opening bash…" });
    $("#dockTerm").append(dockTerm.box);
  }
  if (!RUNS.terminal || RUNS.terminal.done) await startRun("terminal", { terminal: true });
  if (cmd) api(`/api/runs/${RUNS.terminal.rid}/input`, { text: cmd });
  dockTerm.focus();
}
$("#dockToggle").onclick = () => openDock();
$("#dockHide").onclick = () => { $("#dock").hidden = true; $("#dockToggle").hidden = false; };
