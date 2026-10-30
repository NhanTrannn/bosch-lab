// Search page: one box for labs, theory sections and quiz questions (GET /api/search, SQLite FTS5 on the backend).
import { $, api, esc, h, set, setHash, TOPICS } from "./core.js";
import { HUE, hueStyle, renderNav, topicHue } from "./nav.js";
import { go } from "./lab.js";
import { showTopic } from "./theory.js";
import { startQuiz } from "./quiz.js";

const KINDS = { "": "All", lab: "Labs", theory: "Theory", quiz: "Quiz" };
const ICON = { lab: "🧪", theory: "📘", quiz: "✍️" };
let kind = "", tag = "", tags = null, seq = 0;

// Server text marks matches with \x02 … \x03: escape everything else, then turn the marks into <mark>
const marked = s => esc(s).replace(/\x02/g, "<mark>").replace(/\x03/g, "</mark>");
const plain = s => s.replace(/```\w*|`|\*\*|^#+ |^> |\|/gm, " ").replace(/[ \t]+/g, " ").trim();

const topicOfQuestion = id => TOPICS.find(t => t.questions.some(q => q.id === id));

function open(r) {
  if (r.kind === "lab") return go(r.item_id);
  if (r.kind === "theory") { const [id, sec] = r.item_id.split("#"); return showTopic(id, +sec); }
  const t = topicOfQuestion(r.item_id), q = t?.questions.find(x => x.id === r.item_id);
  if (q) startQuiz({ list: [{ ...q, topic: t }] });
}

function where(r) {
  if (r.kind === "lab") return [r.item_id, HUE[r.grp] ?? 245];
  const t = TOPICS.find(x => x.id === r.item_id.split("#")[0]) || topicOfQuestion(r.item_id);
  return [t ? `${t.group} · ${t.title}` : r.grp, t ? topicHue(t) : 245];
}

// The box in the sidebar header (index.html): search while typing
export function initSearchBox() {
  const input = $("#searchBox");
  let timer = 0;
  input.addEventListener("input", () => { clearTimeout(timer); timer = setTimeout(() => showSearch(input.value), 150); });
  input.addEventListener("keydown", e => {
    if (e.key === "Enter") { clearTimeout(timer); showSearch(input.value); }
    else if (e.key === "Escape") { input.value = ""; input.blur(); }
  });
}

export async function showSearch(q = "") {
  set({ mode: "search" });
  setHash("search/" + encodeURIComponent(q));
  renderNav();
  const box = $("#searchBox");
  if (box && box !== document.activeElement && box.value !== q) box.value = q;
  if (!tags) tags = (await api("/api/tags").catch(() => ({}))).tags || [];
  const mine = ++seq;
  const qs = new URLSearchParams({ q, ...(kind && { kind }), ...(tag && { tag }) });
  const results = q.trim() ? (await api("/api/search?" + qs).catch(() => ({}))).results || [] : [];
  if (mine !== seq) return;   // a newer search already started

  const chip = (label, on, onclick) => h("button", { class: "small" + (on ? " on" : ""), onclick }, label);
  const row = r => {
    const [sub, hue] = where(r);
    return h("div", { class: "card lab-row search-row", style: hueStyle(hue), role: "button", tabindex: "0",
      onclick: () => open(r), onkeydown: e => { if (e.key === "Enter") open(r); } },
      h("span", { class: "kind" }, ICON[r.kind]),
      h("span", { class: "grow" },
        h("b", { html: marked(plain(r.title)) }),
        h("small", {}, `${KINDS[r.kind].replace(/s$/, "")} · ${sub}`),
        r.snippet ? h("span", { class: "snip", html: marked(plain(r.snippet)) }) : null));
  };
  const counts = Object.fromEntries(Object.keys(KINDS).map(k => [k, results.filter(r => r.kind === k).length]));
  $("#main").replaceChildren(h("div", { class: "setup" },
    h("div", { class: "eyebrow" }, q.trim() ? `${results.length}${results.length === 60 ? "+" : ""} results` : "Search"),
    h("div", { class: "title-row" }, h("h2", {}, q.trim() ? `“${q.trim()}”` : "Search")),
    h("div", { class: "row search-filters" },
      Object.entries(KINDS).map(([k, label]) => chip(k && kind !== k && counts[k] ? `${label} (${counts[k]})` : label, kind === k, () => { kind = k; showSearch(q); })),
      tags.length ? h("span", { class: "sep" }) : null,
      tag ? chip(`✕ #${tag}`, true, () => { tag = ""; showSearch(q); }) : null),
    tags.length ? h("div", { class: "tag-cloud" }, tags.filter(t => t.tag !== tag).slice(0, 30).map(t =>
      h("button", { class: "small tag", onclick: () => { tag = t.tag; showSearch(q); } }, `#${t.tag}`))) : null,
    !q.trim() ? h("p", { class: "muted" }, "Type a few words: every word must match (as a prefix), e.g. ", h("code", {}, "fork buf"), ", ", h("code", {}, "virtual dtor"), ", ", h("code", {}, "mutex cond"), ". Press ", h("kbd", {}, "/"), " anywhere to jump here.")
      : !results.length ? h("p", { class: "muted" }, "No results. Try fewer or shorter words" + (kind || tag ? ", or remove the filters." : "."))
      : h("div", { class: "lab-rows" }, results.map(row))));
  $("#main").scrollTop = 0;
}

// "/" focuses the search box from anywhere (except while typing)
document.addEventListener("keydown", e => {
  if (e.key !== "/" || e.ctrlKey || e.metaKey || e.altKey || /^(INPUT|TEXTAREA)$/.test(document.activeElement?.tagName)) return;
  e.preventDefault();
  $("#searchBox")?.focus();
});
