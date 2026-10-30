// Quiz: pick a topic, answer (practice / mock exam), results, keyboard shortcuts.
import { $, h, mode, quiz, saveStore, set, setHash, store, TOPICS } from "./core.js";
import { inline, md } from "./render.js";
import { hueStyle, renderNav, ring, theoryBanner, topicGroups, topicHue } from "./nav.js";
import { qStats, qStore } from "./theory.js";

// ---------- quiz
// Default: every question, practice mode. Mock exam only when asked for (the "mock exam" button on the Overview page).
let quizCfg = { topics: null, count: "all", exam: false, filter: "all" };
delete store.__quizCfg;
const shuffle = a => { for (let i = a.length - 1; i > 0; i--) { const j = Math.random() * (i + 1) | 0; [a[i], a[j]] = [a[j], a[i]]; } return a; };
const isRight = it => it.q.options.every((o, i) => o.ok === it.picked.has(i));
const letters = (it, pred) => it.order.map((oi, k) => pred(oi) ? String.fromCharCode(65 + k) : null).filter(Boolean).join(", ") || "(blank)";

function buildPool(cfg) {
  let qs = TOPICS.filter(t => cfg.topics.includes(t.id)).flatMap(t => t.questions.map(q => ({ ...q, topic: t })));
  if (cfg.filter === "new") qs = qs.filter(q => !qStore()[q.id]);
  if (cfg.filter === "wrong") qs = qs.filter(q => qStore()[q.id] && !qStore()[q.id].last);
  return qs;
}

export function showQuiz() {
  set({ mode: "quiz" });
  setHash("quiz");
  renderNav();
  if (quiz && !quiz.finished) renderQuestion();
  else if (quiz) renderResult();
  else renderQuizSetup();
}

function renderQuizSetup() {
  set({ quiz: null });
  const go = over => startQuiz({ count: "all", filter: "all", exam: false, ...over });
  const allIds = TOPICS.map(t => t.id);
  const totalQ = TOPICS.reduce((n, t) => n + t.questions.length, 0);
  const allWrong = qStats(TOPICS.flatMap(t => t.questions)).wrong;
  const card = t => {
    const st = qStats(t.questions), pct = st.total ? 100 * st.done / st.total : 0;
    return h("div", { class: "card topic-card", style: hueStyle(topicHue(t)), role: "button", tabindex: "0",
      onclick: () => go({ topics: [t.id] }),
      onkeydown: e => { if (e.key === "Enter" || e.key === " ") { e.preventDefault(); go({ topics: [t.id] }); } } },
      h("div", { class: "eyebrow" }, `${t.questions.length} questions`),
      h("div", { class: "t" }, t.title),
      h("div", { class: "bar" }, h("div", { style: `width:${pct}%` })),
      h("div", { class: "foot" },
        h("span", { class: "muted" }, st.done ? `${st.done}/${st.total} done` : "not started"),
        st.wrong ? h("button", { class: "small wrong-chip", title: "Redo only the questions of this topic that are currently wrong",
          onclick: e => { e.stopPropagation(); go({ topics: [t.id], filter: "wrong" }); } }, `↻ ${st.wrong} wrong`) : null));
  };
  $("#main").replaceChildren(h("div", { class: "setup" },
    theoryBanner(),
    h("div", { class: "eyebrow" }, `${TOPICS.length} topics · ${totalQ} questions`),
    h("div", { class: "title-row" }, h("h2", {}, "Quiz"),
      h("span", { class: "sp", style: "flex:1" }),
      h("button", { class: "small", onclick: () => go({ topics: allIds }) }, `🔀 Mix all topics (${totalQ})`),
      allWrong ? h("button", { class: "small", onclick: () => go({ topics: allIds, filter: "wrong" }) }, `↻ Review ${allWrong} wrong`) : null),
    h("p", { class: "muted", style: "margin:4px 0 14px" }, "Click a topic to answer all of its questions, or mix a whole group (questions and options are shuffled). Shortcuts: 1–4 or A–D to pick, Enter for the next question, Esc to exit."),
    topicGroups().map(g => groupSection(g, card))));
}

