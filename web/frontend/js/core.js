// Shared helpers (h, esc, api), progress stored in localStorage, global state.
export const $ = (s, el = document) => el.querySelector(s);
export const esc = s => String(s).replace(/[&<>"]/g, c => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;" }[c]));
export const h = (tag, attrs = {}, ...kids) => {
  const el = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) {
    if (k.startsWith("on")) el.addEventListener(k.slice(2), v);
    else if (k === "html") el.innerHTML = v;
    else if (v !== false && v != null) el.setAttribute(k, v === true ? "" : v);
  }
  for (const kid of kids.flat()) if (kid != null && kid !== false) el.append(kid);
  return el;
};
export const api = async (path, body) => {
  const res = await fetch(path, body === undefined ? {} : { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(body) });
  return res.json();
};

// ---------- state saved in the browser (predictions, right/wrong)
const STORE_KEY = "bosch-lab:v1";
export let store = {};
try { store = JSON.parse(localStorage.getItem(STORE_KEY)) || {}; } catch { store = {}; }
export const saveStore = () => { try { localStorage.setItem(STORE_KEY, JSON.stringify(store)); } catch {} };
export const labState = id => (store[id] ||= { pred: "", mark: null, revealed: false });

// Shared state: other modules read these live bindings and change them with set({ name: value })
export let ROOT = "", LABS = [], current = null, filter = "all", fileTab = 0, ignoreNums = false;
export let mode = "lab", TOPICS = [], currentTopic = null, quiz = null, labTopic = null;
const SETTERS = {
  ROOT: v => { ROOT = v; }, LABS: v => { LABS = v; }, TOPICS: v => { TOPICS = v; }, current: v => { current = v; },
  filter: v => { filter = v; }, fileTab: v => { fileTab = v; }, ignoreNums: v => { ignoreNums = v; }, mode: v => { mode = v; },
  currentTopic: v => { currentTopic = v; }, quiz: v => { quiz = v; }, labTopic: v => { labTopic = v; },
};
export const set = patch => { for (const [k, v] of Object.entries(patch)) SETTERS[k](v); };
export const setHash = hsh => { try { history.replaceState(null, "", "#" + hsh); } catch {} };
export const DIR_NAMES = { "c": "C", "cpp": "C++", "linux-process": "Process", "linux-signal": "Signal", "linux-thread": "Thread", "linux-ipc": "IPC", "kernel-module": "Kernel module", "shell": "Shell", "alp": "Advanced Linux Programming ch.1–5", "design-patterns": "Design patterns" };

// Renamed labs/topics/questions (data/migrations/*.json): move the saved progress to the new ids, once per file
export function applyMigrations(migs) {
  const done = (store.__migrations ||= []);
  const move = (obj, map) => { for (const [o, n] of Object.entries(map || {})) if (obj && o in obj) { if (!(n in obj)) obj[n] = obj[o]; delete obj[o]; } };
  for (const [name, m] of Object.entries(migs || {})) {
    if (done.includes(name)) continue;
    move(store, m.labs); move(store.__quiz, m.questions); move(store.__read, m.topics);
    done.push(name);
  }
  saveStore();
}

// ---------- light / dark theme: auto (follow the OS) → light → dark
const THEMES = { auto: ["◐", "Theme: follows the system — click for light"], light: ["☀", "Theme: light — click for dark"], dark: ["☾", "Theme: dark — click to follow the system"] };
function applyTheme(t) {
  if (t === "light" || t === "dark") document.documentElement.dataset.theme = t;
  else delete document.documentElement.dataset.theme;
  try { t === "auto" ? localStorage.removeItem("bosch-lab:theme") : localStorage.setItem("bosch-lab:theme", t); } catch {}
  const btn = document.getElementById("themeBtn");
  if (btn) { btn.textContent = THEMES[t][0]; btn.title = THEMES[t][1]; btn.setAttribute("aria-label", THEMES[t][1]); }
}
(() => {
  const btn = document.getElementById("themeBtn");
  const cur = () => document.documentElement.dataset.theme || "auto";
  if (btn) btn.onclick = () => applyTheme({ auto: "light", light: "dark", dark: "auto" }[cur()]);
  applyTheme(cur());
})();
