"""Chạy một lệnh như shell chạy foreground job trên terminal (pty).

    python3 jobwrap.py STATUS_FD CTL_FD prog [args...]

stdin/stdout/stderr phải là slave của pty. Wrapper làm session leader + nhận
pty làm controlling terminal, rồi fork con vào process group riêng và đưa nhóm
đó lên foreground — giống bash. Nhờ vậy:
  - Ctrl+C / Ctrl+Z / Ctrl+\\ (ghi \\x03 \\x1a \\x1c vào master) đi tới đúng job,
  - stdout của bài là line-buffered (vì là terminal),
  - khi job thoát, tiến trình con mồ côi KHÔNG bị SIGHUP (wrapper giữ session).

STATUS_FD nhận các dòng: "pid N", "stopped SIG", "continued", "exit C", "signal S".
CTL_FD nhận lệnh: "fg" (đưa job lên foreground + SIGCONT), "bg" (chỉ SIGCONT).
"""
import fcntl
import os
import select
import signal
import sys
import termios

JOB_SIGNALS = (signal.SIGTTOU, signal.SIGTTIN, signal.SIGTSTP, signal.SIGINT, signal.SIGQUIT)


def main():
    status_fd, ctl_fd, argv = int(sys.argv[1]), int(sys.argv[2]), sys.argv[3:]

    def report(msg):
        os.write(status_fd, (msg + "\n").encode())

    fcntl.ioctl(0, termios.TIOCSCTTY, 0)
    for s in JOB_SIGNALS:
        signal.signal(s, signal.SIG_IGN)

    pid = os.fork()
    if pid == 0:
        os.setpgid(0, 0)
        os.tcsetpgrp(0, os.getpid())
        for s in JOB_SIGNALS:
            signal.signal(s, signal.SIG_DFL)
        os.close(status_fd)
        os.close(ctl_fd)
        try:
            os.execvp(argv[0], argv)
        except OSError as e:
            os.write(2, f"exec {argv[0]}: {e.strerror}\n".encode())
            os._exit(127)

    # Gọi lại ở cha để không phụ thuộc thứ tự chạy cha/con
    try:
        os.setpgid(pid, pid)
        os.tcsetpgrp(0, pid)
    except OSError:
        pass
    report(f"pid {pid}")

    while True:
        ready, _, _ = select.select([ctl_fd], [], [], 0.05)
        if ready:
            for cmd in os.read(ctl_fd, 256).decode().split():
                try:
                    if cmd == "fg":
                        os.tcsetpgrp(0, pid)
                    os.killpg(pid, signal.SIGCONT)
                except OSError:
                    pass
        while True:
            try:
                wpid, st = os.waitpid(pid, os.WNOHANG | os.WUNTRACED | os.WCONTINUED)
            except ChildProcessError:
                return
            if wpid == 0:
                break
            if os.WIFSTOPPED(st):
                os.tcsetpgrp(0, os.getpgrp())
                report(f"stopped {os.WSTOPSIG(st)}")
            elif os.WIFCONTINUED(st):
                report("continued")
            else:
                report(f"exit {os.WEXITSTATUS(st)}" if os.WIFEXITED(st) else f"signal {os.WTERMSIG(st)}")
                # Lấy lại terminal rồi thoát: SIGHUP của session leader chỉ tới nhóm của wrapper
                try:
                    os.tcsetpgrp(0, os.getpgrp())
                except OSError:
                    pass
                return


if __name__ == "__main__":
    main()
