// Smoke test of the web UI without a browser: loads web/frontend/js/main.js as an ES module in gjs (Firefox's
// SpiderMonkey engine) with a tiny fake DOM and the real API, then opens every page and clicks through the quizzes.
//   make test-web      (needs gjs, curl and a running `make web`)
// Catches missing imports, assignments to imported state and runtime errors in any page.
import GLib from "gi://GLib";

const FRONT = ARGV[0];
const BASE = ARGV[1] || "http://localhost:8000";
const errors = [];

class El {
  constructor(tag) { this.tagName = tag.toUpperCase(); this.children = []; this.attrs = {}; this.style = {}; this.dataset = {}; this._html = ""; this.textContent = ""; this.hidden = false; this.listeners = {}; this.scrollTop = 0; this.scrollHeight = 0; this.clientHeight = 0; this.value = ""; }
  setAttribute(k, v) { this.attrs[k] = String(v); if (k === "class") this.className = v; if (k === "id") this.id = v; }
  getAttribute(k) { return this.attrs[k]; }
  addEventListener(k, fn) { (this.listeners[k] ||= []).push(fn); }
  append(...kids) { for (const k of kids) this.children.push(typeof k === "string" ? { text: k } : k); }
  replaceChildren(...kids) { this.children = []; this.append(...kids); }
  set innerHTML(v) { this._html = v; this.children = []; }
  get innerHTML() { return this._html; }
  get content() { return { firstChild: new El("svg") }; }
  get firstChild() { return this.children[0]; }
  querySelector() { return null; }
  querySelectorAll() { return []; }
  focus() {} scrollIntoView() {}
  click() { for (const fn of this.listeners.click || []) fn({ preventDefault() {}, stopPropagation() {}, target: this }); }
}
const byId = {};
const el = id => (byId[id] ||= Object.assign(new El("div"), { id }));
globalThis.document = {
  documentElement: new El("html"),
  createElement: t => new El(t),
  getElementById: id => el(id),
  querySelector: s => s.startsWith("#") ? el(s.slice(1)) : null,
  addEventListener() {},
  get activeElement() { return null; },
};
globalThis.addEventListener = () => {};
const ls = {};
globalThis.localStorage = { getItem: k => ls[k] ?? null, setItem: (k, v) => { ls[k] = String(v); }, removeItem: k => { delete ls[k]; } };
globalThis.history = { replaceState: (a, b, u) => { location.hash = u; } };
globalThis.location = { hash: "" };
globalThis.confirm = () => true;
globalThis.URLSearchParams = class { constructor(o) { this.o = o; } toString() { return Object.entries(this.o).map(([k, v]) => k + "=" + encodeURIComponent(v)).join("&"); } };
globalThis.setInterval = () => 0;
globalThis.setTimeout = (fn) => { fn(); return 0; };
const curl = path => new TextDecoder().decode(GLib.spawn_command_line_sync(`curl -s '${BASE}${path}'`)[1]);
globalThis.fetch = async path => ({ json: async () => JSON.parse(curl(path)) });

// Old progress, to check the migration runs at startup
ls["bosch-lab:v1"] = JSON.stringify({ "linux-process/01_fork_count": { mark: "wrong" }, "03-process/02_fork_if": { mark: "right" }, __read: { "01-c": true } });

const step = async (name, fn) => {
  try { await fn(); print("ok   " + name); } catch (e) { errors.push(name); print(`FAIL ${name}: ${e}\n${e.stack}`); }
};
const tick = () => new Promise(r => GLib.idle_add(GLib.PRIORITY_DEFAULT, () => { r(); return false; }));

const core = await import(`file://${FRONT}/js/core.js`);
await import(`file://${FRONT}/js/main.js`);
for (let i = 0; i < 20; i++) await tick();
const main = el("main");
const kids = n => JSON.stringify(n.children.length);
await step("startup + migration", () => {
  if (!core.LABS.length || !core.TOPICS.length) throw new Error("data not loaded");
  const st = JSON.parse(ls["bosch-lab:v1"]);
  if (!st["linux-process/02_fork_if"] || st["03-process/02_fork_if"] || !st.__read["c/c-language"]) throw new Error("migration not applied: " + ls["bosch-lab:v1"]);
});
const nav = await import(`file://${FRONT}/js/nav.js`);
const home = await import(`file://${FRONT}/js/home.js`);
const lab = await import(`file://${FRONT}/js/lab.js`);
const labindex = await import(`file://${FRONT}/js/labindex.js`);
const theory = await import(`file://${FRONT}/js/theory.js`);
const quiz = await import(`file://${FRONT}/js/quiz.js`);

