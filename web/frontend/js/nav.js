// Sidebar: mode tabs, lab / topic lists; topic colors, icons, progress rings.
import { $, current, currentTopic, esc, filter, h, LABS, labTopic, mode, set, store, TOPICS } from "./core.js";
import { RUNS } from "./terminal.js";
import { daysLeft, showHome } from "./home.js";
import { go } from "./lab.js";
import { labGroups, labStats, labTopicById, showLabIndex, showLabTopic } from "./labindex.js";
import { qStats, showTheoryIndex, showTopic } from "./theory.js";
import { showQuiz, startQuiz } from "./quiz.js";

// ---------- sidebar
export function counts() {
  const ids = LABS.filter(l => l.kind === "program").map(l => l.id);
  const st = id => store[id] || {};
  return { total: ids.length, done: ids.filter(id => st(id).mark).length, right: ids.filter(id => st(id).mark === "right").length, wrong: ids.filter(id => st(id).mark === "wrong").length };
}
export const HUE = { "c": 215, "cpp": 265, "linux-process": 170, "linux-signal": 35, "linux-thread": 345, "linux-ipc": 195, "kernel-module": 20, "shell": 140, "alp": 95, "design-patterns": 300 };
// Topics come from the server already sorted by group (see THEORY_GROUPS in web/backend/config.py)
const GROUP_HUES = { "C": 215, "C++": 265, "Linux": 170, "Advanced Linux (ALP)": 95, "Build, debug & kernel": 20 };
export const topicHue = t => GROUP_HUES[t.group] ?? 300;
export function topicGroups() {
  const groups = new Map();
  for (const t of TOPICS) { const g = t.group || "Other"; if (!groups.has(g)) groups.set(g, []); groups.get(g).push(t); }
  return [...groups].map(([name, topics]) => ({ name, topics }));
}
export const hueStyle = hue => `--h:${hue}`;
const ICONS = {
  home: '<path d="M3 11l9-7 9 7v9a1 1 0 0 1-1 1h-5v-6H9v6H4a1 1 0 0 1-1-1z"/>',
  lab: '<path d="M9 3h6M10 3v6l-5.6 9.6A1.6 1.6 0 0 0 5.8 21h12.4a1.6 1.6 0 0 0 1.4-2.4L14 9V3"/><path d="M7.5 15h9"/>',
  theory: '<path d="M4 19.5V5a2 2 0 0 1 2-2h14v16H6a2 2 0 0 0-2 2z"/><path d="M8 7h8M8 11h6"/>',
  quiz: '<circle cx="12" cy="12" r="9"/><path d="M9.6 9.3a2.5 2.5 0 1 1 3.4 2.6c-.6.3-1 .8-1 1.6M12 17h.01"/>',
};
function svgIcon(k) {
  const t = document.createElement("template");
  t.innerHTML = `<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">${ICONS[k]}</svg>`;
  return t.content.firstChild;
}
export function ring(pct, color, label) {
  const r = 42, c = 2 * Math.PI * r;
  return h("div", { class: "ring", html: `<svg viewBox="0 0 100 100" width="100%" height="100%"><circle cx="50" cy="50" r="${r}" fill="none" stroke="var(--line)" stroke-width="10"/>` +
    `<circle cx="50" cy="50" r="${r}" fill="none" stroke="${color}" stroke-width="10" stroke-linecap="round" stroke-dasharray="${c * Math.min(pct, 100) / 100} ${c}" transform="rotate(-90 50 50)"/>` +
    `<text x="50" y="51" text-anchor="middle" dominant-baseline="middle" font-size="${label.length > 4 ? 18 : 22}" font-weight="700" fill="currentColor">${esc(label)}</text></svg>` });
}
export function theoryBanner() {
  return TOPICS.length ? null : h("div", { class: "banner", html:
    "<b>Could not load theory/quizzes from the server.</b><br>The running server is an old version: stop it (Ctrl+C in the terminal running <code>make web</code>), run <code>make web</code> again and reload the page." });
}

