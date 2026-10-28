// Thanh bên: tab chế độ, danh sách bài lab / chủ đề; màu chủ đề, icon, vòng tiến độ.
"use strict";

// ---------- sidebar
function counts() {
  const ids = LABS.filter(l => l.kind === "program").map(l => l.id);
  const st = id => store[id] || {};
  return { total: ids.length, done: ids.filter(id => st(id).mark).length, right: ids.filter(id => st(id).mark === "right").length, wrong: ids.filter(id => st(id).mark === "wrong").length };
}
const HUE = { "01-c": 215, "02-cpp": 265, "03-process": 170, "04-signal": 35, "05-thread": 345, "06-ipc": 195, "07-kernel-module": 20, "08-shell": 140, "10-alp": 95 };
const TOPIC_HUES = [215, 265, 300, 170, 35, 345, 140, 195, 20];
const topicHue = t => TOPIC_HUES[Math.max(0, TOPICS.indexOf(t)) % TOPIC_HUES.length];
const hueStyle = hue => `--h:${hue}`;
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
function ring(pct, color, label) {
  const r = 42, c = 2 * Math.PI * r;
  return h("div", { class: "ring", html: `<svg viewBox="0 0 100 100" width="100%" height="100%"><circle cx="50" cy="50" r="${r}" fill="none" stroke="var(--line)" stroke-width="10"/>` +
    `<circle cx="50" cy="50" r="${r}" fill="none" stroke="${color}" stroke-width="10" stroke-linecap="round" stroke-dasharray="${c * Math.min(pct, 100) / 100} ${c}" transform="rotate(-90 50 50)"/>` +
    `<text x="50" y="51" text-anchor="middle" dominant-baseline="middle" font-size="${label.length > 4 ? 18 : 22}" font-weight="700" fill="currentColor">${esc(label)}</text></svg>` });
}
function theoryBanner() {
  return TOPICS.length ? null : h("div", { class: "banner", html:
    "<b>Không tải được lý thuyết/trắc nghiệm từ server.</b><br>Server đang chạy là bản cũ: tắt nó (Ctrl+C trong terminal đang chạy <code>make web</code>) rồi chạy lại <code>make web</code> và tải lại trang." });
}

function renderNav() {
  const wrong = qStats(TOPICS.flatMap(t => t.questions)).wrong;
  const M = [["home", "Tổng quan"], ["lab", "Bài lab"], ["theory", "Lý thuyết"], ["quiz", "Trắc nghiệm"]];
  $("#modes").replaceChildren(...M.map(([k, v]) =>
    h("button", { class: mode === k ? "on" : "", onclick: () => switchMode(k) }, svgIcon(k), v,
      k === "quiz" && wrong ? h("span", { class: "badge", title: "số câu đang sai" }, String(wrong)) : null)));
  const d = daysLeft();
  $("#brandSub").textContent = d > 0 ? `Còn ${d} ngày tới assessment` : d === 0 ? "Hôm nay thi — cố lên!" : "Ôn C++ & Linux System";
  if (mode === "lab") renderLabNav(); else renderTopicNav();
}
function switchMode(k) {
  if (k === "home") showHome();
  else if (k === "lab") go(current?.id || LABS[0].id);
  else if (k === "theory") showTopic(currentTopic?.id || TOPICS[0]?.id);
  else showQuiz();
}
function renderTopicNav() {
  const all = TOPICS.flatMap(t => t.questions), stats = qStats(all);
  $("#bar").style.width = (all.length ? 100 * stats.done / all.length : 0) + "%";
  $("#stats").textContent = TOPICS.length ? `${stats.done}/${all.length} câu đã làm · ${stats.wrong} đang sai` : "Chưa tải được trắc nghiệm";
  $("#filters").replaceChildren();
  $("#nav").replaceChildren(h("div", { class: "group" }, h("h2", {}, mode === "quiz" ? "Làm hết câu của một chủ đề" : "Chủ đề lý thuyết"),
    TOPICS.map(t => {
      const st = qStats(t.questions);
      const dot = !st.done ? "" : st.wrong ? "wrong" : st.done === st.total ? "right" : "pred";
      const active = mode === "theory" && currentTopic?.id === t.id;
      return h("div", { class: "item" + (active ? " active" : ""), style: hueStyle(topicHue(t)),
        onclick: () => mode === "quiz" ? startQuiz({ topics: [t.id], count: "all", filter: "all" }) : showTopic(t.id) },
        h("span", { class: "dot " + dot }),
        h("span", { class: "nm" }, t.title, h("small", {}, `${st.done}/${st.total} câu` + (st.wrong ? ` · ${st.wrong} sai` : "")),
          h("div", { class: "minibar" }, h("div", { style: `width:${st.total ? 100 * st.done / st.total : 0}%` }))));
    })));
}
function renderLabNav() {
  const c = counts();
  $("#bar").style.width = (c.total ? 100 * c.done / c.total : 0) + "%";
  $("#stats").textContent = `${c.done}/${c.total} bài đã chấm · ${c.right} đúng · ${c.wrong} sai`;
  const F = { all: "Tất cả", todo: "Chưa làm", wrong: "Đoán sai" };
  $("#filters").replaceChildren(...Object.entries(F).map(([k, v]) =>
    h("button", { class: "small" + (filter === k ? " on" : ""), onclick: () => { filter = k; renderNav(); } }, v)));
  const nav = $("#nav");
  nav.replaceChildren();
  const groups = {};
  for (const lab of LABS) {
    const s = store[lab.id] || {};
    if (filter === "todo" && (s.mark || lab.kind !== "program")) continue;
    if (filter === "wrong" && s.mark !== "wrong") continue;
    (groups[lab.dir] ||= []).push(lab);
  }
  for (const [dir, labs] of Object.entries(groups)) {
    nav.append(h("div", { class: "group", style: hueStyle(HUE[dir] ?? 245) }, h("h2", {}, h("span", { class: "sw" }), `${dir} · ${DIR_NAMES[dir] || ""}`),
      labs.map(lab => {
        const s = store[lab.id] || {};
        const dot = s.mark || (s.pred?.trim() ? "pred" : "");
        const running = RUNS[lab.id] && !RUNS[lab.id].done;
        return h("div", { class: "item" + (current?.id === lab.id ? " active" : ""), onclick: () => go(lab.id) },
          h("span", { class: "dot " + dot }),
          h("span", { class: "nm" }, lab.name, h("small", {}, lab.title)),
          running ? h("span", { class: "run-badge", title: "đang chạy" }, "●") : null);
      })));
  }
  if (!nav.children.length) nav.append(h("div", { class: "empty" }, "Không có bài nào."));
}
