// Trang Tổng quan: đếm ngược, lịch ôn, tiến độ, lối tắt.
"use strict";

// ---------- tổng quan
const EXAM = new Date(2026, 9, 29); // Thứ Năm 29/10/2026
const PLAN = [
  [new Date(2026, 9, 26), "Thứ Hai", ["01-c", "02-cpp"], ["01-c", "02-cpp-oop", "03-cpp-stl-modern"]],
  [new Date(2026, 9, 27), "Thứ Ba", ["03-process", "04-signal", "08-shell"], ["04-process-memory", "05-signal-ipc", "07-linux-shell-fs"]],
  [new Date(2026, 9, 28), "Thứ Tư", ["05-thread", "06-ipc", "07-kernel-module"], ["06-thread-sync", "08-build-debug", "09-kernel-embedded"]],
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
    todayRow.append(h("b", {}, `Lịch ${plan[1]}:`),
      ...plan[2].filter(firstOf).map(dir => h("span", { class: "pill", onclick: () => go(firstOf(dir).id) }, `🧪 ${dir}`)),
      ...plan[3].map(id => TOPICS.find(t => t.id === id)).filter(Boolean).map(t => h("span", { class: "pill", onclick: () => showTopic(t.id) }, `📘 ${t.title}`)));
  } else if (d === 0) {
    todayRow.append(h("b", {}, "Sáng nay:"), h("span", {}, " đọc lại ANSWERS.md và làm lại các câu từng sai."),
      h("span", { class: "pill", onclick: () => startQuiz({ topics: TOPICS.map(t => t.id), count: "all", filter: "wrong", exam: false }) }, "🔁 Ôn câu sai"));
  }

  const kpi = (ringEl, val, lbl, btn) => h("div", { class: "card kpi" }, ringEl, h("div", {}, h("div", { class: "val" }, val), h("div", { class: "lbl" }, lbl), btn));
  const pctLab = c.total ? Math.round(100 * c.done / c.total) : 0, pctQ = allQ.length ? Math.round(100 * qs.done / allQ.length) : 0;

  const topicTile = t => {
    const st = qStats(t.questions), pct = st.total ? 100 * st.done / st.total : 0;
    return h("div", { class: "card tile", style: hueStyle(topicHue(t)) },
      h("div", { class: "eyebrow" }, `Chủ đề ${TOPICS.indexOf(t) + 1}`),
      h("div", { class: "t" }, t.title),
      h("div", { class: "bar" }, h("div", { style: `width:${pct}%` })),
      h("div", { class: "muted" }, `${st.done}/${st.total} câu` + (st.wrong ? ` · ${st.wrong} đang sai` : st.done === st.total && st.total ? " · xong ✓" : "")),
      h("div", { class: "acts" },
        h("button", { class: "small", onclick: () => showTopic(t.id) }, "📘 Lý thuyết"),
        h("button", { class: "small", onclick: () => startQuiz({ topics: [t.id], count: "all", filter: "all" }) }, "✍️ Trắc nghiệm")));
  };
  const dirTile = dir => {
    const labs = LABS.filter(l => l.dir === dir), prog = labs.filter(l => l.kind === "program");
    const done = prog.filter(l => (store[l.id] || {}).mark).length, wrongN = prog.filter(l => (store[l.id] || {}).mark === "wrong").length;
    return h("div", { class: "card tile", style: hueStyle(HUE[dir] ?? 245) },
      h("div", { class: "eyebrow" }, dir),
      h("div", { class: "t" }, DIR_NAMES[dir] || dir),
      prog.length ? h("div", { class: "bar" }, h("div", { style: `width:${100 * done / prog.length}%` })) : null,
      h("div", { class: "muted" }, prog.length ? `${done}/${prog.length} bài đã chấm` + (wrongN ? ` · ${wrongN} đoán sai` : "") : "bài đọc / thực hành lệnh"),
      h("div", { class: "acts" }, h("button", { class: "small", onclick: () => go((prog.find(l => !(store[l.id] || {}).mark) || labs[0]).id) }, prog.length ? "Làm tiếp" : "Mở")));
  };

  $("#main").replaceChildren(h("div", { class: "page" },
    theoryBanner(),
    h("div", { class: "hero" },
      h("div", { class: "top" },
        h("div", {}, h("div", { style: "opacity:.85;font-weight:600;letter-spacing:.06em;font-size:12px;text-transform:uppercase" }, "Assessment C++ & Linux System"),
          h("h2", {}, d > 0 ? "Đoán trước, chạy sau, hiểu tại sao." : d === 0 ? "Hôm nay thi. Bình tĩnh, đọc kỹ đề!" : "Chúc mừng đã thi xong!"),
          h("p", {}, "Thứ Năm 29/10/2026 · " + `${LABS.length} bài lab · ${allQ.length} câu trắc nghiệm · ${TOPICS.length} chủ đề lý thuyết`)),
        d >= 0 ? h("div", { class: "count" }, h("b", {}, String(d)), h("span", {}, d ? "ngày nữa" : "hôm nay")) : null),
      todayRow),
    h("div", { class: "kpis" },
      kpi(ring(pctLab, "#22c55e", `${pctLab}%`), `${c.done}/${c.total}`, "bài lab đã tự chấm",
        nextLab ? h("button", { class: "small", onclick: () => go(nextLab.id) }, "Bài tiếp theo →") : null),
      kpi(ring(pctQ, "var(--accent)", `${pctQ}%`), `${qs.done}/${allQ.length}`, qs.done ? `câu đã làm · đúng ${acc}%` : "câu trắc nghiệm đã làm",
        h("button", { class: "small primary", disabled: !TOPICS.length, onclick: () => startQuiz({ topics: TOPICS.map(t => t.id), count: "20", filter: "all", exam: true }) }, "Thi thử 20 câu")),
      kpi(ring(qs.done ? 100 * qs.wrong / qs.done : 0, "var(--bad)", String(qs.wrong)), `${qs.wrong} câu`, "đang sai — ôn lại trước ngày thi",
        h("button", { class: "small", disabled: !qs.wrong, onclick: () => startQuiz({ topics: TOPICS.map(t => t.id), count: "all", filter: "wrong", exam: false }) }, "Ôn câu sai"))),
    TOPICS.length ? [h("div", { class: "section-title" }, h("h3", {}, "Lý thuyết & trắc nghiệm"), h("span", { class: "muted" }, "đọc tóm tắt rồi làm câu hỏi của chủ đề")),
      h("div", { class: "tiles" }, TOPICS.map(topicTile))] : null,
    h("div", { class: "section-title" }, h("h3", {}, "Bài lab"), h("span", { class: "muted" }, "đoán output → chạy → so sánh → tự chấm")),
    h("div", { class: "tiles" }, [...new Set(LABS.map(l => l.dir))].map(dirTile))));
  $("#main").scrollTop = 0;
}