// One group (C, C++, Linux, …): heading with mix / review-wrong buttons, then its topic cards
function groupSection(g, card) {
  const ids = g.topics.map(t => t.id), qs = g.topics.flatMap(t => t.questions), st = qStats(qs);
  const go = over => startQuiz({ topics: ids, count: "all", filter: "all", exam: false, ...over });
  return h("section", { class: "quiz-group", style: hueStyle(topicHue(g.topics[0])) },
    h("div", { class: "group-head" },
      h("span", { class: "sw" }),
      h("h3", {}, g.name),
      h("span", { class: "muted" }, `${g.topics.length} topic${g.topics.length > 1 ? "s" : ""} · ${st.done}/${st.total} done`),
      h("span", { class: "sp", style: "flex:1" }),
      g.topics.length > 1 ? h("button", { class: "small", onclick: () => go({}) }, `🔀 Mix ${g.name} (${st.total})`) : null,
      st.wrong ? h("button", { class: "small wrong-chip", onclick: () => go({ filter: "wrong" }) }, `↻ ${st.wrong} wrong`) : null),
    h("div", { class: "topic-cards" }, g.topics.map(card)));
}

export function startQuiz(over) {
  const cfg = { ...quizCfg, ...over };
  let qs = over.list || shuffle(buildPool(cfg));
  if (!over.list && cfg.count !== "all") qs = qs.slice(0, +cfg.count);
  if (!qs.length) { quizCfg = { ...quizCfg, ...over }; set({ mode: "quiz" }); renderNav(); renderQuizSetup(); return; }
  set({ mode: "quiz", quiz: {
    cfg, i: 0, finished: false, start: Date.now(),
    items: shuffle([...qs]).map(q => ({ q, order: shuffle(q.options.map((_, i) => i)), picked: new Set(), checked: false })),
  } });
  setHash("quiz");
  renderNav();
  renderQuestion();
}

function record(it) {
  const s = qStore()[it.q.id] || { seen: 0, wrong: 0 };
  const ok = isRight(it);
  qStore()[it.q.id] = { seen: s.seen + 1, wrong: s.wrong + (ok ? 0 : 1), last: ok };
  saveStore();
}

function quizAction(kind, arg) {
  if (!quiz || quiz.finished) return;
  const it = quiz.items[quiz.i], exam = quiz.cfg.exam;
  if (kind === "pick") {
    if (it.checked && !exam) return;
    if (it.q.multi) it.picked.has(arg) ? it.picked.delete(arg) : it.picked.add(arg);
    else it.picked = new Set([arg]);
    if (!exam && !it.q.multi) return quizAction("check");
  } else if (kind === "check") {
    if (exam || it.checked || !it.picked.size) return;
    it.checked = true;
    record(it);
  } else if (kind === "next") {
    if (!exam && !it.checked) return quizAction("check");
    if (quiz.i < quiz.items.length - 1) quiz.i++;
    else return finishQuiz();
  } else if (kind === "prev") {
    if (quiz.i > 0) quiz.i--;
  } else if (kind === "goto") {
    quiz.i = arg;
  }
  renderQuestion();
}

// Back to the topic list. Practice: checked answers are already saved. Mock exam: an unsubmitted exam is discarded.
function exitQuiz() {
  if (quiz && !quiz.finished && quiz.cfg.exam && quiz.items.some(x => x.picked.size)
      && !confirm("Your unsubmitted mock exam will be discarded. Exit anyway?")) return;
  set({ quiz: null });
  renderNav();
  renderQuizSetup();
}

function finishQuiz() {
  if (quiz.cfg.exam) quiz.items.forEach(it => { it.checked = true; record(it); });
  quiz.finished = true;
  quiz.end = Date.now();
  renderNav();
  renderResult();
}

const fmtTime = ms => { const s = Math.round(ms / 1000); return `${Math.floor(s / 60)}:${String(s % 60).padStart(2, "0")}`; };