export function renderNav() {
  const wrong = qStats(TOPICS.flatMap(t => t.questions)).wrong;
  const M = [["home", "Overview"], ["lab", "Labs"], ["theory", "Theory"], ["quiz", "Quiz"]];
  $("#modes").replaceChildren(...M.map(([k, v]) =>
    h("button", { class: mode === k ? "on" : "", onclick: () => switchMode(k) }, svgIcon(k), v,
      k === "quiz" && wrong ? h("span", { class: "badge", title: "questions currently wrong" }, String(wrong)) : null)));
  const d = daysLeft();
  $("#brandSub").textContent = d > 0 ? `${d} day${d > 1 ? "s" : ""} until the assessment` : d === 0 ? "Exam day — good luck!" : "C++ & Linux System prep";
  if (mode === "lab") renderLabNav(); else renderTopicNav();
}
function switchMode(k) {
  if (k === "home") showHome();
  else if (k === "lab") showLabIndex();
  else if (k === "theory") showTheoryIndex();
  else showQuiz();
}
function renderTopicNav() {
  const all = TOPICS.flatMap(t => t.questions), stats = qStats(all);
  $("#bar").style.width = (all.length ? 100 * stats.done / all.length : 0) + "%";
  $("#stats").textContent = TOPICS.length ? `${stats.done}/${all.length} questions done · ${stats.wrong} wrong` : "Quizzes not loaded";
  $("#filters").replaceChildren();
  $("#nav").replaceChildren(...topicGroups().map(g => h("div", { class: "group", style: hueStyle(topicHue(g.topics[0])) },
    h("h2", {}, h("span", { class: "sw" }), `${g.name} · ${g.topics.reduce((n, t) => n + t.questions.length, 0)} questions`),
    g.topics.map(t => {
      const st = qStats(t.questions);
      const dot = !st.done ? "" : st.wrong ? "wrong" : st.done === st.total ? "right" : "pred";
      const active = mode === "theory" && currentTopic?.id === t.id;
      return h("div", { class: "item" + (active ? " active" : ""), style: hueStyle(topicHue(t)),
        onclick: () => mode === "quiz" ? startQuiz({ topics: [t.id], count: "all", filter: "all" }) : showTopic(t.id) },
        h("span", { class: "dot " + dot }),
        h("span", { class: "nm" }, t.title, h("small", {}, `${st.done}/${st.total} questions` + (st.wrong ? ` · ${st.wrong} wrong` : "")),
          h("div", { class: "minibar" }, h("div", { style: `width:${st.total ? 100 * st.done / st.total : 0}%` }))));
    }))));
}
function renderLabNav() {
  const c = counts();
  $("#bar").style.width = (c.total ? 100 * c.done / c.total : 0) + "%";
  $("#stats").textContent = `${c.done}/${c.total} labs graded · ${c.right} right · ${c.wrong} wrong`;
  const F = { all: "All", todo: "To do", wrong: "Guessed wrong" };
  $("#filters").replaceChildren(...Object.entries(F).map(([k, v]) =>
    h("button", { class: "small" + (filter === k ? " on" : ""), onclick: () => { set({ filter: k }); renderNav(); } }, v)));
  const nav = $("#nav");
  nav.replaceChildren();
  // On the index page: one row per topic. Inside a topic or a lab: that topic's labs only.
  const keep = l => {
    const s = store[l.id] || {};
    return filter === "all" || (filter === "todo" ? !s.mark && l.kind === "program" : s.mark === "wrong");
  };
  const topic = labTopic && labTopicById(labTopic);
  if (!topic) {
    for (const g of labGroups())
      nav.append(h("div", { class: "group", style: hueStyle(HUE[g.topics[0].dir] ?? 245) }, h("h2", {}, h("span", { class: "sw" }), g.name),
        g.topics.map(t => {
          const st = labStats(t.labs), shown = t.labs.filter(keep);
          if (!shown.length) return null;
          return h("div", { class: "item", style: hueStyle(HUE[t.dir] ?? 245), onclick: () => showLabTopic(t.id) },
            h("span", { class: "dot " + (st.wrong ? "wrong" : st.prog && st.done === st.prog ? "right" : st.done ? "pred" : "") }),
            h("span", { class: "nm" }, t.title, h("small", {}, filter === "all" ? `${t.labs.length} labs · ${st.done}/${st.prog} graded` : `${shown.length} ${filter === "todo" ? "to do" : "wrong"}`),
              h("div", { class: "minibar" }, h("div", { style: `width:${st.prog ? 100 * st.done / st.prog : 0}%` }))));
        })));
  } else {
    nav.append(h("div", { class: "item", onclick: showLabIndex }, h("span", { class: "nm" }, "← All lab topics")));
    nav.append(h("div", { class: "group", style: hueStyle(HUE[topic.dir] ?? 245) },
      h("h2", { style: "cursor:pointer", onclick: () => showLabTopic(topic.id) }, h("span", { class: "sw" }), topic.title),
      topic.labs.filter(keep).map(lab => {
        const s = store[lab.id] || {};
        const dot = s.mark || (s.pred?.trim() ? "pred" : "");
        const running = RUNS[lab.id] && !RUNS[lab.id].done;
        return h("div", { class: "item" + (current?.id === lab.id && location.hash.startsWith("#lab/") ? " active" : ""), onclick: () => go(lab.id) },
          h("span", { class: "dot " + dot }),
          h("span", { class: "nm" }, lab.name, h("small", {}, lab.title)),
          running ? h("span", { class: "run-badge", title: "running" }, "●") : null);
      })));
  }
  if (!nav.children.length) nav.append(h("div", { class: "empty" }, "No labs."));
}
