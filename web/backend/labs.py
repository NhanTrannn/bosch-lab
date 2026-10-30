"""Lab list: reads the header comment of each source file and the answers in <lab dir>/ANSWERS.md."""
import re

from config import DIRS, EXTRA_FILES, HELPERS, LABS_DIR, READONLY, ROOT, SHELL_LAB

HEADER_KEYS = {
    "Lab": "title",
    "Topic": "topic",
    "Predict before running": "predict",
    "Key idea": "idea",
    "Try next": "try",
    "Intentional compiler warning": "warn_build",
    "Tags": "tags",
}


def parse_header(text):
    """Read the header comment: Lab / Topic / Predict... and the '!!!' warning line."""
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


def split_tags(s):
    """"fork, buffering" or "[fork, buffering]" -> ["buffering", "fork"] (lowercase, spaces -> dashes)."""
    return sorted({re.sub(r"\s+", "-", t.strip().lower()) for t in s.strip("[] ").split(",") if t.strip()})


def parse_answers(d):
    """<d>/ANSWERS.md -> ({lab name: (title, markdown)}, markdown of the whole file without its title)."""
    path = LABS_DIR / d / "ANSWERS.md"
    if not path.exists():
        return {}, ""
    sections, whole, cur_key, fence = {}, [], None, False
    for line in path.read_text(encoding="utf-8").split("\n"):
        if line.startswith("```"):
            fence = not fence
        if not fence and line.startswith("# "):
            continue
        if not fence and line.startswith("### "):
            name, _, title = line[4:].partition(" — ")
            cur_key = name.strip()
            sections[cur_key] = [title.strip(), []]
        elif cur_key:
            sections[cur_key][1].append(line)
        whole.append(line)
    return {k: (t, "\n".join(b).strip()) for k, (t, b) in sections.items()}, "\n".join(whole).strip()


def expected_output(md):
    m = re.search(r"\*\*Expected output:\*\*\s*```[^\n]*\n(.*?)\n```", md, re.S)
    return m.group(1) if m else None


def question(md):
    m = re.search(r"\*\*Question:\*\*\s*(.+)", md)
    return m.group(1).strip() if m else None


def discover():
    answers = {}
    labs = []

    def add(lab_id, files, kind):
        """files: paths relative to data/source/labs; the lab gets them relative to ROOT."""
        d, name = lab_id.split("/", 1)
        if d not in answers:
            answers[d] = parse_answers(d)
        sec, whole = answers[d]
        files = [str((LABS_DIR / f).relative_to(ROOT)) for f in files]
        src = (ROOT / files[0]).read_text(encoding="utf-8", errors="replace")
        meta = parse_header(src) if files[0].endswith((".c", ".cpp")) else {}
        key = name if name in sec else re.sub(r"_(reader|writer)$", "", name)
        title, md = sec.get(key, ("", ""))
        if kind == "shell":
            title, md = "20 shell command drills", whole
        labs.append({
            "id": lab_id, "dir": d, "name": name, "kind": kind, "files": files,
            "title": meta.get("title") or re.sub(r"\s*⚠️.*", "", title) or name,
            "unstable": "nondeterministic" in title,
            "meta": meta, "answer": md,
            "question": question(md) or meta.get("predict"),
            "expected": expected_output(md),
            "tags": split_tags(meta.get("tags", "")),
        })

    for d in DIRS:
        srcs = sorted(p for ext in ("*.c", "*.cpp") for p in (LABS_DIR / d).glob(ext))
        for src in srcs:
            rel = f"{d}/{src.name}"
            if rel in HELPERS:
                continue
            lab_id = f"{d}/{src.stem}"
            add(lab_id, [rel] + EXTRA_FILES.get(lab_id, []), "program")
    for lab_id, files in READONLY.items():
        if (LABS_DIR / files[0]).exists():
            add(lab_id, files, "readonly")
    if (LABS_DIR / f"{SHELL_LAB}.md").exists():
        add(SHELL_LAB, [f"{SHELL_LAB}.md"], "shell")
    return labs


def find_lab(lab_id):
    return next((lab for lab in discover() if lab["id"] == lab_id), None)
