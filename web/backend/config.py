"""Paths and constants shared by the backend."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]   # the bosch-lab directory
BACKEND = Path(__file__).resolve().parent
FRONTEND = BACKEND.parent / "frontend"

# Same as the top-level Makefile
DIRS = ["01-c", "02-cpp", "03-process", "04-signal", "05-thread", "06-ipc", "10-alp", "11-design-patterns"]
HELPERS = {"01-c/04_helper.c"}
EXTRA_FILES = {"01-c/04_static_extern": ["01-c/04_helper.c"]}
READONLY = {
    "07-kernel-module/hello": ["07-kernel-module/hello.c", "07-kernel-module/Makefile", "07-kernel-module/README.md"],
}
SHELL_LAB = "08-shell/drills"

# Theory/quiz groups, in display order; each lists 09-theory/*.md stems in reading order.
# Files not listed here (e.g. 09-theory/private/*.md) go into "Personal notes" at the end.
THEORY_GROUPS = [
    ("C", ["01-c"]),
    ("C++", ["10-cpp-basics", "18-cpp-w3s-basics", "11-cpp-functions-pointers", "12-cpp-classes-operators",
             "19-cpp-w3s-classes-errors", "02-cpp-oop", "13-cpp-exceptions-io-stl", "20-cpp-w3s-stl",
             "03-cpp-stl-modern"]),
    ("Linux", ["04-process-memory", "05-signal-ipc", "06-thread-sync", "07-linux-shell-fs"]),
    ("Advanced Linux (ALP)", ["14-alp-ch1-2-gnu-tools", "15-alp-ch3-processes", "16-alp-ch4-threads", "17-alp-ch5-ipc"]),
    ("Build, debug & kernel", ["08-build-debug", "09-kernel-embedded"]),
    ("Design patterns", ["21-design-patterns"]),
    ("Tricky questions", ["22-tricky-questions"]),
]
OTHER_GROUP = "Personal notes"
SHELL_ARGV = ["bash", "--norc", "--noprofile", "--noediting", "-i"]

PROGRAM_LIMIT = 300      # seconds, then the whole group is killed
SHELL_LIMIT = 3 * 3600
OUTPUT_LIMIT = 2_000_000  # characters kept per run
