"""Paths and constants shared by the backend."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]   # the bosch-lab directory
BACKEND = Path(__file__).resolve().parent
FRONTEND = BACKEND.parent / "frontend"

# Content written by hand (markdown + C/C++), see data/README.md
SOURCE = ROOT / "data" / "source"
LABS_DIR = SOURCE / "labs"          # labs/<dir>/<lab>.c|.cpp + ANSWERS.md
THEORY_DIR = SOURCE / "theory"      # theory/<group>/NN-<topic>.md
MIGRATIONS_DIR = ROOT / "data" / "migrations"
# Generated from SOURCE (cache + search index), rebuilt automatically when a source file changes
DB_PATH = ROOT / "data" / "bosch-lab.db"
SCHEMA = ROOT / "data" / "schema.sql"

# Lab directories built by the top-level Makefile (same list as DIRS there), in display order
DIRS = ["c", "cpp", "linux-process", "linux-signal", "linux-thread", "linux-ipc", "alp", "design-patterns"]
HELPERS = {"c/04_helper.c"}
EXTRA_FILES = {"c/04_static_extern": ["c/04_helper.c"]}
READONLY = {"kernel-module/hello": ["kernel-module/hello.c", "kernel-module/Makefile", "kernel-module/README.md"]}
SHELL_LAB = "shell/drills"

# Theory/quiz groups in display order: theory/<folder> -> group name.
# Topics inside a folder are ordered by their NN- file prefix; theory/private/ goes into "Personal notes".
THEORY_GROUPS = [
    ("c", "C"),
    ("cpp", "C++"),
    ("linux", "Linux"),
    ("alp", "Advanced Linux (ALP)"),
    ("build-kernel", "Build, debug & kernel"),
    ("design-patterns", "Design patterns"),
    ("tricky", "Tricky questions"),
]
OTHER_GROUP = "Personal notes"
SHELL_ARGV = ["bash", "--norc", "--noprofile", "--noediting", "-i"]

PROGRAM_LIMIT = 300      # seconds, then the whole group is killed
SHELL_LIMIT = 3 * 3600
OUTPUT_LIMIT = 2_000_000  # characters kept per run
