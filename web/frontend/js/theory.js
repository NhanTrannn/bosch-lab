// Theory page for one topic, with a table of contents.
import { $, h, ROOT, saveStore, set, setHash, store, TOPICS } from "./core.js";
import { md } from "./render.js";
import { hueStyle, renderNav, theoryBanner, topicGroups, topicHue } from "./nav.js";
import { startQuiz } from "./quiz.js";

// ---------- theory
export const qStore = () => (store.__quiz ||= {});
export function qStats(qs) {
  let done = 0, wrong = 0;
  for (const q of qs) { const s = qStore()[q.id]; if (s) { done++; if (!s.last) wrong++; } }
  return { done, wrong, total: qs.length };
}

// Topics opened at least once, so the index can show what is left to read
const readStore = () => (store.__read ||= {});

// Theory index: every topic grouped like the quiz page, open any of them in any order
export function showTheoryIndex() {
  set({ mode: "theory", currentTopic: null });
  setHash("theory");
  renderNav();
  if (!TOPICS.length) { $("#main").replaceChildren(h("div", { class: "narrow" }, theoryBanner())); return; }
  const nRead = TOPICS.filter(t => readStore()[t.id]).length;
  const sections = t => (t.theory.match(/^### /gm) || []).length;
  const card = t => {
    const st = qStats(t.questions), read = readStore()[t.id];
    return h("div", { class: "card topic-card", style: hueStyle(topicHue(t)), role: "button", tabindex: "0",
      onclick: () => showTopic(t.id),
      onkeydown: e => { if (e.key === "Enter" || e.key === " ") { e.preventDefault(); showTopic(t.id); } } },
      h("div", { class: "eyebrow" }, `${sections(t)} sections · ${t.questions.length} questions`),
      h("div", { class: "t" }, t.title),
      h("div", { class: "bar" }, h("div", { style: `width:${st.total ? 100 * st.done / st.total : 0}%` })),
      h("div", { class: "foot" },
        h("span", { class: read ? "read-chip" : "muted" }, read ? "✓ read" : "not read yet"),
        h("button", { class: "small", style: "margin-left:auto", title: "Quiz this topic",
          onclick: e => { e.stopPropagation(); startQuiz({ topics: [t.id], count: "all", filter: "all" }); } }, "✍️ Quiz")));
  };
  const section = g => {
    const left = g.topics.find(t => !readStore()[t.id]);
    return h("section", { class: "quiz-group", style: hueStyle(topicHue(g.topics[0])) },
      h("div", { class: "group-head" },
        h("span", { class: "sw" }),
        h("h3", {}, g.name),
        h("span", { class: "muted" }, `${g.topics.filter(t => readStore()[t.id]).length}/${g.topics.length} read`),
        h("span", { class: "sp", style: "flex:1" }),
        left ? h("button", { class: "small", onclick: () => showTopic(left.id) }, "Next unread →") : null),
      h("div", { class: "topic-cards" }, g.topics.map(card)));
  };
  $("#main").replaceChildren(h("div", { class: "setup" },
    h("div", { class: "eyebrow" }, `${TOPICS.length} topics · ${nRead} read`),
    h("div", { class: "title-row" }, h("h2", {}, "Theory")),
    h("p", { class: "muted", style: "margin:4px 0 14px" }, "Pick any topic to read; topics are grouped like the quiz. Each topic page has a table of contents and a quiz button."),
    topicGroups().map(section)));
  $("#main").scrollTop = 0;
}

// Outline numbering (rules in data/README.md): "### " → I, II…; "#### " → 1, 2… inside it;
// a list right under a "### " counts 1, 2…, a list under a "#### " counts a, b… (nested lists go one level deeper, see CSS)
const ROMAN = [[10, "X"], [9, "IX"], [5, "V"], [4, "IV"], [1, "I"]];
const roman = n => ROMAN.reduce((s, [v, r]) => { while (n >= v) { s += r; n -= v; } return s; }, "");
// The number goes in a badge; "I. Title" is kept for the table of contents
const label = (el, num) => { el.dataset.toc = `${num}. ${el.textContent}`; el.prepend(h("span", { class: "num" }, num)); };
function numberOutline(body) {
  let part = 0, sec = 0;
  for (const el of body.children) {
    if (el.tagName === "H4") { part++; sec = 0; label(el, roman(part)); }
    else if (el.tagName === "H5") { sec++; label(el, String(sec)); }
    else if (el.tagName === "UL" || el.tagName === "OL") el.classList.add(sec ? "lv-alpha" : "lv-num");
  }
}

// sec: index of a "### " section to scroll to (search results link to sections)
export function showTopic(id, sec) {
  const t = TOPICS.find(x => x.id === id);
  if (!t) return showTheoryIndex();
  set({ mode: "theory", currentTopic: t });
  if (!readStore()[t.id]) { readStore()[t.id] = true; saveStore(); }
  setHash("theory/" + t.id);
  renderNav();
  const st = qStats(t.questions), idx = TOPICS.indexOf(t);
  const body = h("div", { class: "bd md", html: md(t.theory) });
  numberOutline(body);
  const heads = [...body.querySelectorAll("h4")];
  heads.forEach((el, i) => { el.id = `sec-${i}`; });
  const toc = h("nav", { class: "toc" }, h("b", {}, "On this page"),
    [...body.querySelectorAll("h4, h5")].map(el => h("a", { href: "#", class: el.tagName === "H5" ? "sub" : null,
      onclick: e => { e.preventDefault(); el.scrollIntoView({ behavior: "smooth", block: "start" }); } }, el.dataset.toc || el.textContent)));
  $("#main").replaceChildren(h("div", { class: "page theory", style: hueStyle(topicHue(t)) },
    h("div", { class: "topic-hero" },
      h("div", { class: "row", style: "margin-bottom:8px" },
        h("button", { class: "small", onclick: showTheoryIndex }, "← All topics"),
        h("span", { class: "eyebrow" }, `Theory · ${t.group} · topic ${idx + 1}/${TOPICS.length}`)),
      h("h2", {}, t.title),
      h("div", { class: "row", style: "margin-top:12px" },
        h("button", { class: "primary", onclick: () => startQuiz({ topics: [t.id], count: "all", filter: "all" }) }, `▶ Take the ${t.questions.length}-question quiz`),
        st.wrong ? h("button", { onclick: () => startQuiz({ topics: [t.id], count: "all", filter: "wrong" }) }, `Redo ${st.wrong} wrong question(s)`) : null,
        h("span", { class: "muted" }, `${st.done}/${st.total} questions done · `, h("a", { href: `vscode://file${ROOT}/${t.path}` }, "edit in VS Code ↗")))),
    h("div", { class: "theory-layout" },
      h("div", {},
        h("div", { class: "card" }, body),
        h("div", { class: "row", style: "margin-top:16px" },
          h("button", { disabled: idx <= 0, onclick: () => showTopic(TOPICS[idx - 1].id) }, "← Previous topic"),
          h("button", { disabled: idx >= TOPICS.length - 1, onclick: () => showTopic(TOPICS[idx + 1].id) }, "Next topic →"),
          h("button", { class: "primary", style: "margin-left:auto", onclick: () => startQuiz({ topics: [t.id], count: "all", filter: "all" }) }, "Quiz this topic"))),
      toc)));
  $("#main").scrollTop = 0;
  if (sec != null) heads[sec]?.scrollIntoView({ block: "start" });
}
