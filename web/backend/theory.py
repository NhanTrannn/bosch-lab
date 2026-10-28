"""Theory + quizzes: each 09-theory/*.md file is one topic."""
import hashlib
import re

from config import ROOT


def parse_quiz(stem, text):
    """The "## Quiz" section: each "### " is a question, "- [ ]"/"- [x]" are options, "> " is the explanation."""
    questions, cur, fence = [], None, False
    for line in text.split("\n"):
        if line.startswith("```"):
            fence = not fence
        if not fence and line.startswith("### "):
            cur = {"text": line[4:].strip(), "body": [], "options": [], "why": []}
            questions.append(cur)
        elif cur is None:
            continue
        elif not fence and re.match(r"- \[[ xX]\] ", line):
            cur["options"].append({"text": line[6:].strip(), "ok": line[3] in "xX"})
        elif not fence and line.startswith(">"):
            cur["why"].append(line[1:].strip())
        elif not cur["options"]:
            cur["body"].append(line)
    out = []
    for q in questions:
        body = "\n".join(q["body"]).strip()
        n_ok = sum(o["ok"] for o in q["options"])
        if len(q["options"]) < 2 or n_ok == 0:
            continue
        digest = hashlib.sha1((q["text"] + body).encode()).hexdigest()[:10]
        out.append({"id": f"{stem}:{digest}", "text": q["text"], "body": body, "options": q["options"],
                    "multi": n_ok > 1, "why": "\n".join(q["why"])})
    return out


def parse_theory():
    topics = []
    # private/: personal notes (e.g. copied from a Udemy course), kept out of git
    theory_dir = ROOT / "09-theory"
    for path in sorted(theory_dir.glob("*.md")) + sorted(theory_dir.glob("private/*.md")):
        text = path.read_text(encoding="utf-8")
        m = re.search(r"^# (.+)$", text, re.M)
        parts = re.split(r"\n## Quiz[^\n]*", text, maxsplit=1)
        theory, quiz = parts[0], parts[1] if len(parts) > 1 else ""
        theory = re.sub(r"^# .+\n+", "", theory, count=1)
        theory = re.sub(r"^## Theory\n+", "", theory.strip(), count=1)
        topics.append({"id": path.stem, "title": m.group(1).strip() if m else path.stem,
                       "theory": theory, "questions": parse_quiz(path.stem, quiz)})
    return topics