await step("overview", () => { home.showHome(); if (core.mode !== "home" || !main.children.length) throw new Error("empty"); });
await step("lab index", () => { labindex.showLabIndex(); if (core.mode !== "lab" || core.labTopic !== null) throw new Error(core.mode); });
for (const t of labindex.labTopics()) await step("lab topic " + t.id, () => { labindex.showLabTopic(t.id); if (core.labTopic !== t.id) throw new Error("labTopic " + core.labTopic); });
for (const l of core.LABS) await step("lab " + l.id, () => { lab.go(l.id); if (core.current?.id !== l.id) throw new Error("current " + core.current?.id); });
await step("lab nav filters", () => { for (const f of ["todo", "wrong", "all"]) { core.set({ filter: f }); nav.renderNav(); } });
await step("theory index", () => { theory.showTheoryIndex(); if (core.mode !== "theory") throw new Error(core.mode); });
for (const t of core.TOPICS) await step("theory " + t.id, () => { theory.showTopic(t.id); if (core.currentTopic?.id !== t.id) throw new Error("topic"); });
await step("quiz setup", () => { quiz.showQuiz(); if (core.mode !== "quiz") throw new Error(core.mode); });
const text = n => n.text ?? (n.children || []).map(text).join("");
const buttons = (n, out = []) => { if (n.tagName === "BUTTON") out.push(n); for (const c of n.children || []) if (c.children) buttons(c, out); return out; };
const press = (label, root = main) => { const b = buttons(root).find(b => typeof label === "string" ? text(b).includes(label) : label(b)); if (!b) throw new Error("no button " + label); b.click(); };
const answerAll = () => {
  for (let guard = 0; guard < 1000 && core.quiz && !core.quiz.finished; guard++) {
    press(b => (b.className || "").startsWith("opt") && !b.attrs.disabled);
    const labels = buttons(main).map(text);
    if (labels.includes("Check")) press("Check");
    press(b => /^(Next →|See results|Submit)$/.test(text(b)));
  }
};
await step("quiz practice: answer every tricky question by clicking", () => {
  quiz.startQuiz({ topics: ["tricky/tricky-questions"], count: "all", filter: "all" });
  const n = core.quiz.items.length;
  answerAll();
  if (!core.quiz.finished) throw new Error("not finished");
  const qs = core.store.__quiz; if (Object.keys(qs).length !== n) throw new Error("recorded " + Object.keys(qs).length + "/" + n);
  if (!text(main).includes("Results")) throw new Error("no results page");
});
await step("quiz mock exam", () => {
  quiz.startQuiz({ topics: core.TOPICS.map(t => t.id), count: "20", filter: "all", exam: true });
  answerAll();
  if (!core.quiz.finished) throw new Error("not finished");
});
await step("quiz: redo wrong, then exit", () => {
  press(b => /^Redo \d+ wrong$/.test(text(b)));
  if (!core.quiz || core.quiz.finished) throw new Error("redo did not start");
  press("← Exit");
  if (core.quiz !== null) throw new Error("quiz not reset");
});
await step("mode tabs", () => { for (const k of ["Overview", "Labs", "Theory", "Quiz"]) { press(k, el("modes")); } if (core.mode !== "quiz") throw new Error(core.mode); });
const search = await import(`file://${FRONT}/js/search.js`);
await step("search page: empty, results, filters, open each kind", async () => {
  await search.showSearch("");
  if (core.mode !== "search" || !text(main).includes("Type a few words")) throw new Error("empty page");
  await search.showSearch("fork buf");
  const rows = () => (function walk(n, out = []) { if ((n.className || "").includes("search-row")) out.push(n); for (const c of n.children || []) if (c.children) walk(c, out); return out; })(main);
  if (!rows().length) throw new Error("no results");
  for (const k of ["Labs", "Theory", "Quiz"]) {
    press(b => text(b).startsWith(k));
    for (let i = 0; i < 10; i++) await tick();
    const r = rows(); if (!r.length) throw new Error("no " + k + " results");
    r[0].click();
    const want = { Labs: "lab", Theory: "theory", Quiz: "quiz" }[k];
    if (core.mode !== want) throw new Error(`${k}: opened mode ${core.mode}`);
    if (want === "quiz" && core.quiz.items.length !== 1) throw new Error("quiz should have 1 question");
    await search.showSearch("fork buf");
  }
  press(b => text(b) === "All");
  for (let i = 0; i < 10; i++) await tick();
  press(b => text(b) === "#alp");
  for (let i = 0; i < 10; i++) await tick();
  if (!text(main).includes("✕ #alp")) throw new Error("tag filter not shown");
  await search.showSearch("zzzzqqq");
  if (!text(main).includes("No results")) throw new Error("no-results message");
});
await step("theme button", () => { const b = el("themeBtn"); for (let i = 0; i < 3; i++) b.onclick(); });
print(errors.length ? `${errors.length} FAILED` : "ALL OK");
imports.system.exit(errors.length ? 1 : 0);
