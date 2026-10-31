# data — content of bosch-lab

```
data/
├── source/                       written by hand, the only place to edit content
│   ├── labs/<dir>/               NN_name.c | NN_name.cpp + ANSWERS.md (+ README.md / extra files)
│   └── theory/<group>/           NN-topic.md (theory + quiz); private/ is not committed
├── migrations/YYYY-MM-DD-*.json  old id -> new id, applied once to the progress saved in the browser
├── schema.sql                    tables of the database
└── bosch-lab.db                  generated: cache + search index (not committed)
```

Markdown and C/C++ files are the source of truth: easy to edit in VS Code, reviewable with `git diff`.
Anything generated from them (binaries in `build/`, the database `bosch-lab.db`) can be deleted and rebuilt.
The web server rebuilds the database by itself whenever a source file changes; `make db` does it by hand.

After editing, run `make check` (or reload the web UI — it rereads the files on every request).

## Labs: `source/labs/<dir>/`

| Dir | Group in the UI | Built by `make` |
| --- | --- | --- |
| `c`, `cpp` | C, C++ | yes |
| `linux-process`, `linux-signal`, `linux-thread`, `linux-ipc` | Linux system programming | yes (`linux-thread` adds `-pthread`, `linux-ipc` `-lrt`) |
| `alp` | Advanced Linux (ALP) | yes (`-pthread -ldl -lrt`) |
| `design-patterns` | Design patterns | yes (`-pthread`) |
| `kernel-module` | Kernel & shell | no — build it in its own directory |
| `shell` | Kernel & shell | no — `drills.md` only |

A lab's id is `<dir>/<file name without extension>`, e.g. `linux-ipc/01_pipe`. Keep the `NN_` prefix: it sets the order.

### A new lab
1. Create `source/labs/<dir>/NN_name.c` starting with the header comment the web UI reads:
   ```c
   /*
    * Lab: one-line title
    * Topic: what it is about
    * Predict before running: the question to answer before running it
    * Key idea: the explanation in one or two sentences
    * Try next: an experiment to try afterwards
    * Intentional compiler warning: (only if there is one) which warning and why
    * Tags: (optional) fork, buffering
    * !!! (optional) a warning shown in red, e.g. "Needs 2 terminals"
    */
   ```
2. Add its entry to `source/labs/<dir>/ANSWERS.md`:
   ````markdown
   ### NN_name — title

   **Question:** …

   **Expected output:**

   ```text
   exact output of the program
   ```

   **Explanation:** …
   ````
   Put `⚠️ nondeterministic` in the title when the output changes between runs (the UI then skips the exact diff).
3. Add the lab number to a topic in `web/frontend/js/labindex.js` (`LAB_TOPIC_DEFS`); unlisted labs show up under "Other".
4. A new directory also needs to be added to `DIRS` in the `Makefile` and in `web/backend/config.py`.

## Theory + quizzes: `source/theory/<group>/NN-topic.md`

The folder is the group (`c`, `cpp`, `linux`, `alp`, `build-kernel`, `design-patterns`, `tricky`; names in `THEORY_GROUPS` in
`web/backend/config.py`), the `NN-` prefix the order. The topic id is `<group>/<topic>` without the prefix, e.g. `linux/thread-sync`,
so renumbering a file does not lose anything.

```markdown
---
tags: [linux, threads, mutex]
---

# Topic title

## Theory

### Section
… any markdown: tables, code blocks …

## Quiz

### Question text {#linux-thread-017}
```c
optional code shown with the question
```
- [ ] wrong option
- [x] right option          (several [x] = "select all that apply")
- [ ] wrong option
> Explanation shown after answering.
```

### Outline of the theory part: I → 1 → a

The web UI numbers the theory automatically, so **never type the numbers yourself** (inserting a section later would
mean renumbering everything by hand):

| You write | Shown as | Use it for |
| --- | --- | --- |
| `### Title` | **I.** II. III. | a part of the topic |
| `#### Title` | **1.** 2. 3. (restarts in every part) | a section, when a part is long |
| `- point` right under a `###` | 1. 2. 3. | the points of a short part |
| `- point` under a `####` | a. b. c. | the points of a section |
| `  - sub-point` (indented 2 spaces) | one level deeper: 1 → a → i | details of a point |

```markdown
### Threads                         →  I. Threads
#### Creating threads               →    1. Creating threads
- pthread_create(...)               →       a. pthread_create(...)
  - returns 0 or an error code      →          i. returns 0 or an error code
- pthread_join(...)                 →       b. pthread_join(...)
#### Cancellation                   →    2. Cancellation
```

- Start each point with the **key term in bold**, then the explanation: `- **Detached thread**: cleans up by itself…`.
- Tables and code blocks can go anywhere; they are not numbered.
- More than about 9 points in one `###` part → split it into `####` sections (`make check` warns).
- The table of contents on the right lists the `###` parts and their `####` sections.
- VS Code's preview (Ctrl+Shift+V) shows plain bullets; the numbers only appear in the web UI.

### Other rules

- The `tags:` block at the top is optional; tags apply to the topic, its sections and its questions, and are the
  filter chips on the search page. Use lowercase words joined with `-` (e.g. `smart-pointers`).
- Options are shuffled, so don't write "both of the above".
- **`{#id}` is the question's permanent id**: your answers and "got it wrong" history are stored under it.
  Edit the question text freely, but never change or reuse an id.
- Write new questions **without** an id and run `make ids`: it appends the next free id (same prefix as the rest of the file).
- `make check` reports missing/duplicate ids, questions with no right option or fewer than 2 options, and missing explanations.

## Renaming or moving things

Lab ids, topic ids and question ids are keys of the progress stored in the browser. When you rename one, add a file
`migrations/YYYY-MM-DD-what.json`:

```json
{ "labs": { "old/lab": "new/lab" }, "topics": { "old/topic": "new/topic" }, "questions": { "old-id": "new-id" } }
```

The web UI applies each file once (it remembers which ones it applied).
