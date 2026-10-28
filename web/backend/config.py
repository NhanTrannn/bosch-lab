"""Paths and constants shared by the backend."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]   # the bosch-lab directory
BACKEND = Path(__file__).resolve().parent
FRONTEND = BACKEND.parent / "frontend"

# Same as the top-level Makefile
DIRS = ["01-c", "02-cpp", "03-process", "04-signal", "05-thread", "06-ipc", "10-alp"]
HELPERS = {"01-c/04_helper.c"}
EXTRA_FILES = {"01-c/04_static_extern": ["01-c/04_helper.c"]}
READONLY = {
    "07-kernel-module/hello": ["07-kernel-module/hello.c", "07-kernel-module/Makefile", "07-kernel-module/README.md"],
}
SHELL_LAB = "08-shell/drills"
SHELL_ARGV = ["bash", "--norc", "--noprofile", "--noediting", "-i"]

PROGRAM_LIMIT = 300      # seconds, then the whole group is killed
SHELL_LIMIT = 3 * 3600
OUTPUT_LIMIT = 2_000_000  # characters kept per run
