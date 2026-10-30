// Lab page: predict the output -> run -> compare (diff) -> answer.
import { $, current, DIR_NAMES, fileTab, h, ignoreNums, LABS, labState, labTopic, ROOT, saveStore, set, setHash } from "./core.js";
import { highlight, inline, langOf, md } from "./render.js";
import { openDock, RUNS, runText, startRun, termView, VIEWS } from "./terminal.js";
import { HUE, hueStyle, renderNav } from "./nav.js";
import { labTopicById, labTopicOf } from "./labindex.js";

// ---------- compare a prediction with the real output
function diffLines(a, b) {
  const n = a.length, m = b.length, dp = Array.from({ length: n + 1 }, () => new Int32Array(m + 1));
  for (let i = n - 1; i >= 0; i--) for (let j = m - 1; j >= 0; j--)
    dp[i][j] = a[i].key === b[j].key ? dp[i + 1][j + 1] + 1 : Math.max(dp[i + 1][j], dp[i][j + 1]);
  const out = [];
  let i = 0, j = 0;
  while (i < n || j < m) {
    if (i < n && j < m && a[i].key === b[j].key) { out.push(["same", b[j].text]); i++; j++; }
    else if (j < m && (i === n || dp[i][j + 1] >= dp[i + 1][j])) out.push(["add", b[j++].text]);
    else out.push(["del", a[i++].text]);
  }
  return out;
}
function toLines(s) {
  const lines = s.replace(/\r/g, "").split("\n").map(l => l.replace(/\s+$/, ""));
  while (lines.length && !lines[lines.length - 1]) lines.pop();
  return lines.slice(0, 400).map(text => ({ text, key: (ignoreNums ? text.replace(/\d+/g, "#") : text).replace(/\s+/g, " ") }));
}
function diffView(want, got, labels) {
  const rows = diffLines(toLines(want), toLines(got));
  const same = rows.every(r => r[0] === "same");
  return h("div", {},
    h("div", { class: "row", style: "margin:0 0 6px" },
      h("span", { class: "verdict " + (same ? "ok" : "bad") }, same ? "✓ Exact match" : `✗ ${rows.filter(r => r[0] !== "same").length} line(s) differ`),
      h("span", { class: "muted" }, h("span", { style: "color:var(--bad)" }, "- " + labels[0]), " · ", h("span", { style: "color:var(--ok)" }, "+ " + labels[1]))),
    h("div", { class: "diff" }, rows.map(([k, t]) => h("div", { class: k }, t || " "))));
}

// ---------- one lab page
export function go(id) {
  if (current?.id !== id) set({ fileTab: 0 });
  set({ mode: "lab", current: LABS.find(l => l.id === id) || LABS[0] });
  if (!labTopicById(labTopic)?.labs.includes(current)) set({ labTopic: labTopicOf(current)?.id || null });
  setHash("lab/" + current.id);
  renderNav();
  renderLab();
  $("#main").scrollTop = 0;
}

