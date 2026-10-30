#!/usr/bin/env python3
"""Content tools for data/source (run from anywhere).

    python3 tools/content.py check   report format problems; exit code 1 if there are errors   (make check)
    python3 tools/content.py ids     append a stable {#id} to every quiz question that has none  (make ids)

Uses the same parsers as the web backend, so "check passes" means the web UI reads the files the same way.
"""
import re
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "web" / "backend"))
from config import ROOT  # noqa: E402
from labs import discover  # noqa: E402
from theory import QID, parse_quiz, topic_files  # noqa: E402


def quiz_headings(path):
    """(line index, heading line) of every question in the "## Quiz" section, skipping code blocks."""
    out, in_quiz, fence = [], False, False
    for i, line in enumerate(path.read_text(encoding="utf-8").split("\n")):
        if line.startswith("```"):
            fence = not fence
        elif not fence and line.startswith("## "):
            in_quiz = line.startswith("## Quiz")
        elif in_quiz and not fence and line.startswith("### "):
            out.append((i, line))
    return out


def check():
    errors, warnings = [], []
    err = lambda where, msg: errors.append(f"{where}: {msg}")
    warn = lambda where, msg: warnings.append(f"{where}: {msg}")
    seen = {}
    for folder, path in topic_files():
        rel = path.relative_to(ROOT)
        text = path.read_text(encoding="utf-8")
        if not re.search(r"^# .+", text, re.M):
            err(rel, "missing the '# Title' line")
        if "\n## Quiz" not in text:
            continue
        heads = quiz_headings(path)
        for i, line in heads:
            m = QID.search(line)
            if not m:
                err(f"{rel}:{i + 1}", "question has no {#id} (run: make ids)")
            elif m.group(1) in seen:
                err(f"{rel}:{i + 1}", f"duplicate id {m.group(1)} (also in {seen[m.group(1)]})")
            else:
                seen[m.group(1)] = f"{rel}:{i + 1}"
        parsed = parse_quiz(folder, text.split("\n## Quiz", 1)[1])
        if len(parsed) < len(heads):
            err(rel, f"{len(heads) - len(parsed)} question(s) ignored: they need at least 2 options and one '- [x]'")
        for q in parsed:
            if not q["why"]:
                warn(rel, f"no '> explanation' for: {q['text'][:60]}")
    for lab in discover():
        where = lab["files"][0]
        if lab["kind"] == "program" and not lab["answer"]:
            warn(where, "no entry in ANSWERS.md")
        elif lab["kind"] == "program" and not lab["expected"]:
            warn(where, "ANSWERS.md entry has no **Expected output:** block")
    for w in warnings:
        print("warning:", w)
    for e in errors:
        print("error:", e)
    print(f"{len(seen)} questions, {len(errors)} error(s), {len(warnings)} warning(s)")
    return 1 if errors else 0


def assign_ids():
    files = topic_files()
    used = Counter()
    for _, path in files:
        for _, line in quiz_headings(path):
            m = QID.search(line)
            if m:
                prefix, _, num = m.group(1).rpartition("-")
                used[prefix] = max(used[prefix], int(num) if num.isdigit() else 0)
    added = 0
    for folder, path in files:
        heads = quiz_headings(path)
        missing = [i for i, line in heads if not QID.search(line)]
        if not missing:
            continue
        # Reuse the prefix most ids in this file have, else build one from the path: cpp/02-functions.md -> cpp-functions
        prefixes = Counter(QID.search(l).group(1).rpartition("-")[0] for _, l in heads if QID.search(l))
        prefix = prefixes.most_common(1)[0][0] if prefixes else folder + "-" + re.sub(r"^\d+-", "", path.stem)
        lines = path.read_text(encoding="utf-8").split("\n")
        for i in missing:
            used[prefix] += 1
            lines[i] = f"{lines[i].rstrip()} {{#{prefix}-{used[prefix]:03d}}}"
            added += 1
        path.write_text("\n".join(lines), encoding="utf-8")
        print(f"{path.relative_to(ROOT)}: {len(missing)} id(s) added")
    print(f"{added} id(s) added")
    return 0


if __name__ == "__main__":
    cmd = sys.argv[1] if len(sys.argv) > 1 else ""
    if cmd == "check":
        sys.exit(check())
    if cmd == "ids":
        sys.exit(assign_ids())
    sys.exit(__doc__)
