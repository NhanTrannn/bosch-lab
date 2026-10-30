// Lab index: lab topics (one per directory) grouped like the theory/quiz pages, and one page per topic.
import { $, DIR_NAMES, h, LABS, set, setHash, store } from "./core.js";
import { HUE, hueStyle, renderNav } from "./nav.js";
import { go } from "./lab.js";

// Lab topics: each topic picks labs by directory and number (e.g. ["cpp", [17, 18]]).
// Labs not listed in any topic fall into one "Other <dir>" topic per directory.
const T = (id, group, title, dir, nums) => ({ id, group, title, dir, nums });
const LAB_TOPIC_DEFS = [
  T("c-memory", "C", "Pointers, memory & storage", "c", [1, 2, 3, 4, 6, 9, 11, 13, 16]),
  T("c-operators", "C", "Operators, types, conversions & macros", "c", [5, 7, 8, 10, 12, 14, 15]),
  T("cpp-basics", "C++", "Basics: types, operators, control flow, strings", "cpp", [17, 18, 19, 20]),
  T("cpp-functions", "C++", "Functions, pointers & references", "cpp", [21, 22]),
  T("cpp-classes", "C++", "Classes, special members & operator overloading", "cpp", [23, 24, 5, 13, 15]),
  T("cpp-inheritance", "C++", "Inheritance & polymorphism", "cpp", [1, 2, 3, 4, 9, 10, 12, 25]),
  T("cpp-exceptions", "C++", "Exceptions, streams & templates", "cpp", [11, 26, 27, 16]),
  T("cpp-stl", "C++", "STL, smart pointers, move & lambdas", "cpp", [6, 7, 8, 14, 28, 29]),
  T("dp-creational", "Design patterns", "Creational patterns", "design-patterns", [1, 2, 3]),
  T("dp-structural", "Design patterns", "Structural patterns", "design-patterns", [4, 5, 6]),
  T("dp-behavioral", "Design patterns", "Behavioral patterns", "design-patterns", [7, 8, 9, 10, 11]),
  T("dp-j2ee", "Design patterns", "J2EE patterns", "design-patterns", [12]),
  T("processes", "Linux system programming", "Processes: fork, exec, wait", "linux-process", null),
  T("signals", "Linux system programming", "Signals", "linux-signal", null),
  T("threads", "Linux system programming", "Threads & synchronization", "linux-thread", null),
  T("ipc", "Linux system programming", "IPC: pipes, FIFOs, shm, sockets, queues", "linux-ipc", null),
  T("alp-ch2", "Advanced Linux (ALP)", "Ch.1–2: writing good Linux software", "alp", [1, 2, 3, 4, 5, 6]),
  T("alp-ch3", "Advanced Linux (ALP)", "Ch.3: processes", "alp", [7, 8]),
  T("alp-ch4", "Advanced Linux (ALP)", "Ch.4: threads", "alp", [9, 10, 11]),
  T("alp-ch5", "Advanced Linux (ALP)", "Ch.5: IPC", "alp", [12, 13, 14, 15]),
  T("kernel", "Kernel & shell", "Kernel module", "kernel-module", null),
  T("shell", "Kernel & shell", "Shell command drills", "shell", null),
];
const labNum = lab => parseInt(lab.name, 10);

// Built once from LABS: [{id, group, title, dir, labs: [lab...]}]
let LAB_TOPICS_CACHE = null;
export function labTopics() {
  if (LAB_TOPICS_CACHE) return LAB_TOPICS_CACHE;
  const used = new Set();
  const topics = LAB_TOPIC_DEFS.map(t => {
    const labs = LABS.filter(l => l.dir === t.dir && (!t.nums || t.nums.includes(labNum(l))))
      .sort((x, y) => labNum(x) - labNum(y) || x.name.localeCompare(y.name));
    labs.forEach(l => used.add(l.id));
    return { ...t, labs };
  }).filter(t => t.labs.length);
  for (const dir of new Set(LABS.map(l => l.dir))) {
    const rest = LABS.filter(l => l.dir === dir && !used.has(l.id));
    if (rest.length) topics.push({ id: "other-" + dir, group: "Other", title: DIR_NAMES[dir] || dir, dir, labs: rest });
  }
  return (LAB_TOPICS_CACHE = topics);
}
export const labTopicById = id => labTopics().find(t => t.id === id);
export const labTopicOf = lab => labTopics().find(t => t.labs.includes(lab));
export function labGroups() {
  const groups = new Map();
  for (const t of labTopics()) { if (!groups.has(t.group)) groups.set(t.group, []); groups.get(t.group).push(t); }
  return [...groups].map(([name, topics]) => ({ name, topics }));
}

