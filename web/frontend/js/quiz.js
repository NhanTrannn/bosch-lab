// Trắc nghiệm: chọn đề, làm bài (luyện tập / thi thử), kết quả, phím tắt.
"use strict";

// ---------- trắc nghiệm
let quizCfg = { topics: null, count: "20", exam: false, filter: "all" };
try { Object.assign(quizCfg, store.__quizCfg || {}); } catch {}
const shuffle = a => { for (let i = a.length - 1; i > 0; i--) { const j = Math.random() * (i + 1) | 0; [a[i], a[j]] = [a[j], a[i]]; } return a; };
const isRight = it => it.q.options.every((o, i) => o.ok === it.picked.has(i));
const letters = (it, pred) => it.order.map((oi, k) => pred(oi) ? String.fromCharCode(65 + k) : null).filter(Boolean).join(", ") || "(bỏ trống)";

function buildPool(cfg) {
  let qs = TOPICS.filter(t => cfg.topics.includes(t.id)).flatMap(t => t.questions.map(q => ({ ...q, topic: t })));
  if (cfg.filter === "new") qs = qs.filter(q => !qStore()[q.id]);
  if (cfg.filter === "wrong") qs = qs.filter(q => qStore()[q.id] && !qStore()[q.id].last);
  return qs;
}

function showQuiz() {
  mode = "quiz";
  setHash("quiz");
  renderNav();
  if (quiz && !quiz.finished) renderQuestion();
  else if (quiz) renderResult();
  else renderQuizSetup();
}

function renderQuizSetup() {
  quiz = null;
  const cfg = quizCfg;
  if (!cfg.topics || !cfg.topics.length) cfg.topics = TOPICS.map(t => t.id);
  cfg.topics = cfg.topics.filter(id => TOPICS.some(t => t.id === id));
  const save = () => { store.__quizCfg = cfg; saveStore(); };
  const info = h("span", { class: "muted" });
  const startBtn = h("button", { class: "primary", onclick: () => { save(); startQuiz({}); } }, "▶ Bắt đầu");
  const refresh = () => {
    const n = buildPool(cfg).length, take = cfg.count === "all" ? n : Math.min(n, +cfg.count);
    info.textContent = n ? `${take} câu (trong ${n} câu phù hợp)` : "Không có câu nào phù hợp — đổi chủ đề hoặc bộ lọc.";
    startBtn.disabled = !n;
  };
  const seg = (key, opts) => {
    const box = h("div", { class: "seg" });
    const paint = () => box.replaceChildren(...opts.map(([v, label]) =>
      h("button", { class: cfg[key] === v ? "on" : "", onclick: () => { cfg[key] = v; save(); paint(); refresh(); } }, label)));
    paint();
    return box;
  };
  const grid = h("div", { class: "topics-grid" }, TOPICS.map(t => {
    const st = qStats(t.questions);
    const cb = h("input", { type: "checkbox", checked: cfg.topics.includes(t.id) });
    cb.addEventListener("change", () => {
      cfg.topics = cb.checked ? [...cfg.topics, t.id] : cfg.topics.filter(x => x !== t.id);
      save(); refresh();
    });
    return h("label", { class: "topic-pick", style: hueStyle(topicHue(t)) }, cb, h("span", { class: "grow" }, t.title,
      h("small", {}, `${t.questions.length} câu · đã làm ${st.done}` + (st.wrong ? ` · ${st.wrong} đang sai` : ""))));
  }));
  const setAll = on => { cfg.topics = on ? TOPICS.map(t => t.id) : []; save(); renderQuizSetup(); };
  $("#main").replaceChildren(h("div", { class: "setup narrow" },
    theoryBanner(),
    h("div", { class: "eyebrow" }, `${TOPICS.length} chủ đề · ${TOPICS.reduce((n, t) => n + t.questions.length, 0)} câu`),
    h("div", { class: "title-row" }, h("h2", {}, "Trắc nghiệm")),
    h("p", { class: "muted", style: "margin:4px 0 0" }, "Thứ tự câu và đáp án được xáo trộn mỗi lần. Phím tắt: 1–4 hoặc A–D để chọn, Enter để kiểm tra / sang câu tiếp."),
    h("h3", {}, "Chủ đề"),
    h("div", { class: "row", style: "margin:0 0 8px" },
      h("button", { class: "small", onclick: () => setAll(true) }, "Chọn tất cả"),
      h("button", { class: "small", onclick: () => setAll(false) }, "Bỏ chọn")),
    grid,
    h("h3", {}, "Số câu"), seg("count", [["10", "10"], ["20", "20"], ["40", "40"], ["all", "Tất cả"]]),
    h("h3", {}, "Chọn câu"), seg("filter", [["all", "Tất cả"], ["new", "Chưa làm"], ["wrong", "Đang sai"]]),
    h("h3", {}, "Chế độ"), seg("exam", [[false, "Luyện tập — hiện đáp án ngay"], [true, "Thi thử — chấm khi nộp bài"]]),
    h("div", { class: "row", style: "margin-top:20px" }, startBtn, info),
    h("p", { class: "muted", style: "margin-top:20px" }, "Kết quả lưu trong trình duyệt. Câu \"đang sai\" là câu mà lần làm gần nhất bạn trả lời sai.")));
  refresh();
}

