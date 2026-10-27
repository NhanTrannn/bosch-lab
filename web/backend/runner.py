"""Build bài bằng Makefile ở gốc và chạy trong pty qua jobwrap.py (giống terminal thật)."""
import codecs
import fcntl
import os
import pty
import signal
import struct
import subprocess
import sys
import termios
import threading
import time
import uuid

from config import BACKEND, OUTPUT_LIMIT, PROGRAM_LIMIT, ROOT, SHELL_ARGV, SHELL_LIMIT

SIGNALS = {name: getattr(signal, "SIG" + name) for name in
           ["INT", "QUIT", "TERM", "KILL", "USR1", "USR2", "STOP", "CONT", "TSTP", "HUP"]}
KEYS = {"c": b"\x03", "z": b"\x1a", "\\": b"\x1c", "d": b"\x04"}

BUILD_LOCK = threading.Lock()


def build(lab_id):
    # -B: luôn biên dịch lại để thấy cảnh báo và nhận thay đổi vừa sửa trong editor
    with BUILD_LOCK:
        p = subprocess.run(["make", "--no-print-directory", "-B", f"build/{lab_id}"],
                           cwd=ROOT, capture_output=True, text=True, timeout=120)
    return p.returncode == 0, (p.stdout + p.stderr).strip()


class Run:
    def __init__(self, argv, cwd, limit, label):
        self.id = uuid.uuid4().hex[:12]
        self.label = label
        self.chunks = []          # [loại, text]: "out" = từ terminal, "sys" = ghi chú của web
        self.size = 0
        self.lock = threading.Lock()
        self.pid = None           # PID của job (chính chương trình bài)
        self.state = "running"    # running | stopped | exited | done
        self.result = None
        self.done = False
        self.finished_at = None

        master, slave = pty.openpty()
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 30, 120, 0, 0))
        status_r, status_w = os.pipe()
        ctl_r, ctl_w = os.pipe()
        env = dict(os.environ, TERM="dumb", PS1=r"\w\$ ")
        self.proc = subprocess.Popen(
            [sys.executable, "-I", str(BACKEND / "jobwrap.py"), str(status_w), str(ctl_r), *argv],
            stdin=slave, stdout=slave, stderr=slave, cwd=cwd, env=env,
            pass_fds=(status_w, ctl_r), start_new_session=True)
        for fd in (slave, status_w, ctl_r):
            os.close(fd)
        self.master, self.ctl = master, ctl_w
        self.sid = self.proc.pid  # wrapper là session leader → SID = PID của nó

        threading.Thread(target=self._read_status, args=(status_r,), daemon=True).start()
        threading.Thread(target=self._read_output, daemon=True).start()
        self.timer = threading.Timer(limit, self._timeout, args=(limit,))
        self.timer.daemon = True
        self.timer.start()

    def add(self, kind, text):
        with self.lock:
            if self.size > OUTPUT_LIMIT:
                return
            self.size += len(text)
            self.chunks.append([kind, text])
            if self.size > OUTPUT_LIMIT:
                self.chunks.append(["sys", "\n[output quá dài, bỏ phần sau]\n"])

    def _read_status(self, fd):
        names = {s.value: s.name for s in signal.Signals}
        with os.fdopen(fd) as f:
            for line in f:
                what, _, arg = line.strip().partition(" ")
                if what == "pid":
                    self.pid = int(arg)
                elif what == "stopped":
                    self.state = "stopped"
                    self.add("sys", f"\n[{self.pid}] Stopped ({names.get(int(arg), arg)}) — bấm fg để chạy tiếp\n")
                elif what == "continued":
                    self.state = "running"
                    self.add("sys", f"\n[{self.pid}] Continued\n")
                elif what in ("exit", "signal"):
                    self.state = "exited"
                    self.result = {"kind": what, "value": int(arg),
                                   "name": names.get(int(arg)) if what == "signal" else None}
                    msg = f"mã thoát {arg}" if what == "exit" else f"bị kết thúc bởi {names.get(int(arg), arg)}"
                    self.add("sys", f"\n[tiến trình {self.pid} kết thúc, {msg}]\n")
                    threading.Timer(0.5, self._note_orphans).start()

    def _note_orphans(self):
        if not self.done:
            self.add("sys", "[vẫn còn tiến trình con giữ terminal (orphan/chạy nền) — chờ chúng thoát hoặc bấm Dừng]\n")

    def _read_output(self):
        dec = codecs.getincrementaldecoder("utf-8")(errors="replace")
        while True:
            try:
                data = os.read(self.master, 65536)
            except OSError:   # EIO: không còn tiến trình nào giữ slave
                data = b""
            if not data:
                break
            self.add("out", dec.decode(data))
        self.proc.wait()
        self.timer.cancel()
        for fd in (self.master, self.ctl):
            try:
                os.close(fd)
            except OSError:
                pass
        self.state, self.done, self.finished_at = "done", True, time.time()

    def _timeout(self, limit):
        if not self.done:
            self.add("sys", f"\n[quá {limit} giây, kill toàn bộ]\n")
            self.kill_all()

    # ---- điều khiển từ frontend
    def write(self, data):
        if not self.done:
            try:
                os.write(self.master, data)
            except OSError:
                pass

    def job(self, cmd):
        try:
            os.write(self.ctl, (cmd + "\n").encode())
        except OSError:
            pass

    def send_signal(self, sig, group):
        if not self.pid or self.state in ("exited", "done"):
            return "tiến trình đã kết thúc"
        try:
            (os.killpg if group else os.kill)(self.pid, SIGNALS[sig])
        except ProcessLookupError:
            return "không tìm thấy tiến trình"
        self.add("sys", f"\n$ kill -{sig} {'-' if group else ''}{self.pid}\n")
        return None

    def kill_all(self):
        for pg in (self.pid, self.sid):
            if pg:
                try:
                    os.killpg(pg, signal.SIGKILL)
                except (ProcessLookupError, PermissionError):
                    pass

    def ps(self):
        p = subprocess.run(["ps", "-o", "pid,ppid,pgid,stat,cmd", "-s", str(self.sid)],
                           capture_output=True, text=True)
        lines = [ln for ln in p.stdout.rstrip().split("\n") if "jobwrap.py" not in ln]
        self.add("sys", "\n$ ps -o pid,ppid,pgid,stat,cmd -s <session>\n" + "\n".join(lines) + "\n")

    def snapshot(self, start):
        with self.lock:
            chunks = self.chunks[start:]
            n = len(self.chunks)
        return {"chunks": chunks, "next": n, "pid": self.pid, "sid": self.sid,
                "state": self.state, "done": self.done, "result": self.result}


RUNS = {}
RUNS_LOCK = threading.Lock()


def start_run(lab):
    if lab["kind"] == "shell":
        cwd = ROOT / "build" / "shell"
        cwd.mkdir(parents=True, exist_ok=True)
        return Run(SHELL_ARGV, cwd, SHELL_LIMIT, lab["id"]), ""
    ok, log = build(lab["id"])
    if not ok:
        return None, log
    return Run([str(ROOT / "build" / lab["id"])], ROOT, PROGRAM_LIMIT, lab["id"]), log


def gc_runs():
    now = time.time()
    with RUNS_LOCK:
        for rid in [r for r, run in RUNS.items() if run.done and now - run.finished_at > 1800]:
            del RUNS[rid]


def kill_all_runs():
    for run in list(RUNS.values()):
        run.kill_all()