function renderLab() {
  const lab = current, st = labState(lab.id), m = lab.meta;
  const main = $("#main");
  Object.keys(VIEWS).forEach(k => k !== "terminal" && delete VIEWS[k]);

  const chips = h("div", { class: "chips" },
    m.topic ? h("span", { class: "chip tone" }, m.topic) : null,
    lab.unstable ? h("span", { class: "chip warn", title: "Order/values may differ between runs" }, "⚠️ nondeterministic output") : null,
    m.warning ? h("span", { class: "chip bad" }, "!!! " + m.warning) : null,
    m.warn_build ? h("span", { class: "chip warn", title: m.warn_build }, "intentional compiler warning") : null);

  // --- code
  const codeBody = h("div");
  const tabs = h("div", { class: "tabs" });
  const showFile = k => {
    set({ fileTab: k });
    const f = lab.files[k], src = lab.sources[f], lang = langOf(f);
    tabs.replaceChildren(...lab.files.map((name, j) => h("button", { class: j === k ? "on" : "", onclick: () => showFile(j) }, name.split("/").pop())));
    if (lang === "md") { codeBody.className = "bd md"; codeBody.innerHTML = md(src); return; }
    const n = src.replace(/\n$/, "").split("\n").length;
    codeBody.className = "code";
    codeBody.replaceChildren(h("pre", { class: "gut" }, Array.from({ length: n }, (_, j) => j + 1).join("\n")),
      h("pre", { html: highlight(src.replace(/\n$/, ""), lang) }));
  };
  const codeCard = h("div", { class: "card" },
    h("div", { class: "hd" }, tabs, h("span", { class: "sp" }),
      h("a", { href: `vscode://file${ROOT}/${lab.files[0]}`, class: "muted", title: "Open the file in VS Code to edit it (for the Try next part)" }, "open in VS Code ↗")),
    codeBody);
  showFile(Math.min(fileTab, lab.files.length - 1));

  // --- prediction
  const ta = h("textarea", { placeholder: "Write the output you expect here, then press Run (Ctrl+Enter)…", spellcheck: "false" });
  ta.value = st.pred;
  ta.addEventListener("input", () => { st.pred = ta.value; saveStore(); renderNav(); refreshCompare(); });
  const runnable = lab.kind !== "readonly";
  const runBtn = h("button", { class: "primary", disabled: !runnable, onclick: () => doRun() },
    lab.kind === "shell" ? "▶ Open bash" : "▶ Build & run");
  const doRun = async () => {
    if (!runnable) return;
    await startRun(lab.id, { id: lab.id });
    term.focus();
  };
  ta.addEventListener("keydown", e => { if (e.key === "Enter" && (e.ctrlKey || e.metaKey)) { e.preventDefault(); doRun(); } });

  const predictCard = lab.kind === "program" ? h("div", { class: "card" },
    h("div", { class: "hd" }, h("span", { class: "step" }, "1"), "Predict the output first"),
    h("div", { class: "bd" },
      lab.question ? h("div", { class: "q", html: `<b>Question:</b> ${inline(lab.question)}` }) : null,
      m.predict && m.predict !== lab.question ? h("div", { class: "muted", style: "margin-bottom:8px" }, m.predict) : null,
      ta,
      h("div", { class: "row" }, runBtn, h("span", { class: "muted" }, h("kbd", {}, "Ctrl"), "+", h("kbd", {}, "Enter"), " to run · notes are saved in your browser"))))
    : h("div", { class: "card" }, h("div", { class: "hd" }, lab.kind === "shell" ? "Command practice" : "Reading"),
      h("div", { class: "bd" },
        lab.kind === "shell"
          ? h("div", {}, h("p", { style: "margin-top:0" }, "Open a real bash (scratch directory ", h("code", {}, "build/shell"), ") and work through the 20 drills on the left. Type commands in the stdin box."), runBtn)
          : h("div", {}, h("p", { style: "margin-top:0" }, "This lab needs ", h("code", {}, "sudo"), " and linux-headers, so it does not run in the web UI. Follow the README in ", h("b", {}, "Terminal 2"), " (bottom right) or a normal terminal."),
              h("button", { onclick: () => openDock(`cd data/source/labs/kernel-module && make\n`) }, "Open Terminal 2 and build the module"))));

  // --- terminal + compare
  const compareBody = h("div", { class: "bd" });
  const compareCard = h("div", { class: "card", hidden: lab.kind !== "program" },
    h("div", { class: "hd" }, h("span", { class: "step" }, "3"), "Compare", h("span", { class: "sp" }),
      h("label", { class: "muted", title: "Replace every number with # when comparing (PIDs, addresses…)" },
        h("input", { type: "checkbox", checked: ignoreNums, onchange: e => { set({ ignoreNums: e.target.checked }); refreshCompare(); } }), " ignore numbers (PIDs…)")),
    compareBody);
  function refreshCompare() {
    if (lab.kind !== "program") return;
    const r = RUNS[lab.id], got = runText(r);
    const parts = [];
    if (!r || !r.rid) parts.push(h("div", { class: "muted" }, "Run the lab to compare your prediction with the real output."));
    else if (!st.pred.trim()) parts.push(h("div", { class: "muted" }, "You have not written a prediction yet. Write one in step 1 to compare."));
    else parts.push(h("div", { class: "muted", style: "margin-bottom:4px" }, "Your prediction ↔ real output" + (r.done ? "" : " (running…)")), diffView(st.pred, got, ["prediction", "real output"]));
    if (st.revealed && lab.expected && r?.rid)
      parts.push(h("div", { class: "muted", style: "margin:14px 0 4px" }, "Expected output (ANSWERS.md) ↔ real output"), diffView(lab.expected, got, ["expected", "real output"]));
    if (lab.unstable) parts.push(h("p", { class: "muted" }, "⚠️ This lab's output is nondeterministic: a different order or PID is not necessarily wrong. Read the explanation in the answer."));
    compareBody.replaceChildren(...parts);
  }

  const term = termView(lab.id, { signals: lab.kind === "program", empty: lab.kind === "shell" ? "Press ▶ Open bash to start." : runnable ? "Not run yet. Press ▶ to build & run." : "Reading only, not run in the web UI.", onUpdate: refreshCompare });
  const termCard = h("div", { class: "card", hidden: !runnable },
    h("div", { class: "hd" }, lab.kind === "program" ? [h("span", { class: "step" }, "2"), "Run"] : "bash", term.stateEl, h("span", { class: "sp" }),
      h("button", { class: "small", disabled: !runnable, onclick: doRun }, "Run again")),
    term.box);

  // --- answer
  const ansBody = h("div", { class: "bd" });
  const markBtn = (k, label) => h("button", { class: "small" + (st.mark === k ? " on" : ""), onclick: () => { st.mark = st.mark === k ? null : k; saveStore(); renderNav(); renderAnswer(); } }, label);
  function renderAnswer() {
    if (!st.revealed) {
      ansBody.replaceChildren(h("div", { class: "spoiler" },
        h("p", { class: "muted", style: "margin-top:0" }, "Open only after you have predicted and run it yourself."),
        h("button", { onclick: () => { st.revealed = true; saveStore(); renderAnswer(); refreshCompare(); } }, "Show answer & explanation")));
      return;
    }
    const notes = h("dl", { class: "notes" },
      m.idea ? [h("dt", {}, "Key idea"), h("dd", {}, m.idea)] : null,
      m.try ? [h("dt", {}, "Try next"), h("dd", {}, m.try)] : null,
      m.warn_build ? [h("dt", {}, "Intentional compiler warning"), h("dd", {}, m.warn_build)] : null);
    ansBody.replaceChildren(
      lab.kind === "program" ? h("div", { class: "row", style: "margin:0 0 10px" }, h("b", {}, "Self-grade:"), markBtn("right", "✓ I got it right"), markBtn("wrong", "✗ I got it wrong")) : null,
      h("div", { class: "md", html: md(lab.answer || "_No answer in ANSWERS.md._") }),
      notes.children.length ? notes : null,
      h("div", { class: "row" }, h("button", { class: "small", onclick: () => { st.revealed = false; saveStore(); renderAnswer(); refreshCompare(); } }, "Hide answer")));
  }
  const ansCard = h("div", { class: "card" }, h("div", { class: "hd" }, lab.kind === "program" ? [h("span", { class: "step" }, "4"), "Answer"] : "Answer"), ansBody);
  renderAnswer();
  refreshCompare();

  const idx = LABS.indexOf(lab);
  const navBtns = h("div", { class: "row", style: "margin-top:16px" },
    h("button", { disabled: idx <= 0, onclick: () => go(LABS[idx - 1].id) }, "← Previous lab"),
    h("button", { disabled: idx >= LABS.length - 1, onclick: () => go(LABS[idx + 1].id) }, "Next lab →"));

  main.replaceChildren(h("div", { class: "page", style: hueStyle(HUE[lab.dir] ?? 245) },
    h("div", { class: "eyebrow" }, (t => t ? [h("a", { href: "#labs/" + t.id, title: "All labs of this topic" }, `${t.group} · ${t.title}`), ` · lab ${t.labs.indexOf(lab) + 1}/${t.labs.length} · ${lab.dir}`] : `${lab.dir} · ${DIR_NAMES[lab.dir] || ""}`)(labTopicById(labTopic))),
    h("div", { class: "title-row" }, h("h2", {}, lab.title), h("span", { class: "muted" }, lab.id)),
    chips,
    h("div", { class: "grid" },
      h("div", {}, predictCard, termCard, compareCard),
      h("div", {}, codeCard, ansCard)),
    navBtns));
}
