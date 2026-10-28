// Shared helpers (h, esc, api), progress stored in localStorage, global state.
"use strict";

const $ = (s, el = document) => el.querySelector(s);
const esc = s => String(s).replace(/[&<>"]/g, c => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;" }[c]));
const h = (tag, attrs = {}, ...kids) => {
  const el = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) {
    if (k.startsWith("on")) el.addEventListener(k.slice(2), v);
    else if (k === "html") el.innerHTML = v;
    else if (v !== false && v != null) el.setAttribute(k, v === true ? "" : v);
  }
  for (const kid of kids.flat()) if (kid != null && kid !== false) el.append(kid);
  return el;
};
const api = async (path, body) => {
  const res = await fetch(path, body === undefined ? {} : { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(body) });
  return res.json();
};

// ---------- state saved in the browser (predictions, right/wrong)
const STORE_KEY = "bosch-lab:v1";
let store = {};
try { store = JSON.parse(localStorage.getItem(STORE_KEY)) || {}; } catch { store = {}; }
const saveStore = () => { try { localStorage.setItem(STORE_KEY, JSON.stringify(store)); } catch {} };
const labState = id => (store[id] ||= { pred: "", mark: null, revealed: false });

let ROOT = "", LABS = [], current = null, filter = "all", fileTab = 0, ignoreNums = false;
let mode = "lab", TOPICS = [], currentTopic = null, quiz = null;
const setHash = hsh => { try { history.replaceState(null, "", "#" + hsh); } catch {} };
const DIR_NAMES = { "01-c": "C", "02-cpp": "C++", "03-process": "Process", "04-signal": "Signal", "05-thread": "Thread", "06-ipc": "IPC", "07-kernel-module": "Kernel module", "08-shell": "Shell", "10-alp": "Advanced Linux Programming ch.1–5" };
