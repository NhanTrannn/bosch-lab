"""SQLite database built from data/source (schema: data/schema.sql).

The markdown and C/C++ files stay the source of truth; the database is a cache + search index.
ensure() rebuilds it whenever a source file changes, so editing content still only needs a page reload.
"""
import hashlib
import json
import os
import re
import sqlite3
import threading

from config import DB_PATH, ROOT, SCHEMA, SOURCE
from labs import discover
from theory import parse_theory

_lock = threading.Lock()


def signature():
    """Hash of every source file's path, size and mtime, plus the schema and this module."""
    h = hashlib.sha1()
    for path in sorted(p for p in SOURCE.rglob("*") if p.is_file()):
        st = path.stat()
        h.update(f"{path.relative_to(SOURCE)}\0{st.st_size}\0{st.st_mtime_ns}\n".encode())
    for path in (SCHEMA, __file__):
        h.update(open(path, "rb").read())
    return h.hexdigest()


def connect():
    con = sqlite3.connect(DB_PATH)
    con.row_factory = sqlite3.Row
    return con


def ensure():
    """Rebuild the database if the sources changed since it was built."""
    sig = signature()
    with _lock:
        try:
            with connect() as con:
                if con.execute("SELECT value FROM meta WHERE key = 'signature'").fetchone()[0] == sig:
                    return False
        except (sqlite3.Error, TypeError):
            pass
        build(sig)
        return True


def sections(theory_md):
    """Split a topic's theory into its "### " sections: [(title, markdown)]."""
    out, fence = [], False
    for line in theory_md.split("\n"):
        if line.startswith("```"):
            fence = not fence
        if not fence and line.startswith("### "):
            out.append([line[4:].strip(), []])
        elif out:
            out[-1][1].append(line)
    return [(t, "\n".join(b).strip()) for t, b in out]


def build(sig=None):
    """Write a new database next to the old one, then swap it in (readers never see a half-built file)."""
    tmp = DB_PATH.with_suffix(".db.tmp")
    tmp.unlink(missing_ok=True)
    con = sqlite3.connect(tmp)
    con.executescript(SCHEMA.read_text(encoding="utf-8"))
    ins = lambda table, row: con.execute(f"INSERT INTO {table} VALUES ({','.join('?' * len(row))})", row)
    tag_str = lambda tags: " " + " ".join(tags) + " " if tags else ""

    for pos, t in enumerate(parse_theory()):
        ins("topics", (t["id"], pos, t["group"], t["title"], t["path"], t["theory"]))
        for tag in t["tags"]:
            ins("tags", ("topic", t["id"], tag))
        for i, (title, body) in enumerate(sections(t["theory"])):
            ins("search", ("theory", f"{t['id']}#{i}", t["group"], tag_str(t["tags"]), title, body))
        for qpos, q in enumerate(t["questions"]):
            ins("questions", (q["id"], t["id"], qpos, q["text"], q["body"], q["why"], int(q["multi"])))
            for opos, o in enumerate(q["options"]):
                ins("options", (q["id"], opos, o["text"], int(o["ok"])))
            body = "\n".join([q["body"], *(o["text"] for o in q["options"]), q["why"]])
            ins("search", ("quiz", q["id"], t["group"], tag_str(t["tags"]), q["text"], body))

    for pos, lab in enumerate(discover()):
        m = lab["meta"]
        ins("labs", (lab["id"], pos, lab["dir"], lab["name"], lab["kind"], lab["title"], int(lab["unstable"]),
                     json.dumps(m, ensure_ascii=False), lab["answer"], lab["question"], lab["expected"]))
        for fpos, f in enumerate(lab["files"]):
            ins("lab_files", (lab["id"], fpos, f, (ROOT / f).read_text(encoding="utf-8", errors="replace")))
        for tag in lab["tags"]:
            ins("tags", ("lab", lab["id"], tag))
        body = "\n".join(filter(None, [m.get("topic"), lab["question"], m.get("idea"), m.get("try"), lab["answer"]]))
        ins("search", ("lab", lab["id"], lab["dir"], tag_str(lab["tags"]), f"{lab['name']} — {lab['title']}", body))

    con.execute("INSERT INTO meta VALUES ('signature', ?)", (sig or signature(),))
    con.commit()
    con.close()
    os.replace(tmp, DB_PATH)


