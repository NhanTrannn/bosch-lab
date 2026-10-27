// Trang một bài lab: đoán output → chạy → so sánh (diff) → đáp án.
"use strict";

// ---------- so sánh dự đoán với output thật
function diffLines(a, b) {
  const n = a.length, m = b.length, dp = Array.from({ length: n + 1 }, () => new Int32Array(m + 1));
  for (let i = n - 1; i >= 0; i--) for (let j = m - 1; j >= 0; j--)
    dp[i][j] = a[i].key === b[j].key ? dp[i + 1][j + 1] + 1 : Math.max(dp[i + 1][j], dp[i][j + 1]);
  const out = [];
  let i = 0, j = 0;
  while (i < n || j < m) {
    if (i < n && j < m && a[i].key === b[j].key) { out.push(["same", b[j].text]); i++; j++; }
    else if (j < m && (i === n || dp[i][j + 1] >= dp[i + 1][j])) out.push(["add", b[j++].text]);
    else out.push(["del", a[i++].text]);
  }
  return out;
}
function toLines(s) {
  const lines = s.replace(/\r/g, "").split("\n").map(l => l.replace(/\s+$/, ""));
  while (lines.length && !lines[lines.length - 1]) lines.pop();
  return lines.slice(0, 400).map(text => ({ text, key: (ignoreNums ? text.replace(/\d+/g, "#") : text).replace(/\s+/g, " ") }));
}
function diffView(want, got, labels) {
  const rows = diffLines(toLines(want), toLines(got));
  const same = rows.every(r => r[0] === "same");
  return h("div", {},
    h("div", { class: "row", style: "margin:0 0 6px" },
      h("span", { class: "verdict " + (same ? "ok" : "bad") }, same ? "✓ Khớp hoàn toàn" : `✗ Khác ${rows.filter(r => r[0] !== "same").length} dòng`),
      h("span", { class: "muted" }, h("span", { style: "color:var(--bad)" }, "- " + labels[0]), " · ", h("span", { style: "color:var(--ok)" }, "+ " + labels[1]))),
    h("div", { class: "diff" }, rows.map(([k, t]) => h("div", { class: k }, t || " "))));
}

// ---------- trang một bài
function go(id) {
  if (current?.id !== id) fileTab = 0;
  current = LABS.find(l => l.id === id) || LABS[0];
  mode = "lab";
  setHash("lab/" + current.id);
  renderNav();
  renderLab();
  $("#main").scrollTop = 0;
}

