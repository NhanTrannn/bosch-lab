// Khởi động: tải dữ liệu từ backend rồi định tuyến theo #hash.
"use strict";

// ---------- khởi động
(async () => {
  const [d, th] = await Promise.all([api("/api/labs"), api("/api/theory")]);
  ROOT = d.root; LABS = d.labs; TOPICS = th.topics || [];
  route();
})().catch(e => { $("#main").innerHTML = `<div class="empty">Không kết nối được server: ${esc(e.message)}</div>`; });
function route() {
  const hsh = decodeURIComponent(location.hash.slice(1));
  if (hsh.startsWith("theory/") && TOPICS.length) return showTopic(hsh.slice(7));
  if (hsh === "quiz" && TOPICS.length) return showQuiz();
  if (!hsh || hsh === "home" || hsh === "theory" || hsh === "quiz") return showHome();
  const id = hsh.replace(/^lab\//, "");
  go(LABS.some(l => l.id === id) ? id : (current?.id || LABS[0].id));
}
window.addEventListener("hashchange", route);