function questionBlock(it) {
  return [
    h("div", { class: "eyebrow" }, it.q.topic.title),
    h("div", { class: "qtext", html: inline(it.q.text) }),
    it.q.body ? h("div", { class: "md", html: md(it.q.body) }) : null,
  ];
}

function renderQuestion() {
  const { items, i, cfg } = quiz, it = items[i], exam = cfg.exam;
  const reveal = it.checked && !exam;
  const opts = it.order.map((oi, k) => {
    const o = it.q.options[oi];
    let cls = "opt" + (it.picked.has(oi) ? " picked" : "");
    if (reveal) cls += o.ok ? " right" : it.picked.has(oi) ? " wrong" : "";
    return h("button", { class: cls, disabled: reveal, onclick: () => quizAction("pick", oi) },
      h("span", { class: "key" }, String.fromCharCode(65 + k)), h("span", { html: inline(o.text) }));
  });
  const answered = items.filter(x => exam ? x.picked.size : x.checked).length;
  const right = items.filter(x => x.checked && !exam && isRight(x)).length;
  const last = i === items.length - 1;

  let feedback = null;
  if (reveal) {
    const ok = isRight(it);
    feedback = h("div", { class: "feedback " + (ok ? "ok" : "bad") },
      h("span", { class: "verdict " + (ok ? "ok" : "bad") }, ok ? "✓ Correct" : `✗ Incorrect — answer: ${letters(it, oi => it.q.options[oi].ok)}`),
      it.q.why ? h("div", { class: "md", html: md(it.q.why) }) : null);
  }
  const buttons = h("div", { class: "row", style: "margin-top:14px" },
    exam ? h("button", { disabled: i === 0, onclick: () => quizAction("prev") }, "← Previous") : null,
    !exam && it.q.multi && !it.checked ? h("button", { class: "primary", disabled: !it.picked.size, onclick: () => quizAction("check") }, "Check") : null,
    exam || it.checked ? h("button", { class: "primary", onclick: () => quizAction("next") }, last ? (exam ? "Submit" : "See results") : "Next →") : null,
    exam && !last ? h("button", { onclick: () => finishQuiz() }, "Submit") : null,
    h("span", { class: "muted" }, h("kbd", {}, "Enter"), exam ? " next" : it.checked ? " next" : it.q.multi ? " check" : ""),
    h("span", { class: "sp", style: "flex:1" }),
    h("button", { class: "small", onclick: () => { if (quiz.items.some(x => x.checked || x.picked.size)) finishQuiz(); else exitQuiz(); } }, exam ? "Submit" : "See results"));

  const palette = h("div", { class: "palette" }, items.map((x, k) => {
    let cls = k === i ? "cur" : "";
    if (!exam && x.checked) cls += isRight(x) ? " ok" : " bad";
    else if (exam && x.picked.size) cls += " ans";
    return h("button", { class: cls, onclick: () => quizAction("goto", k) }, String(k + 1));
  }));

  $("#main").replaceChildren(h("div", { class: "quiz" },
    h("div", { class: "qtop" },
      h("button", { class: "small", title: "Back to the topic list (Esc)", onclick: exitQuiz }, "← Exit"),
      h("b", {}, `Question ${i + 1}/${items.length}`),
      h("div", { class: "qbar" }, h("div", { style: `width:${100 * answered / items.length}%` })),
      h("span", { class: "muted" }, exam ? `${answered} answered` : `${right}/${answered} correct`),
      h("span", { class: "muted", id: "qclock" }, fmtTime(Date.now() - quiz.start))),
    h("div", { class: "card qcard", style: hueStyle(topicHue(it.q.topic)) }, h("div", { class: "bd" },
      ...questionBlock(it),
      it.q.multi ? h("div", { class: "qhint" }, "Select all correct answers" + (exam ? "" : ", then press Check")) : null,
      opts, feedback, buttons)),
    palette));
}
setInterval(() => { const el = document.getElementById("qclock"); if (el && quiz && !quiz.finished) el.textContent = fmtTime(Date.now() - quiz.start); }, 1000);

