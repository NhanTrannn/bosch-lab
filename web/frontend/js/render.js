// Syntax highlighting for C/C++/shell and a minimal markdown renderer (enough for ANSWERS.md, 09-theory).
"use strict";

// ---------- code highlighting
const KW = new Set(("auto break case char const continue default do double else enum extern float for goto if inline int long register restrict return short signed sizeof static struct switch typedef union unsigned void volatile while bool true false nullptr class public private protected virtual override final template typename namespace using new delete this operator friend explicit noexcept constexpr try catch throw std size_t uint8_t uint16_t uint32_t uint64_t int8_t int16_t int32_t int64_t pid_t ssize_t mutable static_cast dynamic_cast reinterpret_cast const_cast decltype NULL").split(" "));
function highlight(code, lang) {
  if (lang === "sh" || lang === "make") {
    return code.split("\n").map(l => {
      const i = l.search(/(^|\s)#/);
      return i < 0 ? esc(l) : esc(l.slice(0, i)) + `<span class="c">${esc(l.slice(i))}</span>`;
    }).join("\n");
  }
  const re = /(\/\*[\s\S]*?\*\/|\/\/[^\n]*)|("(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])*')|(^[ \t]*#[^\n]*)|\b(0x[\da-fA-F]+[uUlL]*|\d[\d.]*[uUlLfF]*)\b|\b([A-Za-z_]\w*)\b/gm;
  let out = "", last = 0, m;
  while ((m = re.exec(code))) {
    out += esc(code.slice(last, m.index));
    const [t] = m;
    if (m[1]) out += `<span class="c">${esc(t)}</span>`;
    else if (m[2]) out += `<span class="s">${esc(t)}</span>`;
    else if (m[3]) out += `<span class="p">${esc(t)}</span>`;
    else if (m[4]) out += `<span class="n">${esc(t)}</span>`;
    else out += KW.has(t) ? `<span class="k">${t}</span>` : esc(t);
    last = re.lastIndex;
  }
  return out + esc(code.slice(last));
}
const langOf = f => f.endsWith(".md") ? "md" : f.endsWith("Makefile") ? "make" : "c";

// ---------- minimal markdown (enough for ANSWERS.md / drills.md)
function inline(s) {
  return s.split(/(`[^`]*`)/).map((part, i) => {
    if (i % 2) return `<code>${esc(part.slice(1, -1))}</code>`;
    return esc(part)
      .replace(/\*\*(.+?)\*\*/g, "<b>$1</b>")
      .replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2" target="_blank" rel="noopener">$1</a>');
  }).join("");
}
function md(src) {
  const L = src.split("\n");
  let html = "", i = 0;
  const isBlock = l => /^(```|#{1,6} |\s*[-*] |\d+\. |> |\|)/.test(l);
  while (i < L.length) {
    const l = L[i];
    if (l.startsWith("```")) {
      const lang = l.slice(3).trim(), buf = [];
      for (i++; i < L.length && !L[i].startsWith("```"); i++) buf.push(L[i]);
      i++;
      const body = buf.join("\n");
      html += `<pre><code>${["c", "cpp", "sh", "bash"].includes(lang) ? highlight(body, lang === "bash" ? "sh" : lang) : esc(body)}</code></pre>`;
    } else if (/^#{1,6} /.test(l)) {
      const n = Math.min(l.match(/^#+/)[0].length + 1, 6);
      html += `<h${n}>${inline(l.replace(/^#+ /, ""))}</h${n}>`; i++;
    } else if (/^\s*([-*]|\d+\.) /.test(l)) {
      const ordered = /^\s*\d+\. /.test(l), items = [];
      while (i < L.length && (/^\s*([-*]|\d+\.) /.test(L[i]) || (/^\s{2,}\S/.test(L[i]) && items.length))) {
        if (/^\s*([-*]|\d+\.) /.test(L[i])) items.push(L[i].replace(/^\s*([-*]|\d+\.) /, ""));
        else items[items.length - 1] += " " + L[i].trim();
        i++;
      }
      const t = ordered ? "ol" : "ul";
      html += `<${t}>${items.map(x => `<li>${inline(x)}</li>`).join("")}</${t}>`;
    } else if (l.startsWith(">")) {
      const buf = [];
      while (i < L.length && L[i].startsWith(">")) buf.push(L[i++].replace(/^> ?/, ""));
      html += `<blockquote>${md(buf.join("\n"))}</blockquote>`;
    } else if (l.startsWith("|")) {
      const rows = [];
      while (i < L.length && L[i].startsWith("|")) rows.push(L[i++]);
      const cells = r => r.replace(/^\||\|$/g, "").split("|").map(c => inline(c.trim()));
      const body = rows.filter((r, k) => !(k === 1 && /^[|\s:-]+$/.test(r)));
      html += '<div class="tbl-wrap"><table>' + body.map((r, k) => `<tr>${cells(r).map(c => k ? `<td>${c}</td>` : `<th>${c}</th>`).join("")}</tr>`).join("") + "</table></div>";
    } else if (!l.trim()) {
      i++;
    } else {
      const buf = [];
      while (i < L.length && L[i].trim() && !isBlock(L[i])) buf.push(L[i++]);
      html += `<p>${inline(buf.join(" "))}</p>`;
    }
  }
  return html;
}
