// Startup: load data from the backend, then route by #hash.
import { $, api, applyMigrations, current, esc, LABS, set, TOPICS } from "./core.js";
import { showHome } from "./home.js";
import { go } from "./lab.js";
import { showLabIndex, showLabTopic } from "./labindex.js";
import { showTheoryIndex, showTopic } from "./theory.js";
import { showQuiz } from "./quiz.js";
import { initSearchBox, showSearch } from "./search.js";

// ---------- startup
(async () => {
  const [d, th, migs] = await Promise.all([api("/api/labs"), api("/api/theory"), api("/api/migrations").catch(() => ({}))]);
  applyMigrations(migs);
  initSearchBox();
  set({ ROOT: d.root, LABS: d.labs, TOPICS: th.topics || [] });
  route();
})().catch(e => { $("#main").innerHTML = `<div class="empty">Cannot reach the server: ${esc(e.message)}</div>`; });
function route() {
  const hsh = decodeURIComponent(location.hash.slice(1));
  if (hsh === "search" || hsh.startsWith("search/")) return showSearch(hsh.slice(7));
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
