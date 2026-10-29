// Startup: load data from the backend, then route by #hash.
"use strict";

// ---------- startup
(async () => {
  const [d, th] = await Promise.all([api("/api/labs"), api("/api/theory")]);
  ROOT = d.root; LABS = d.labs; TOPICS = th.topics || [];
  route();
})().catch(e => { $("#main").innerHTML = `<div class="empty">Cannot reach the server: ${esc(e.message)}</div>`; });
function route() {
  const hsh = decodeURIComponent(location.hash.slice(1));
  if (hsh.startsWith("theory/") && TOPICS.length) return showTopic(hsh.slice(7));
  if (hsh === "theory" && TOPICS.length) return showTheoryIndex();
  if (hsh === "quiz" && TOPICS.length) return showQuiz();
  if (!hsh || hsh === "home" || hsh === "theory" || hsh === "quiz") return showHome();
  if (hsh === "labs") return showLabIndex();
  if (hsh.startsWith("labs/")) return showLabTopic(hsh.slice(5));
  const id = hsh.replace(/^lab\//, "");
  go(LABS.some(l => l.id === id) ? id : (current?.id || LABS[0].id));
}
window.addEventListener("hashchange", route);
