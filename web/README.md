# web — study UI

```sh
make web            # or: npm start  /  python3 web/backend/server.py [--port 8000]
```
Open http://localhost:8000. Nothing else to install: the backend uses only the Python 3 standard library, and the frontend is plain HTML/CSS/JS (no build step).

## Layout

```
web/
├── backend/                 Python 3, standard library only
│   ├── server.py            HTTP: serves the frontend + JSON API, restarts itself when a .py file changes
│   ├── config.py            paths, lab directories, time/output limits
│   ├── labs.py              reads labs (header comment of each source file) + ANSWERS.md
│   ├── theory.py            reads 09-theory/*.md -> theory + quiz questions
│   ├── runner.py            builds with the top-level Makefile, runs labs in a pty, keeps the output
│   └── jobwrap.py           runs a program the way a shell runs a foreground job (Ctrl+C/Z, orphans)
└── frontend/
    ├── index.html           page shell
    ├── css/style.css        styles (light/dark)
    └── js/                  loaded in the order listed in index.html, sharing globals
        ├── core.js          h(), api(), progress storage (localStorage), shared state
        ├── render.js        code highlighting, markdown
        ├── terminal.js      in-browser terminal + Terminal 2
        ├── nav.js           sidebar, topic colors, icons
        ├── home.js          Overview page
        ├── lab.js           lab page + prediction/output diff
        ├── theory.js        theory page
        ├── quiz.js          quizzes
        └── main.js          startup + routing (#home, #lab/<id>, #theory/<id>, #quiz)
```

Content lives outside `web/`: labs in `01-c` … `10-alp`, answers in `ANSWERS.md`, theory + quizzes in `09-theory/`. The backend re-reads these files on every API call, so editing content only needs a page reload.

## API

| Method | Path | Description |
| --- | --- | --- |
| GET | `/api/labs` | labs, source code, questions, answers |
| GET | `/api/theory` | theory topics and quiz questions |
| POST | `/api/runs` | `{id}`: build & run a lab; `{terminal: true}`: open bash |
| GET | `/api/runs/<run>?from=N` | output from chunk N, state, PID |
| POST | `/api/runs/<run>/input` | `{text}` sent to stdin |
| POST | `/api/runs/<run>/key` | `{key: "c" \| "z" \| "\\" \| "d"}` like Ctrl+C/Z/\/D |
| POST | `/api/runs/<run>/job` | `{cmd: "fg" \| "bg"}` |
| POST | `/api/runs/<run>/signal` | `{sig: "TERM" \| "USR1" \| …}` sent to the lab's PID |
| POST | `/api/runs/<run>/ps` | prints `ps` for the processes of this run |
| POST | `/api/runs/<run>/stop` | kill -9 everything |

The server listens on `127.0.0.1` only and rejects requests with an unexpected Host/Origin (because one endpoint opens bash). POST bodies must be `application/json`.