export function labStats(labs) {
  const prog = labs.filter(l => l.kind === "program");
  const mark = l => (store[l.id] || {}).mark;
  return { total: labs.length, prog: prog.length, done: prog.filter(mark).length,
    right: prog.filter(l => mark(l) === "right").length, wrong: prog.filter(l => mark(l) === "wrong").length,
    next: prog.find(l => !mark(l)) || labs[0] };
}

const onKey = fn => e => { if (e.key === "Enter" || e.key === " ") { e.preventDefault(); fn(); } };

export function showLabIndex() {
  set({ mode: "lab", labTopic: null });
  setHash("labs");
  renderNav();
  const card = t => {
    const st = labStats(t.labs), hue = HUE[t.dir] ?? 245;
    return h("div", { class: "card topic-card", style: hueStyle(hue), role: "button", tabindex: "0",
      onclick: () => showLabTopic(t.id), onkeydown: onKey(() => showLabTopic(t.id)) },
      h("div", { class: "eyebrow" }, `${t.dir} · ${t.labs.length} lab${t.labs.length > 1 ? "s" : ""}`),
      h("div", { class: "t" }, t.title),
      h("div", { class: "bar" }, h("div", { style: `width:${st.prog ? 100 * st.done / st.prog : 0}%` })),
      h("div", { class: "foot" },
        h("span", { class: "muted" }, st.prog ? `${st.done}/${st.prog} graded` + (st.wrong ? ` · ${st.wrong} wrong` : "") : "reading / practice"),
        h("button", { class: "small", style: "margin-left:auto", title: "Open the next lab you have not graded",
          onclick: e => { e.stopPropagation(); go(st.next.id); } }, st.prog && st.done < st.prog ? "Continue →" : "Open →")));
  };
  const c = labStats(LABS);
  $("#main").replaceChildren(h("div", { class: "setup" },
    h("div", { class: "eyebrow" }, `${labTopics().length} topics · ${LABS.length} labs · ${c.done}/${c.prog} graded`),
    h("div", { class: "title-row" }, h("h2", {}, "Labs")),
    h("p", { class: "muted", style: "margin:4px 0 14px" }, "Pick a topic to see its labs. In each lab: predict the output, run it, compare, then read the answer and self-grade."),
    labGroups().map(g => h("section", { class: "quiz-group", style: hueStyle(HUE[g.topics[0].dir] ?? 245) },
      h("div", { class: "group-head" }, h("span", { class: "sw" }), h("h3", {}, g.name),
        h("span", { class: "muted" }, `${g.topics.length} topics · ${g.topics.reduce((n, t) => n + t.labs.length, 0)} labs`)),
      h("div", { class: "topic-cards" }, g.topics.map(card))))));
  $("#main").scrollTop = 0;
}

export function showLabTopic(id) {
  const t = labTopicById(id) || labTopics().find(x => x.dir === id);   // a directory name also works (old links)
  if (!t) return showLabIndex();
  const labs = t.labs, hue = HUE[t.dir] ?? 245;
  set({ mode: "lab", labTopic: t.id });
  setHash("labs/" + t.id);
  renderNav();
  const st = labStats(labs);
  const row = (lab, i) => {
    const s = store[lab.id] || {};
    const status = s.mark === "right" ? ["right", "✓ right"] : s.mark === "wrong" ? ["wrong", "✗ wrong"] : s.pred?.trim() ? ["pred", "predicted"] : ["", lab.kind === "program" ? "to do" : lab.kind];
    return h("div", { class: "card lab-row", role: "button", tabindex: "0", onclick: () => go(lab.id), onkeydown: onKey(() => go(lab.id)) },
      h("span", { class: "num" }, String(i + 1).padStart(2, "0")),
      h("span", { class: "dot " + status[0] }),
      h("span", { class: "grow" }, h("b", {}, lab.title), h("small", {}, lab.id + (lab.unstable ? " · ⚠️ nondeterministic" : "") + (lab.meta?.warning ? " · " + lab.meta.warning : ""))),
      h("span", { class: "status " + status[0] }, status[1]));
  };
  $("#main").replaceChildren(h("div", { class: "setup", style: hueStyle(hue) },
    h("div", { class: "row", style: "margin-bottom:8px" }, h("button", { class: "small", onclick: showLabIndex }, "← All lab topics")),
    h("div", { class: "eyebrow" }, `${t.group} · ${labs.length} labs` + (st.prog ? ` · ${st.done}/${st.prog} graded` : "")),
    h("div", { class: "title-row" }, h("h2", {}, t.title),
      h("span", { class: "sp", style: "flex:1" }),
      h("button", { class: "primary small", onclick: () => go(st.next.id) }, st.done && st.done < st.prog ? "Continue →" : "Start →")),
    h("div", { class: "lab-rows" }, labs.map(row))));
  $("#main").scrollTop = 0;
}