# ---------- reads (same JSON as the parsers return, so the frontend does not care where it comes from)
def labs():
    ensure()
    with connect() as con:
        files = {}
        for r in con.execute("SELECT lab_id, path, content FROM lab_files ORDER BY lab_id, position"):
            files.setdefault(r["lab_id"], []).append((r["path"], r["content"]))
        tags = {}
        for r in con.execute("SELECT item_id, tag FROM tags WHERE kind = 'lab' ORDER BY tag"):
            tags.setdefault(r["item_id"], []).append(r["tag"])
        out = []
        for r in con.execute("SELECT * FROM labs ORDER BY position"):
            fs = files.get(r["id"], [])
            out.append({"id": r["id"], "dir": r["dir"], "name": r["name"], "kind": r["kind"], "files": [p for p, _ in fs],
                        "title": r["title"], "unstable": bool(r["unstable"]), "meta": json.loads(r["meta"]),
                        "answer": r["answer"], "question": r["question"], "expected": r["expected"],
                        "tags": tags.get(r["id"], []), "sources": dict(fs)})
        return out


def topics():
    ensure()
    with connect() as con:
        opts = {}
        for r in con.execute("SELECT question_id, text, ok FROM options ORDER BY question_id, position"):
            opts.setdefault(r["question_id"], []).append({"text": r["text"], "ok": bool(r["ok"])})
        qs = {}
        for r in con.execute("SELECT * FROM questions ORDER BY topic_id, position"):
            qs.setdefault(r["topic_id"], []).append({"id": r["id"], "text": r["text"], "body": r["body"], "options": opts.get(r["id"], []),
                                                     "multi": bool(r["multi"]), "why": r["why"]})
        tags = {}
        for r in con.execute("SELECT item_id, tag FROM tags WHERE kind = 'topic' ORDER BY tag"):
            tags.setdefault(r["item_id"], []).append(r["tag"])
        return [{"id": r["id"], "title": r["title"], "group": r["grp"], "path": r["path"], "tags": tags.get(r["id"], []),
                 "theory": r["theory"], "questions": qs.get(r["id"], [])}
                for r in con.execute("SELECT * FROM topics ORDER BY position")]


def all_tags():
    """[{tag, count}] over topics and labs, most used first."""
    ensure()
    with connect() as con:
        return [dict(r) for r in con.execute("SELECT tag, COUNT(*) AS count FROM tags GROUP BY tag ORDER BY count DESC, tag")]


def search(q, kind=None, group=None, tag=None, limit=60):
    """Full-text search. Every word must match (as a prefix); results are ranked by bm25, titles count more.
    Matches in title/snippet are wrapped in \\x02 … \\x03 (the frontend escapes the text, then turns them into <mark>)."""
    words = re.findall(r"\w+", q or "")
    if not words:
        return []
    ensure()
    sql = ("SELECT kind, item_id, grp, highlight(search, 4, char(2), char(3)) AS title,"
           " snippet(search, 5, char(2), char(3), '…', 16) AS snippet"
           " FROM search WHERE search MATCH ?")
    args = [" ".join(f'"{w}"*' for w in words)]
    if kind:
        sql += " AND kind = ?"; args.append(kind)
    if group:
        sql += " AND grp = ?"; args.append(group)
    if tag:
        sql += " AND tags LIKE ?"; args.append(f"% {tag} %")
    sql += " ORDER BY bm25(search, 0, 0, 0, 0, 6.0, 1.0) LIMIT ?"
    args.append(limit)
    with connect() as con:
        return [dict(r) for r in con.execute(sql, args)]


if __name__ == "__main__":  # make db: rebuild now and print what is inside
    build()
    with connect() as con:
        n = {t: con.execute(f"SELECT COUNT(*) FROM {t}").fetchone()[0] for t in ("topics", "questions", "labs", "tags", "search")}
    print(f"{DB_PATH.relative_to(ROOT)}: " + ", ".join(f"{v} {k}" for k, v in n.items()))