function renderResult() {
  const items = quiz.items, done = items.filter(x => x.checked);
  const right = done.filter(isRight).length, wrongItems = done.filter(x => !isRight(x));
  const pct = done.length ? Math.round(100 * right / done.length) : 0;
  const byTopic = {};
  for (const x of done) { const b = (byTopic[x.q.topic.title] ||= [0, 0]); b[1]++; if (isRight(x)) b[0]++; }
  $("#main").replaceChildren(h("div", { class: "quiz" },
    h("div", { class: "title-row" }, h("h2", {}, "Results")),
    h("div", { class: "card", style: "margin-top:12px" }, h("div", { class: "bd" },
      h("div", { class: "result-top" },
        ring(pct, `var(--${pct >= 80 ? "ok" : pct >= 50 ? "warn" : "bad"})`, `${pct}%`),
        h("div", {}, h("div", { style: "font-size:22px;font-weight:700" }, pct >= 80 ? "Excellent!" : pct >= 50 ? "Not bad — review the ones you missed" : "Needs more review"),
          h("div", { class: "muted" }, `${right}/${done.length} correct · ${fmtTime((quiz.end || Date.now()) - quiz.start)}` + (done.length < items.length ? ` · ${items.length - done.length} skipped` : ""))),
        h("div", { class: "tbl-wrap md" }, h("table", {}, h("tr", {}, h("th", {}, "Topic"), h("th", {}, "Correct")),
          Object.entries(byTopic).map(([t, [a, b]]) => h("tr", {}, h("td", {}, t), h("td", {}, `${a}/${b}`)))))),
      h("div", { class: "row", style: "margin-top:16px" },
        wrongItems.length ? h("button", { class: "primary", onclick: () => startQuiz({ list: wrongItems.map(x => x.q) }) }, `Redo ${wrongItems.length} wrong`) : null,
        h("button", { onclick: () => startQuiz({ ...quiz.cfg }) }, "Redo this quiz"),
        h("button", { onclick: () => renderQuizSetup() }, "Choose another topic")))),
    wrongItems.length ? h("div", { class: "card" }, h("div", { class: "hd" }, "Wrong answers"),
      h("div", { class: "bd" }, wrongItems.map(x => h("div", { class: "review" },
        ...questionBlock(x),
        x.order.map((oi, k) => h("div", { class: x.q.options[oi].ok ? "correct" : x.picked.has(oi) ? "yours" : "" },
          `${String.fromCharCode(65 + k)}. `, h("span", { html: inline(x.q.options[oi].text) }),
          x.q.options[oi].ok ? " ✓" : x.picked.has(oi) ? " ✗ (your pick)" : "")),
        x.q.why ? h("div", { class: "md muted", html: md(x.q.why) }) : null))))
      : h("p", { class: "verdict ok" }, "No wrong answers 🎉")));
  $("#main").scrollTop = 0;
}

document.addEventListener("keydown", e => {
  if (mode === "quiz" && quiz && e.key === "Escape" && !/^(INPUT|TEXTAREA)$/.test(document.activeElement?.tagName)) { e.preventDefault(); return exitQuiz(); }
  if (mode !== "quiz" || !quiz || quiz.finished || e.ctrlKey || e.metaKey || e.altKey) return;
  if (/^(INPUT|TEXTAREA)$/.test(document.activeElement?.tagName)) return;
  const it = quiz.items[quiz.i];
  const k = e.key.toLowerCase();
  let idx = "1234567".indexOf(k);
  if (idx < 0) idx = "abcdefg".indexOf(k);
  if (idx >= 0 && idx < it.order.length) { e.preventDefault(); quizAction("pick", it.order[idx]); }
  else if (e.key === "Enter") { e.preventDefault(); it.q.multi && !it.checked && !quiz.cfg.exam ? quizAction("check") : quizAction("next"); }
  else if (e.key === "ArrowRight") quizAction(quiz.cfg.exam || it.checked ? "next" : "check");
  else if (e.key === "ArrowLeft" && quiz.cfg.exam) quizAction("prev");
});
