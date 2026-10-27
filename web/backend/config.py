"""Đường dẫn và hằng số dùng chung của backend."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]   # thư mục bosch-lab
BACKEND = Path(__file__).resolve().parent
FRONTEND = BACKEND.parent / "frontend"

# Giống Makefile ở gốc
DIRS = ["01-c", "02-cpp", "03-process", "04-signal", "05-thread", "06-ipc"]
HELPERS = {"01-c/04_helper.c"}
EXTRA_FILES = {"01-c/04_static_extern": ["01-c/04_helper.c"]}
READONLY = {
    "07-kernel-module/hello": ["07-kernel-module/hello.c", "07-kernel-module/Makefile", "07-kernel-module/README.md"],
}
SHELL_LAB = "08-shell/drills"
SHELL_ARGV = ["bash", "--norc", "--noprofile", "--noediting", "-i"]

PROGRAM_LIMIT = 300      # giây, sau đó kill cả nhóm
SHELL_LIMIT = 3 * 3600
OUTPUT_LIMIT = 2_000_000  # ký tự giữ lại cho mỗi lần chạy
