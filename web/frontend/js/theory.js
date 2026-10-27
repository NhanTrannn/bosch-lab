// Trang lý thuyết của một chủ đề, kèm mục lục.
"use strict";

// ---------- lý thuyết
const qStore = () => (store.__quiz ||= {});
function qStats(qs) {
  let done = 0, wrong = 0;
  for (const q of qs) { const s = qStore()[q.id]; if (s) { done++; if (!s.last) wrong++; } }
  return { done, wrong, total: qs.length };
}

function showTopic(id) {
  const t = TOPICS.find(x => x.id === id) || TOPICS[0];
  mode = "theory";
  if (!t) { setHash("theory"); renderNav(); $("#main").replaceChildren(h("div", { class: "narrow" }, theoryBanner())); return; }
  currentTopic = t;
  setHash("theory/" + t.id);
  renderNav();
  const st = qStats(t.questions), idx = TOPICS.indexOf(t);
  const body = h("div", { class: "bd md", html: md(t.theory) });
  const heads = [...body.querySelectorAll("h4")];
  heads.forEach((el, i) => { el.id = `sec-${i}`; });
  const toc = h("nav", { class: "toc" }, h("b", {}, "Trong trang này"),
    heads.map(el => h("a", { href: "#", onclick: e => { e.preventDefault(); el.scrollIntoView({ behavior: "smooth", block: "start" }); } }, el.textContent)));
  $("#main").replaceChildren(h("div", { class: "page theory", style: hueStyle(topicHue(t)) },
    h("div", { class: "topic-hero" },
      h("div", { class: "eyebrow" }, `Lý thuyết · chủ đề ${idx + 1}/${TOPICS.length}`),
      h("h2", {}, t.title),
      h("div", { class: "row", style: "margin-top:12px" },
        h("button", { class: "primary", onclick: () => startQuiz({ topics: [t.id], count: "all", filter: "all" }) }, `▶ Làm ${t.questions.length} câu trắc nghiệm`),
        st.wrong ? h("button", { onclick: () => startQuiz({ topics: [t.id], count: "all", filter: "wrong" }) }, `Làm lại ${st.wrong} câu đang sai`) : null,
        h("span", { class: "muted" }, `${st.done}/${st.total} câu đã làm · `, h("a", { href: `vscode://file${ROOT}/09-theory/${t.id}.md` }, "sửa trong VS Code ↗")))),
    h("div", { class: "theory-layout" },
      h("div", {},
        h("div", { class: "card" }, body),
        h("div", { class: "row", style: "margin-top:16px" },
          h("button", { disabled: idx <= 0, onclick: () => showTopic(TOPICS[idx - 1].id) }, "← Chủ đề trước"),
          h("button", { disabled: idx >= TOPICS.length - 1, onclick: () => showTopic(TOPICS[idx + 1].id) }, "Chủ đề sau →"),
          h("button", { class: "primary", style: "margin-left:auto", onclick: () => startQuiz({ topics: [t.id], count: "all", filter: "all" }) }, "Làm trắc nghiệm chủ đề này"))),
      toc)));
  $("#main").scrollTop = 0;
}
