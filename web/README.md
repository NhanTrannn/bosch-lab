# web — study UI

```sh
make web            # or: npm start  /  python3 web/backend/server.py [--port 8000]
```
Open http://localhost:8000. Nothing else to install: the backend uses only the Python 3 standard library, and the frontend is plain HTML/CSS/JS (no build step).

## Layout

```
web/
├── backend/                 Python 3, standard library only (http.server + sqlite3)
│   ├── server.py            HTTP: serves the frontend + JSON API, restarts itself when a .py file changes
│   ├── config.py            paths, lab directories, theory groups, time/output limits
│   ├── labs.py              parses labs (header comment of each source file) + each lab dir's ANSWERS.md
│   ├── theory.py            parses data/source/theory/<group>/*.md -> theory + quiz questions (stable {#id}, tags)
│   ├── db.py                builds data/bosch-lab.db from the parsers (schema: data/schema.sql), reads it, full-text search
│   ├── runner.py            builds with the top-level Makefile, runs labs in a pty, keeps the output
│   └── jobwrap.py           runs a program the way a shell runs a foreground job (Ctrl+C/Z, orphans)
└── frontend/                no build step: index.html loads js/main.js as an ES module, which imports the rest
    ├── index.html           page shell
    ├── css/style.css        styles (light/dark)
    └── js/
        ├── core.js          h(), api(), progress storage (localStorage), migrations, shared state + set({...}), theme
        ├── render.js        code highlighting, markdown
        ├── terminal.js      in-browser terminal + Terminal 2
        ├── nav.js           sidebar, topic colors, icons
        ├── home.js          Overview page
        ├── lab.js           lab page + prediction/output diff
        ├── labindex.js      lab topics (LAB_TOPIC_DEFS), lab index and topic pages
        ├── theory.js        theory pages
        ├── quiz.js          quizzes
        ├── search.js        search box + results page (#search/<words>)
        └── main.js          startup + routing (#home, #labs, #lab/<id>, #theory/<id>, #quiz, #search/<q>)
```

**Content** lives outside `web/`, in `data/source/` (see `data/README.md`). The backend keeps a SQLite copy in
`data/bosch-lab.db`: on every API call `db.ensure()` compares a hash of the source files (path, size, mtime) with the
one stored in the database and rebuilds it if anything changed (about 0.1 s), so editing content still only needs a
page reload. The database is a cache: delete it any time.

**Frontend modules.** Each file imports what it uses (`import { go } from "./lab.js"`). The shared state (`mode`,
`LABS`, `TOPICS`, `current`, `quiz`, …) lives in `core.js`; other modules read it as live imports and change it with
`set({ mode: "lab", current: lab })`, because an imported binding cannot be assigned. Cyclic imports between pages are
fine as long as a module's top level only *defines* things (no calls into other modules while loading).

**Test.** `make test-web` (with `make web` running) loads the modules in gjs with a fake DOM and the real API, opens
every lab/topic/page, answers quizzes by clicking, uses the search page, and fails on any missing import or error.

## API

| Method | Path | Description |
| --- | --- | --- |
| GET | `/api/labs` | labs, source code, questions, answers |
| GET | `/api/theory` | theory topics and quiz questions |
| GET | `/api/search?q=…&kind=lab\|theory\|quiz&tag=…` | full-text search (every word as a prefix, ranked by bm25); matches are wrapped in `\x02…\x03` |
| GET | `/api/tags` | `[{tag, count}]` |
| GET | `/api/migrations` | `data/migrations/*.json`: old id → new id, applied once to the saved progress |
| POST | `/api/runs` | `{id}`: build & run a lab; `{terminal: true}`: open bash |
| GET | `/api/runs/<run>?from=N` | output from chunk N, state, PID |
| POST | `/api/runs/<run>/input` | `{text}` sent to stdin |
| POST | `/api/runs/<run>/key` | `{key: "c" \| "z" \| "\\" \| "d"}` like Ctrl+C/Z/\/D |
| POST | `/api/runs/<run>/job` | `{cmd: "fg" \| "bg"}` |
| POST | `/api/runs/<run>/signal` | `{sig: "TERM" \| "USR1" \| …}` sent to the lab's PID |
| POST | `/api/runs/<run>/ps` | prints `ps` for the processes of this run |
| POST | `/api/runs/<run>/stop` | kill -9 everything |

The server listens on `127.0.0.1` only and rejects requests with an unexpected Host/Origin (because one endpoint opens bash). POST bodies must be `application/json`.
