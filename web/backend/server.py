#!/usr/bin/env python3
"""Backend cho web bosch-lab. Chỉ dùng thư viện chuẩn của Python 3.

    python3 web/backend/server.py [--port 8000]     (hoặc: make web / npm start)

Phục vụ frontend (web/frontend) và JSON API:
  GET  /api/labs                 danh sách bài + mã nguồn + đáp án
  GET  /api/theory               lý thuyết + câu trắc nghiệm
  POST /api/runs                 build & chạy một bài ({id}) hoặc mở bash ({terminal: true})
  GET  /api/runs/<id>?from=N     output từ chunk thứ N
  POST /api/runs/<id>/<action>   input | key | job | signal | ps | stop
Chỉ lắng nghe 127.0.0.1 và kiểm tra Host/Origin vì có endpoint mở bash.
"""
import argparse
import json
import os
import re
import sys
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import parse_qs, urlparse

from config import BACKEND, FRONTEND, ROOT, SHELL_ARGV, SHELL_LIMIT
from labs import discover, find_lab
from runner import KEYS, RUNS, RUNS_LOCK, SIGNALS, Run, gc_runs, kill_all_runs, start_run
from theory import parse_theory

STATIC_TYPES = {".html": "text/html; charset=utf-8", ".css": "text/css; charset=utf-8",
                ".js": "text/javascript; charset=utf-8", ".svg": "image/svg+xml", ".png": "image/png"}


class Handler(BaseHTTPRequestHandler):
    server_version = "bosch-lab"

    def log_message(self, fmt, *args):
        pass

    def allowed(self):
        port = self.server.server_address[1]
        ok_hosts = {f"127.0.0.1:{port}", f"localhost:{port}"}
        origin = self.headers.get("Origin")
        return self.headers.get("Host") in ok_hosts and (
            origin is None or origin.split("//", 1)[-1] in ok_hosts)

    def reply(self, obj, code=200):
        body = json.dumps(obj, ensure_ascii=False).encode()
        self.send_response(code)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        if not self.allowed():
            return self.reply({"error": "forbidden"}, 403)
        url = urlparse(self.path)
        q = parse_qs(url.query)
        if url.path == "/" or url.path.startswith(("/css/", "/js/")):
            return self.static(url.path)
        if url.path == "/api/labs":
            labs = discover()
            for lab in labs:
                lab["sources"] = {f: (ROOT / f).read_text(encoding="utf-8", errors="replace") for f in lab["files"]}
            return self.reply({"root": str(ROOT), "labs": labs})
        if url.path == "/api/theory":
            return self.reply({"topics": parse_theory()})
        m = re.fullmatch(r"/api/runs/(\w+)", url.path)
        if m and m.group(1) in RUNS:
            return self.reply(RUNS[m.group(1)].snapshot(int(q.get("from", ["0"])[0])))
        self.reply({"error": "not found"}, 404)

    def static(self, path):
        f = (FRONTEND / (path.lstrip("/") or "index.html")).resolve()
        if FRONTEND not in f.parents or not f.is_file() or f.suffix not in STATIC_TYPES:
            return self.reply({"error": "not found"}, 404)
        body = f.read_bytes()
        self.send_response(200)
        self.send_header("Content-Type", STATIC_TYPES[f.suffix])
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def do_POST(self):
        if not self.allowed() or not self.headers.get("Content-Type", "").startswith("application/json"):
            return self.reply({"error": "forbidden"}, 403)
        try:
            body = json.loads(self.rfile.read(int(self.headers.get("Content-Length", 0))) or b"{}")
        except ValueError:
            return self.reply({"error": "bad json"}, 400)
        path = urlparse(self.path).path

        if path == "/api/runs":
            gc_runs()
            if body.get("terminal"):
                run = Run(SHELL_ARGV, ROOT, SHELL_LIMIT, "terminal")
                log = ""
            else:
                lab = find_lab(body.get("id", ""))
                if not lab or lab["kind"] == "readonly":
                    return self.reply({"error": "bài không chạy được"}, 400)
                run, log = start_run(lab)
                if not run:
                    return self.reply({"ok": False, "build_log": log})
            with RUNS_LOCK:
                RUNS[run.id] = run
            return self.reply({"ok": True, "run": run.id, "build_log": log})

        m = re.fullmatch(r"/api/runs/(\w+)/(\w+)", path)
        run = RUNS.get(m.group(1)) if m else None
        if not run:
            return self.reply({"error": "not found"}, 404)
        action = m.group(2)
        if action == "input":
            run.write(body.get("text", "").encode())
        elif action == "key" and body.get("key") in KEYS:
            run.write(KEYS[body["key"]])
        elif action == "job" and body.get("cmd") in ("fg", "bg"):
            run.job(body["cmd"])
        elif action == "signal" and body.get("sig") in SIGNALS:
            err = run.send_signal(body["sig"], bool(body.get("group")))
            if err:
                return self.reply({"error": err}, 409)
        elif action == "ps":
            run.ps()
        elif action == "stop":
            run.kill_all()
        else:
            return self.reply({"error": "bad action"}, 400)
        self.reply({"ok": True})


def reload_on_change():
    """Code backend đổi → tự exec lại chính mình (frontend đọc lại mỗi request nên không cần)."""
    files = sorted(BACKEND.glob("*.py"))
    stamp = lambda: [f.stat().st_mtime for f in files]
    first = stamp()
    while True:
        time.sleep(1)
        try:
            if stamp() == first:
                continue
        except OSError:
            continue
        print("backend thay đổi → khởi động lại", flush=True)
        kill_all_runs()
        os.execv(sys.executable, [sys.executable] + sys.argv)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--port", type=int, default=8000)
    args = ap.parse_args()
    srv = ThreadingHTTPServer(("127.0.0.1", args.port), Handler)
    srv.daemon_threads = True
    print(f"bosch-lab web: http://localhost:{args.port}  (Ctrl+C để tắt)", flush=True)
    threading.Thread(target=reload_on_change, daemon=True).start()
    try:
        srv.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        kill_all_runs()


if __name__ == "__main__":
    main()