function startQuiz(over) {
  const cfg = { ...quizCfg, ...over };
  let qs = over.list || shuffle(buildPool(cfg));
  if (!over.list && cfg.count !== "all") qs = qs.slice(0, +cfg.count);
  if (!qs.length) { quizCfg = { ...quizCfg, ...over }; mode = "quiz"; renderNav(); renderQuizSetup(); return; }
  quiz = {
    cfg, i: 0, finished: false, start: Date.now(),
    items: shuffle([...qs]).map(q => ({ q, order: shuffle(q.options.map((_, i) => i)), picked: new Set(), checked: false })),
  };
  mode = "quiz";
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
      h("span", { class: "verdict " + (ok ? "ok" : "bad") }, ok ? "✓ Chính xác" : `✗ Chưa đúng — đáp án: ${letters(it, oi => it.q.options[oi].ok)}`),
      it.q.why ? h("div", { class: "md", html: md(it.q.why) }) : null);
  }
  const buttons = h("div", { class: "row", style: "margin-top:14px" },
    exam ? h("button", { disabled: i === 0, onclick: () => quizAction("prev") }, "← Câu trước") : null,
    !exam && it.q.multi && !it.checked ? h("button", { class: "primary", disabled: !it.picked.size, onclick: () => quizAction("check") }, "Kiểm tra") : null,
    exam || it.checked ? h("button", { class: "primary", onclick: () => quizAction("next") }, last ? (exam ? "Nộp bài" : "Xem kết quả") : "Câu tiếp →") : null,
    exam && !last ? h("button", { onclick: () => finishQuiz() }, "Nộp bài") : null,
    h("span", { class: "muted" }, h("kbd", {}, "Enter"), exam ? " câu tiếp" : it.checked ? " câu tiếp" : it.q.multi ? " kiểm tra" : ""),
    h("span", { class: "sp", style: "flex:1" }),
    h("button", { class: "small", onclick: () => { if (quiz.items.some(x => x.checked || x.picked.size)) finishQuiz(); else renderQuizSetup(); } }, exam ? "Dừng" : "Kết thúc"));

  const palette = h("div", { class: "palette" }, items.map((x, k) => {
    let cls = k === i ? "cur" : "";
    if (!exam && x.checked) cls += isRight(x) ? " ok" : " bad";
    else if (exam && x.picked.size) cls += " ans";
    return h("button", { class: cls, onclick: () => quizAction("goto", k) }, String(k + 1));
  }));

  $("#main").replaceChildren(h("div", { class: "quiz" },
    h("div", { class: "qtop" },
      h("b", {}, `Câu ${i + 1}/${items.length}`),
      h("div", { class: "qbar" }, h("div", { style: `width:${100 * answered / items.length}%` })),
      h("span", { class: "muted" }, exam ? `đã trả lời ${answered}` : `đúng ${right}/${answered}`),
      h("span", { class: "muted", id: "qclock" }, fmtTime(Date.now() - quiz.start))),
    h("div", { class: "card qcard", style: hueStyle(topicHue(it.q.topic)) }, h("div", { class: "bd" },
      ...questionBlock(it),
      it.q.multi ? h("div", { class: "qhint" }, "Chọn tất cả đáp án đúng" + (exam ? "" : ", rồi bấm Kiểm tra")) : null,
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
    h("div", { class: "title-row" }, h("h2", {}, "Kết quả")),
    h("div", { class: "card", style: "margin-top:12px" }, h("div", { class: "bd" },
      h("div", { class: "result-top" },
        ring(pct, `var(--${pct >= 80 ? "ok" : pct >= 50 ? "warn" : "bad"})`, `${pct}%`),
        h("div", {}, h("div", { style: "font-size:22px;font-weight:700" }, pct >= 80 ? "Rất tốt!" : pct >= 50 ? "Khá — ôn lại câu sai nhé" : "Cần ôn thêm"),
          h("div", { class: "muted" }, `${right}/${done.length} câu đúng · ${fmtTime((quiz.end || Date.now()) - quiz.start)}` + (done.length < items.length ? ` · bỏ ${items.length - done.length} câu` : ""))),
        h("div", { class: "tbl-wrap md" }, h("table", {}, h("tr", {}, h("th", {}, "Chủ đề"), h("th", {}, "Đúng")),
          Object.entries(byTopic).map(([t, [a, b]]) => h("tr", {}, h("td", {}, t), h("td", {}, `${a}/${b}`)))))),
      h("div", { class: "row", style: "margin-top:16px" },
        wrongItems.length ? h("button", { class: "primary", onclick: () => startQuiz({ list: wrongItems.map(x => x.q) }) }, `Làm lại ${wrongItems.length} câu sai`) : null,
        h("button", { onclick: () => renderQuizSetup() }, "Bài mới"),
        h("button", { onclick: () => startQuiz({}) }, "Làm lại với cấu hình cũ")))),
    wrongItems.length ? h("div", { class: "card" }, h("div", { class: "hd" }, "Các câu sai"),
      h("div", { class: "bd" }, wrongItems.map(x => h("div", { class: "review" },
        ...questionBlock(x),
        x.order.map((oi, k) => h("div", { class: x.q.options[oi].ok ? "correct" : x.picked.has(oi) ? "yours" : "" },
          `${String.fromCharCode(65 + k)}. `, h("span", { html: inline(x.q.options[oi].text) }),
          x.q.options[oi].ok ? " ✓" : x.picked.has(oi) ? " ✗ (bạn chọn)" : "")),
        x.q.why ? h("div", { class: "md muted", html: md(x.q.why) }) : null))))
      : h("p", { class: "verdict ok" }, "Không sai câu nào 🎉")));
  $("#main").scrollTop = 0;
}

document.addEventListener("keydown", e => {
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