function renderLab() {
  const lab = current, st = labState(lab.id), m = lab.meta;
  const main = $("#main");
  Object.keys(VIEWS).forEach(k => k !== "terminal" && delete VIEWS[k]);

  const chips = h("div", { class: "chips" },
    m.topic ? h("span", { class: "chip tone" }, m.topic) : null,
    lab.unstable ? h("span", { class: "chip warn", title: "Thứ tự/giá trị có thể khác mỗi lần chạy" }, "⚠️ output không cố định") : null,
    m.warning ? h("span", { class: "chip bad" }, "!!! " + m.warning) : null,
    m.warn_build ? h("span", { class: "chip warn", title: m.warn_build }, "cảnh báo biên dịch cố ý") : null);

  // --- code
  const codeBody = h("div");
  const tabs = h("div", { class: "tabs" });
  const showFile = k => {
    fileTab = k;
    const f = lab.files[k], src = lab.sources[f], lang = langOf(f);
    tabs.replaceChildren(...lab.files.map((name, j) => h("button", { class: j === k ? "on" : "", onclick: () => showFile(j) }, name.split("/").pop())));
    if (lang === "md") { codeBody.className = "bd md"; codeBody.innerHTML = md(src); return; }
    const n = src.replace(/\n$/, "").split("\n").length;
    codeBody.className = "code";
    codeBody.replaceChildren(h("pre", { class: "gut" }, Array.from({ length: n }, (_, j) => j + 1).join("\n")),
      h("pre", { html: highlight(src.replace(/\n$/, ""), lang) }));
  };
  const codeCard = h("div", { class: "card" },
    h("div", { class: "hd" }, tabs, h("span", { class: "sp" }),
      h("a", { href: `vscode://file${ROOT}/${lab.files[0]}`, class: "muted", title: "Mở file trong VS Code để sửa (phần Thử thêm)" }, "mở trong VS Code ↗")),
    codeBody);
  showFile(Math.min(fileTab, lab.files.length - 1));

  // --- dự đoán
  const ta = h("textarea", { placeholder: "Ghi output bạn đoán vào đây, rồi bấm Chạy (Ctrl+Enter)…", spellcheck: "false" });
  ta.value = st.pred;
  ta.addEventListener("input", () => { st.pred = ta.value; saveStore(); renderNav(); refreshCompare(); });
  const runnable = lab.kind !== "readonly";
  const runBtn = h("button", { class: "primary", disabled: !runnable, onclick: () => doRun() },
    lab.kind === "shell" ? "▶ Mở bash" : "▶ Build & chạy");
  const doRun = async () => {
    if (!runnable) return;
    await startRun(lab.id, { id: lab.id });
    term.focus();
  };
  ta.addEventListener("keydown", e => { if (e.key === "Enter" && (e.ctrlKey || e.metaKey)) { e.preventDefault(); doRun(); } });

  const predictCard = lab.kind === "program" ? h("div", { class: "card" },
    h("div", { class: "hd" }, h("span", { class: "step" }, "1"), "Đoán output trước"),
    h("div", { class: "bd" },
      lab.question ? h("div", { class: "q", html: `<b>Câu hỏi:</b> ${inline(lab.question)}` }) : null,
      m.predict && m.predict !== lab.question ? h("div", { class: "muted", style: "margin-bottom:8px" }, m.predict) : null,
      ta,
      h("div", { class: "row" }, runBtn, h("span", { class: "muted" }, h("kbd", {}, "Ctrl"), "+", h("kbd", {}, "Enter"), " để chạy · ghi chú tự lưu trong trình duyệt"))))
    : h("div", { class: "card" }, h("div", { class: "hd" }, lab.kind === "shell" ? "Thực hành lệnh" : "Bài đọc"),
      h("div", { class: "bd" },
        lab.kind === "shell"
          ? h("div", {}, h("p", { style: "margin-top:0" }, "Mở bash thật (thư mục nháp ", h("code", {}, "build/shell"), ") rồi làm lần lượt 20 drill bên trái. Gõ lệnh ở ô stdin."), runBtn)
          : h("div", {}, h("p", { style: "margin-top:0" }, "Bài này cần ", h("code", {}, "sudo"), " và linux-headers nên không chạy trong web. Làm theo README trong ", h("b", {}, "Terminal 2"), " (góc dưới phải) hoặc terminal thường."),
              h("button", { onclick: () => openDock(`cd 07-kernel-module && make\n`) }, "Mở Terminal 2 và build module"))));

  // --- terminal + so sánh
  const compareBody = h("div", { class: "bd" });
  const compareCard = h("div", { class: "card", hidden: lab.kind !== "program" },
    h("div", { class: "hd" }, h("span", { class: "step" }, "3"), "So sánh", h("span", { class: "sp" }),
      h("label", { class: "muted", title: "Thay mọi số bằng # khi so (PID, địa chỉ…)" },
        h("input", { type: "checkbox", checked: ignoreNums, onchange: e => { ignoreNums = e.target.checked; refreshCompare(); } }), " bỏ qua số (PID…)")),
    compareBody);
  function refreshCompare() {
    if (lab.kind !== "program") return;
    const r = RUNS[lab.id], got = runText(r);
    const parts = [];
    if (!r || !r.rid) parts.push(h("div", { class: "muted" }, "Chạy bài để so dự đoán với output thật."));
    else if (!st.pred.trim()) parts.push(h("div", { class: "muted" }, "Bạn chưa ghi dự đoán — ghi vào ô ở bước 1 để so."));
    else parts.push(h("div", { class: "muted", style: "margin-bottom:4px" }, "Dự đoán của bạn ↔ output thật" + (r.done ? "" : " (đang chạy…)")), diffView(st.pred, got, ["dự đoán", "output thật"]));
    if (st.revealed && lab.expected && r?.rid)
      parts.push(h("div", { class: "muted", style: "margin:14px 0 4px" }, "Output mong đợi (ANSWERS.md) ↔ output thật"), diffView(lab.expected, got, ["mong đợi", "output thật"]));
    if (lab.unstable) parts.push(h("p", { class: "muted" }, "⚠️ Bài này output không cố định: khác thứ tự/PID chưa chắc là sai — đọc giải thích ở đáp án."));
    compareBody.replaceChildren(...parts);
  }

  const term = termView(lab.id, { signals: lab.kind === "program", empty: lab.kind === "shell" ? "Bấm ▶ Mở bash để bắt đầu." : runnable ? "Chưa chạy. Bấm ▶ để build & chạy." : "Bài đọc — không chạy trong web.", onUpdate: refreshCompare });
  const termCard = h("div", { class: "card", hidden: !runnable },
    h("div", { class: "hd" }, lab.kind === "program" ? [h("span", { class: "step" }, "2"), "Chạy"] : "bash", term.stateEl, h("span", { class: "sp" }),
      h("button", { class: "small", disabled: !runnable, onclick: doRun }, "Chạy lại")),
    term.box);

  // --- đáp án
  const ansBody = h("div", { class: "bd" });
  const markBtn = (k, label) => h("button", { class: "small" + (st.mark === k ? " on" : ""), onclick: () => { st.mark = st.mark === k ? null : k; saveStore(); renderNav(); renderAnswer(); } }, label);
  function renderAnswer() {
    if (!st.revealed) {
      ansBody.replaceChildren(h("div", { class: "spoiler" },
        h("p", { class: "muted", style: "margin-top:0" }, "Chỉ mở sau khi đã tự đoán và chạy thử."),
        h("button", { onclick: () => { st.revealed = true; saveStore(); renderAnswer(); refreshCompare(); } }, "Xem đáp án & giải thích")));
      return;
    }
    const notes = h("dl", { class: "notes" },
      m.idea ? [h("dt", {}, "Ý chính"), h("dd", {}, m.idea)] : null,
      m.try ? [h("dt", {}, "Thử thêm"), h("dd", {}, m.try)] : null,
      m.warn_build ? [h("dt", {}, "Cảnh báo biên dịch cố ý"), h("dd", {}, m.warn_build)] : null);
    ansBody.replaceChildren(
      lab.kind === "program" ? h("div", { class: "row", style: "margin:0 0 10px" }, h("b", {}, "Tự chấm:"), markBtn("right", "✓ Đoán đúng"), markBtn("wrong", "✗ Đoán sai")) : null,
      h("div", { class: "md", html: md(lab.answer || "_Không có đáp án trong ANSWERS.md._") }),
      notes.children.length ? notes : null,
      h("div", { class: "row" }, h("button", { class: "small", onclick: () => { st.revealed = false; saveStore(); renderAnswer(); refreshCompare(); } }, "Ẩn đáp án")));
  }
  const ansCard = h("div", { class: "card" }, h("div", { class: "hd" }, lab.kind === "program" ? [h("span", { class: "step" }, "4"), "Đáp án"] : "Đáp án"), ansBody);
  renderAnswer();
  refreshCompare();

  const idx = LABS.indexOf(lab);
  const navBtns = h("div", { class: "row", style: "margin-top:16px" },
    h("button", { disabled: idx <= 0, onclick: () => go(LABS[idx - 1].id) }, "← Bài trước"),
    h("button", { disabled: idx >= LABS.length - 1, onclick: () => go(LABS[idx + 1].id) }, "Bài sau →"));

  main.replaceChildren(h("div", { class: "page", style: hueStyle(HUE[lab.dir] ?? 245) },
    h("div", { class: "eyebrow" }, `${lab.dir} · ${DIR_NAMES[lab.dir] || ""}`),
    h("div", { class: "title-row" }, h("h2", {}, lab.title), h("span", { class: "muted" }, lab.id)),
    chips,
    h("div", { class: "grid" },
      h("div", {}, predictCard, termCard, compareCard),
      h("div", {}, codeCard, ansCard)),
    navBtns));
}
