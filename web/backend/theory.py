"""Theory + quizzes: each data/source/theory/<group>/NN-<topic>.md file is one topic."""
import hashlib
import re

from config import OTHER_GROUP, ROOT, THEORY_DIR, THEORY_GROUPS
from labs import split_tags

GROUP_OF = dict(THEORY_GROUPS)
QID = re.compile(r"\s*\{#([\w-]+)\}\s*$")  # stable question id at the end of the heading: ### text {#c-001}


def parse_quiz(topic_id, text):
    """The "## Quiz" section: each "### " is a question, "- [ ]"/"- [x]" are options, "> " is the explanation.
    A question without a {#id} gets a hash id, which changes whenever its text is edited (tools/content.py ids adds ids)."""
    questions, cur, fence = [], None, False
    for line in text.split("\n"):
        if line.startswith("```"):
            fence = not fence
        if not fence and line.startswith("### "):
            m = QID.search(line)
            cur = {"text": QID.sub("", line[4:]).strip(), "qid": m and m.group(1), "body": [], "options": [], "why": []}
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
        qid = q["qid"] or f"{topic_id}:" + hashlib.sha1((q["text"] + body).encode()).hexdigest()[:10]
        out.append({"id": qid, "text": q["text"], "body": body, "options": q["options"],
                    "multi": n_ok > 1, "why": "\n".join(q["why"])})
    return out


def front_matter(text):
    """Optional block at the very top of a file:  ---\ntags: [fork, buffering]\n---  ->  ({"tags": "[fork, buffering]"}, rest)."""
    m = re.match(r"---\n(.*?)\n---\n+", text, re.S)
    if not m:
        return {}, text
    meta = dict(re.match(r"(\w[\w-]*):\s*(.*)", l).groups() for l in m.group(1).split("\n") if re.match(r"\w[\w-]*:", l))
    return meta, text[m.end():]


def topic_files():
    """(group folder, path) in display order: groups as in THEORY_GROUPS, then files by name; private/ last."""
    order = [folder for folder, _ in THEORY_GROUPS]
    folders = sorted((p for p in THEORY_DIR.iterdir() if p.is_dir()),
                     key=lambda p: (p.name == "private", order.index(p.name) if p.name in order else len(order), p.name))
    return [(f.name, path) for f in folders for path in sorted(f.glob("*.md"))]


def parse_theory():
    topics = []
    for folder, path in topic_files():
        text = path.read_text(encoding="utf-8")
        front, text = front_matter(text)
        m = re.search(r"^# (.+)$", text, re.M)
        parts = re.split(r"\n## Quiz[^\n]*", text, maxsplit=1)
        theory, quiz = parts[0], parts[1] if len(parts) > 1 else ""
        theory = re.sub(r"^# .+\n+", "", theory, count=1)
        theory = re.sub(r"^## Theory\n+", "", theory.strip(), count=1)
        topic_id = folder + "/" + re.sub(r"^\d+-", "", path.stem)  # the NN- prefix only sets the order
        topics.append({"id": topic_id, "title": m.group(1).strip() if m else path.stem,
                       "group": GROUP_OF.get(folder, OTHER_GROUP), "path": str(path.relative_to(ROOT)),
                       "tags": split_tags(front.get("tags", "")),
                       "theory": theory, "questions": parse_quiz(topic_id, quiz)})
    return topics
