"""Danh sách bài lab: đọc comment đầu file nguồn và đáp án trong ANSWERS.md."""
import re

from config import DIRS, EXTRA_FILES, HELPERS, READONLY, ROOT, SHELL_LAB

HEADER_KEYS = {
    "Bài": "title",
    "Chủ đề": "topic",
    "Dự đoán trước khi chạy": "predict",
    "Ý chính": "idea",
    "Thử thêm": "try",
    "Cảnh báo biên dịch cố ý": "warn_build",
}


def parse_header(text):
    """Đọc comment đầu file: Bài / Chủ đề / Dự đoán... và dòng cảnh báo '!!!'."""
    m = re.match(r"\s*/\*(.*?)\*/", text, re.S)
    meta = {}
    if not m:
        return meta
    key = None
    for raw in m.group(1).split("\n"):
        line = re.sub(r"^\s*\* ?", "", raw).rstrip()
        hit = re.match(r"(%s):\s*(.*)" % "|".join(map(re.escape, HEADER_KEYS)), line)
        if hit:
            key = HEADER_KEYS[hit.group(1)]
            meta[key] = hit.group(2)
        elif line.startswith("!!!"):
            meta["warning"] = line.lstrip("! ")
            key = None
        elif key and line:
            meta[key] += "\n" + line
    return meta


def parse_answers():
    """ANSWERS.md → ({dir: {tên bài: (tiêu đề, markdown)}}, {dir: markdown cả phần})."""
    path = ROOT / "ANSWERS.md"
    sections, whole = {}, {}
    if not path.exists():
        return sections, whole
    cur_dir, cur_key, fence = None, None, False
    for line in path.read_text(encoding="utf-8").split("\n"):
        if line.startswith("```"):
            fence = not fence
        if not fence and line.startswith("## "):
            cur_dir, cur_key = line[3:].strip(), None
            sections.setdefault(cur_dir, {})
            whole[cur_dir] = []
            continue
        if not fence and line.startswith("### ") and cur_dir:
            name, _, title = line[4:].partition(" — ")
            cur_key = name.strip()
            sections[cur_dir][cur_key] = [title.strip(), []]
            whole[cur_dir].append(line)
            continue
        if cur_dir:
            whole[cur_dir].append(line)
            if cur_key:
                sections[cur_dir][cur_key][1].append(line)
    sections = {d: {k: (t, "\n".join(b).strip()) for k, (t, b) in v.items()} for d, v in sections.items()}
    return sections, {d: "\n".join(b).strip() for d, b in whole.items()}


def expected_output(md):
    m = re.search(r"\*\*Output mong đợi:\*\*\s*```[^\n]*\n(.*?)\n```", md, re.S)
    return m.group(1) if m else None


def question(md):
    m = re.search(r"\*\*Câu hỏi:\*\*\s*(.+)", md)
    return m.group(1).strip() if m else None


def discover():
    answers, whole = parse_answers()
    labs = []

    def add(lab_id, files, kind):
        d, name = lab_id.split("/", 1)
        src = (ROOT / files[0]).read_text(encoding="utf-8", errors="replace")
        meta = parse_header(src) if files[0].endswith((".c", ".cpp")) else {}
        sec = answers.get(d, {})
        key = name if name in sec else re.sub(r"_(reader|writer)$", "", name)
        title, md = sec.get(key, ("", ""))
        if kind == "shell":
            title, md = "20 bài lệnh shell", whole.get(d, "")
        labs.append({
            "id": lab_id, "dir": d, "name": name, "kind": kind, "files": files,
            "title": meta.get("title") or re.sub(r"\s*⚠️.*", "", title) or name,
            "unstable": "không cố định" in title,
            "meta": meta, "answer": md,
            "question": question(md) or meta.get("predict"),
            "expected": expected_output(md),
        })

    for d in DIRS:
        srcs = sorted(p for ext in ("*.c", "*.cpp") for p in (ROOT / d).glob(ext))
        for src in srcs:
            rel = f"{d}/{src.name}"
            if rel in HELPERS:
                continue
            lab_id = f"{d}/{src.stem}"
            add(lab_id, [rel] + EXTRA_FILES.get(lab_id, []), "program")
    for lab_id, files in READONLY.items():
        if (ROOT / files[0]).exists():
            add(lab_id, files, "readonly")
    if (ROOT / "08-shell/drills.md").exists():
        add(SHELL_LAB, ["08-shell/drills.md"], "shell")
    return labs


def find_lab(lab_id):
    return next((lab for lab in discover() if lab["id"] == lab_id), None)
