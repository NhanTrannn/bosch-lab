-- bosch-lab database, generated from data/source by web/backend/db.py (make db).
-- data/bosch-lab.db is not committed: delete it any time, it is rebuilt when the sources change.

CREATE TABLE meta (
  key   TEXT PRIMARY KEY,
  value TEXT NOT NULL                 -- "signature": hash of the source files the database was built from
);

-- Theory + quiz: one topic per data/source/theory/<group>/NN-<topic>.md
CREATE TABLE topics (
  id       TEXT PRIMARY KEY,          -- <group folder>/<topic>, e.g. linux/thread-sync
  position INTEGER NOT NULL,          -- display order
  grp      TEXT NOT NULL,             -- group name shown in the UI, e.g. "Linux"
  title    TEXT NOT NULL,
  path     TEXT NOT NULL,             -- source file, relative to the repository root
  theory   TEXT NOT NULL              -- markdown of the "## Theory" part
);

CREATE TABLE questions (
  id       TEXT PRIMARY KEY,          -- the {#id} written in the markdown, e.g. linux-thread-017
  topic_id TEXT NOT NULL REFERENCES topics(id),
  position INTEGER NOT NULL,
  text     TEXT NOT NULL,             -- the "### " line (markdown, inline)
  body     TEXT NOT NULL,             -- markdown between the question and its options (code, …)
  why      TEXT NOT NULL,             -- explanation ("> " lines)
  multi    INTEGER NOT NULL           -- 1 = several right options
);
CREATE INDEX questions_by_topic ON questions(topic_id, position);

CREATE TABLE options (
  question_id TEXT NOT NULL REFERENCES questions(id),
  position    INTEGER NOT NULL,
  text        TEXT NOT NULL,
  ok          INTEGER NOT NULL,
  PRIMARY KEY (question_id, position)
);

-- Labs: data/source/labs/<dir>/<lab>.c|.cpp|.md + the entry in that directory's ANSWERS.md
CREATE TABLE labs (
  id       TEXT PRIMARY KEY,          -- <dir>/<lab>, e.g. linux-process/01_fork_count
  position INTEGER NOT NULL,
  dir      TEXT NOT NULL,
  name     TEXT NOT NULL,
  kind     TEXT NOT NULL,             -- program | readonly | shell
  title    TEXT NOT NULL,
  unstable INTEGER NOT NULL,          -- 1 = nondeterministic output
  meta     TEXT NOT NULL,             -- JSON: the header comment (topic, predict, idea, try, warning, tags…)
  answer   TEXT NOT NULL,             -- markdown of the ANSWERS.md entry
  question TEXT,
  expected TEXT                       -- the "Expected output" block
);

CREATE TABLE lab_files (
  lab_id   TEXT NOT NULL REFERENCES labs(id),
  position INTEGER NOT NULL,          -- 0 = the main file
  path     TEXT NOT NULL,             -- relative to the repository root
  content  TEXT NOT NULL,
  PRIMARY KEY (lab_id, position)
);

-- Tags: "tags:" front matter of a theory file (applies to the topic, its sections and questions),
-- or the "Tags:" line of a lab's header comment
CREATE TABLE tags (
  kind    TEXT NOT NULL,              -- topic | lab
  item_id TEXT NOT NULL,
  tag     TEXT NOT NULL,
  PRIMARY KEY (kind, item_id, tag)
);
CREATE INDEX tags_by_tag ON tags(tag);

-- Full-text search over labs, theory sections and quiz questions
CREATE VIRTUAL TABLE search USING fts5(
  kind UNINDEXED,                     -- lab | theory | quiz
  item_id UNINDEXED,                  -- lab id, "<topic id>#<section index>" or question id
  grp UNINDEXED,                      -- UI group of the item (C, C++, Linux, …)
  tags UNINDEXED,                     -- space-separated, with a space at both ends: " fork buffering "
  title,
  body,
  tokenize = 'unicode61'
);
