// Overview page: countdown, study plan, progress, shortcuts.
"use strict";

// ---------- overview
const EXAM = new Date(2026, 9, 29); // Thursday 29 Oct 2026
const PLAN = [
  [new Date(2026, 9, 26), "Monday", ["01-c", "02-cpp"], ["01-c", "02-cpp-oop", "03-cpp-stl-modern"]],
  [new Date(2026, 9, 27), "Tuesday", ["03-process", "04-signal", "08-shell"], ["04-process-memory", "05-signal-ipc", "07-linux-shell-fs"]],
  [new Date(2026, 9, 28), "Wednesday", ["05-thread", "06-ipc", "07-kernel-module"], ["06-thread-sync", "08-build-debug", "09-kernel-embedded"]],
];
function daysLeft() { const t = new Date(); t.setHours(0, 0, 0, 0); return Math.round((EXAM - t) / 864e5); }

function showHome() {
  mode = "home";
  setHash("home");
  renderNav();
  const today = new Date(); today.setHours(0, 0, 0, 0);
  const d = daysLeft();
  const plan = PLAN.find(p => p[0].getTime() === today.getTime()) || (today < PLAN[0][0] ? PLAN[0] : null);
  const allQ = TOPICS.flatMap(t => t.questions), qs = qStats(allQ);
  const acc = qs.done ? Math.round(100 * (qs.done - qs.wrong) / qs.done) : 0;
  const c = counts();
  const nextLab = LABS.find(l => l.kind === "program" && !(store[l.id] || {}).mark);
  const firstOf = dir => LABS.find(l => l.dir === dir);

  const todayRow = h("div", { class: "today" });
  if (plan) {
    todayRow.append(h("b", {}, `${plan[1]} plan:`),
      ...plan[2].filter(firstOf).map(dir => h("span", { class: "pill", onclick: () => go(firstOf(dir).id) }, `🧪 ${dir}`)),
      ...plan[3].map(id => TOPICS.find(t => t.id === id)).filter(Boolean).map(t => h("span", { class: "pill", onclick: () => showTopic(t.id) }, `📘 ${t.title}`)));
  } else if (d === 0) {
    todayRow.append(h("b", {}, "This morning:"), h("span", {}, " reread ANSWERS.md and redo the questions you got wrong."),
      h("span", { class: "pill", onclick: () => startQuiz({ topics: TOPICS.map(t => t.id), count: "all", filter: "wrong", exam: false }) }, "🔁 Review wrong answers"));
  }

  const kpi = (ringEl, val, lbl, btn) => h("div", { class: "card kpi" }, ringEl, h("div", {}, h("div", { class: "val" }, val), h("div", { class: "lbl" }, lbl), btn));
  const pctLab = c.total ? Math.round(100 * c.done / c.total) : 0, pctQ = allQ.length ? Math.round(100 * qs.done / allQ.length) : 0;

  const topicTile = t => {
    const st = qStats(t.questions), pct = st.total ? 100 * st.done / st.total : 0;
    return h("div", { class: "card tile", style: hueStyle(topicHue(t)) },
      h("div", { class: "eyebrow" }, `Topic ${TOPICS.indexOf(t) + 1}`),
      h("div", { class: "t" }, t.title),
      h("div", { class: "bar" }, h("div", { style: `width:${pct}%` })),
      h("div", { class: "muted" }, `${st.done}/${st.total} questions` + (st.wrong ? ` · ${st.wrong} wrong` : st.done === st.total && st.total ? " · done ✓" : "")),
      h("div", { class: "acts" },
        h("button", { class: "small", onclick: () => showTopic(t.id) }, "📘 Theory"),
        h("button", { class: "small", onclick: () => startQuiz({ topics: [t.id], count: "all", filter: "all" }) }, "✍️ Quiz")));
  };
  const dirTile = dir => {
    const labs = LABS.filter(l => l.dir === dir), prog = labs.filter(l => l.kind === "program");
    const done = prog.filter(l => (store[l.id] || {}).mark).length, wrongN = prog.filter(l => (store[l.id] || {}).mark === "wrong").length;
    return h("div", { class: "card tile", style: hueStyle(HUE[dir] ?? 245) },
      h("div", { class: "eyebrow" }, dir),
      h("div", { class: "t" }, DIR_NAMES[dir] || dir),
      prog.length ? h("div", { class: "bar" }, h("div", { style: `width:${100 * done / prog.length}%` })) : null,
      h("div", { class: "muted" }, prog.length ? `${done}/${prog.length} labs graded` + (wrongN ? ` · ${wrongN} guessed wrong` : "") : "reading / command practice"),
      h("div", { class: "acts" }, h("button", { class: "small", onclick: () => go((prog.find(l => !(store[l.id] || {}).mark) || labs[0]).id) }, prog.length ? "Continue" : "Open")));
  };

  $("#main").replaceChildren(h("div", { class: "page" },
    theoryBanner(),
    h("div", { class: "hero" },
      h("div", { class: "top" },
        h("div", {}, h("div", { style: "opacity:.85;font-weight:600;letter-spacing:.06em;font-size:12px;text-transform:uppercase" }, "Assessment C++ & Linux System"),
          h("h2", {}, d > 0 ? "Predict first, run second, understand why." : d === 0 ? "Exam day. Stay calm and read each question carefully!" : "Congratulations on finishing the exam!"),
          h("p", {}, "Thursday 29 Oct 2026 · " + `${LABS.length} labs · ${allQ.length} quiz questions · ${TOPICS.length} theory topics`)),
        d >= 0 ? h("div", { class: "count" }, h("b", {}, String(d)), h("span", {}, d ? (d > 1 ? "days left" : "day left") : "today")) : null),
      todayRow),
    h("div", { class: "kpis" },
      kpi(ring(pctLab, "#22c55e", `${pctLab}%`), `${c.done}/${c.total}`, "labs self-graded",
        nextLab ? h("button", { class: "small", onclick: () => go(nextLab.id) }, "Next lab →") : null),
      kpi(ring(pctQ, "var(--accent)", `${pctQ}%`), `${qs.done}/${allQ.length}`, qs.done ? `questions done · ${acc}% correct` : "quiz questions done",
        h("button", { class: "small primary", disabled: !TOPICS.length, onclick: () => startQuiz({ topics: TOPICS.map(t => t.id), count: "20", filter: "all", exam: true }) }, "20-question mock exam")),
      kpi(ring(qs.done ? 100 * qs.wrong / qs.done : 0, "var(--bad)", String(qs.wrong)), `${qs.wrong} question${qs.wrong === 1 ? "" : "s"}`, "currently wrong — review before the exam",
        h("button", { class: "small", disabled: !qs.wrong, onclick: () => startQuiz({ topics: TOPICS.map(t => t.id), count: "all", filter: "wrong", exam: false }) }, "Review wrong answers"))),
    TOPICS.length ? [h("div", { class: "section-title" }, h("h3", {}, "Theory & quizzes"), h("span", { class: "muted" }, "read the summary, then answer the topic's questions")),
      h("div", { class: "tiles" }, TOPICS.map(topicTile))] : null,
    h("div", { class: "section-title" }, h("h3", {}, "Labs"), h("span", { class: "muted" }, "predict the output → run → compare → self-grade")),
    h("div", { class: "tiles" }, [...new Set(LABS.map(l => l.dir))].map(dirTile))));
  $("#main").scrollTop = 0;
}
