# Shell drills — answers

> Open this file only **after** you have predicted the output yourself. Labs whose output is **nondeterministic** are marked ⚠️.

### Drill 01

Create a file `run.sh`, give it permissions `754`, and check with `ls -l` and `stat`. What permission string is shown? Who may do what?

```sh
touch run.sh
chmod 754 run.sh
ls -l run.sh                 # -rwxr-xr-- 1 user user 0 ... run.sh
stat -c '%a %A %U' run.sh    # 754 -rwxr-xr-- user
```
7 = rwx (owner), 5 = r-x (group), 4 = r-- (others). r = 4, w = 2, x = 1.

### Drill 02

Show the current `umask`. With `umask 022`, what permissions does a new file created by `touch` get, and a new directory created by `mkdir`?

```sh
umask            # normal Ubuntu user: 0002; root: 0022
umask 022
touch f; mkdir d
ls -ld f d       # -rw-r--r-- f  (644)   drwxr-xr-x d  (755)
```
Files start from 666 and directories from 777, then the bits in the umask are **removed**: 666 & ~022 = 644, 777 & ~022 = 755.

### Drill 03

Create `a.txt`, a hard link `hard.txt` and a soft link `soft.txt` to it. Delete `a.txt`. What does `cat` print for the two links? What does `ls -li` show about the inodes?

```sh
echo hello > a.txt
ln a.txt hard.txt
ln -s a.txt soft.txt
ls -li            # a.txt and hard.txt have the SAME inode number, link count = 2; soft.txt has a different inode
rm a.txt
cat hard.txt      # hello  (the data survives because the inode still has 1 link)
cat soft.txt      # cat: soft.txt: No such file or directory  (a dangling symlink)
```
A hard link = another name for the same inode (not allowed for directories, cannot cross filesystems). A soft link = a file containing a path.

### Drill 04

Use `ls /etc/hostname /does_not_exist` (it prints to both stdout and stderr). Compare `cmd > f 2>&1` with `cmd 2>&1 > f`: where does the error go?

```sh
ls /etc/hostname /does_not_exist > f1 2>&1
cat f1            # both the error line and /etc/hostname are in f1; nothing on screen
ls /etc/hostname /does_not_exist 2>&1 > f2
                  # screen: ls: cannot access '/does_not_exist': No such file or directory
cat f2            # only /etc/hostname
```
Redirections are processed **left to right**. `2>&1` means "make fd 2 point where fd 1 points **right now**". In the second form, fd 1 is still the terminal at that moment.

### Drill 05

Distinguish `>`, `>>`, `2>/dev/null`, `&>` and `tee`. Write the output of `date` to `log.txt` while still seeing it on screen, without overwriting the old content.

```sh
date > log.txt        # overwrite
date >> log.txt       # append
find / -name passwd 2>/dev/null   # drop error messages
ls /x /etc &> all.txt # stdout + stderr into the same file (bash)
date | tee -a log.txt # show on screen and append to the file
```

### Drill 06

In the `bosch-lab` directory, find every line containing `fork` in the `.c` files, with line numbers. Then: count the files containing `pthread`, search case-insensitively, and print the lines of a file that do NOT contain `#include`.

```sh
grep -rn "fork" --include="*.c" .
grep -rl "pthread" . | wc -l
grep -rni "mutex" .
grep -v "#include" data/source/labs/linux-thread/02_mutex_fix.c
grep -c "printf" data/source/labs/c/05_bitwise.c      # number of matching LINES
```
`-r` recursive, `-n` line numbers, `-i` ignore case, `-v` invert, `-l` file names only, `-c` count, `-w` whole words, `-E` extended regex.

### Drill 07

Use `find`: find every `.cpp` file; find files larger than 1 MB in `/var/log`; find files modified in the last day; delete every `.o` file under `build/`.

```sh
find . -name "*.cpp"
sudo find /var/log -type f -size +1M
find . -type f -mtime -1
find build -name "*.o" -delete          # or: -exec rm {} \;
find . -name "*.c" -exec wc -l {} +     # line counts of every .c file
```

### Drill 08

Use `awk` to print field 1 (the user name) of `/etc/passwd`. Print the user name and shell (field 7) of users with UID ≥ 1000.

```sh
awk -F: '{print $1}' /etc/passwd
awk -F: '$3 >= 1000 {print $1, $7}' /etc/passwd
ps aux | awk '{print $2, $11}'      # PID and command
```
`-F:` sets the separator; `$0` is the whole line, `NR` the line number, `NF` the number of fields.

### Drill 09

Count how many users use each shell (field 7 of `/etc/passwd`), sorted in descending order.

```sh
cut -d: -f7 /etc/passwd | sort | uniq -c | sort -nr
#     20 /usr/sbin/nologin
#      3 /bin/bash
#      1 /bin/sync
```
`uniq` only merges **adjacent** lines → `sort` first. `sort -n` numeric, `-r` reverse, `-k2` by field 2, `-u` unique.

### Drill 10

Count the lines, words and bytes of `/etc/passwd`. Count the `.c` files in `bosch-lab`.

```sh
wc /etc/passwd          # lines  words  bytes  name
wc -l /etc/passwd
find . -name "*.c" | wc -l
ls | wc -l
```

### Drill 11

Print the first 5 lines, the last 5 lines, and everything from line 2 onwards of `/etc/passwd`. Follow a log in real time.

```sh
head -n 5 /etc/passwd
tail -n 5 /etc/passwd
tail -n +2 /etc/passwd          # skip the first line (a header)
sed -n '3,6p' /etc/passwd       # lines 3 to 6
sudo tail -f /var/log/syslog    # Ctrl+C to quit
```

### Drill 12

Find the running `bash` processes with `ps aux | grep` and with `pgrep`. What do the `VSZ`, `RSS` and `STAT` columns of `ps aux` mean?

```sh
ps aux | grep bash              # the "grep bash" line shows up too!
ps aux | grep [b]ash            # trick to exclude the grep itself
pgrep -a bash
ps -ef --forest                 # process tree (UID PID PPID ...)
ps -o pid,ppid,stat,ni,cmd -p $$
```
VSZ: virtual memory (KB). RSS: physical memory actually in RAM. STAT: R/S/D/T/Z, `+` foreground, `s` session leader, `<` high priority, `N` low priority.

### Drill 13

Open `top`: sort by memory, by CPU, show each core, quit. What do the 3 numbers of `load average` mean?

```sh
top            # M: by memory, P: by CPU, 1: per core, k: kill, q: quit
htop           # a nicer version (sudo apt install htop)
uptime         # load average: 0.52, 0.40, 0.35
nproc          # number of cores, to compare with the load
```
Load average = the average number of processes running or waiting for the CPU (and waiting for I/O in state D) over 1, 5 and 15 minutes.

### Drill 14

Run `sleep 100`, press `Ctrl+Z`, continue it in the background, list the jobs, bring it back to the foreground, then end it. Start a command directly in the background.

```sh
sleep 100
^Z                  # [1]+  Stopped   sleep 100   (sends SIGTSTP)
bg %1               # continue in the background (sends SIGCONT)
jobs -l             # [1]+ 12345 Running   sleep 100 &
fg %1               # back to the foreground
^C                  # SIGINT
sleep 200 &         # starts in the background; $! is its PID
kill %1             # kill by job number
nohup ./server &    # keeps running after the terminal closes (ignores SIGHUP)
```

### Drill 15

List the signal names. How do `kill PID`, `kill -9 PID`, `pkill` and `killall` differ?

```sh
kill -l                 # list of signals
kill 1234               # sends SIGTERM (15) - can be caught/ignored
kill -9 1234            # SIGKILL - cannot be caught
kill -SIGSTOP 1234; kill -CONT 1234
pkill -f "python app"   # by pattern (-f matches the whole command line)
killall sleep           # by exact name
```

### Drill 16

Compare `df -h` and `du -sh`. Which directory under `/var` takes the most space?

```sh
df -h                   # free space of each mounted FILESYSTEM
du -sh ~/bosch-lab      # total size of one DIRECTORY
sudo du -sh /var/* 2>/dev/null | sort -h | tail -3
df -i                   # free inodes
lsblk                   # disks / partitions
```
The two can disagree: a deleted file still held open by a process is counted by `df` but not by `du`.

### Drill 17

Run `make run T=linux-process/05_zombie` in terminal 1. In terminal 2, look at the files that process has open using `lsof` and `/proc`. Find which process listens on port 22.

```sh
pgrep 05_zombie
lsof -p <PID>               # cwd, txt (the program), mem (.so libraries), 0u 1u 2u (/dev/pts/N)
ls -l /proc/<PID>/fd        # 0 -> /dev/pts/1, 1 -> /dev/pts/1, 2 -> /dev/pts/1
cat /proc/<PID>/status | head
sudo lsof -i :22            # the process listening on port 22
ss -tlnp                    # listening TCP ports
```

### Drill 18

Use `strace` to look at the system calls of `01_fork_count`: get call counts, and show only `clone`/`write`. Use `ltrace` for library calls.

```sh
strace -c ./build/linux-process/01_fork_count          # syscall statistics table
strace -f -e trace=clone,write ./build/linux-process/01_fork_count
strace -p <PID>                                     # attach to a running process
ltrace ./build/c/07_signed_unsigned              # printf, strlen...
```
glibc implements `fork()` with the `clone` system call (you will see `clone(child_stack=NULL, flags=...SIGCHLD...)`). `-f` also follows child processes.

### Drill 19

Print the exit code of the previous command, the PID of the current shell and the PID of the most recent background job. Add `~/bin` to PATH. How do shell variables and environment variables differ?

```sh
ls /does_not_exist; echo $?   # 2 (non-zero = error)
true; echo $?               # 0
echo $$                     # PID of the shell
sleep 50 & echo $!          # PID of the background job just started
export PATH="$HOME/bin:$PATH"
X=1; bash -c 'echo "X=$X"'          # X=   (shell variables are not passed to child processes)
export X; bash -c 'echo "X=$X"'     # X=1
cmd1 && cmd2    # cmd2 runs if cmd1 succeeds;   cmd1 || cmd2  if it fails
```
In a script: `$0` the script name, `$1..$9` arguments, `$#` the argument count, `$@` all arguments.

### Drill 20

Watch the 4 compilation stages of `data/source/labs/c/08_macro_traps.c`. Look at the shared libraries the executable uses and the symbols in the `.o` file.

```sh
gcc -E data/source/labs/c/08_macro_traps.c | tail -20    # 1. preprocess: the macros are expanded
gcc -S data/source/labs/c/08_macro_traps.c -o m.s        # 2. compile to assembly
gcc -c data/source/labs/c/08_macro_traps.c -o m.o        # 3. assemble to an object file
gcc m.o -o m                               # 4. link
file m                 # ELF 64-bit LSB pie executable, x86-64, dynamically linked
ldd m                  # libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6
nm m.o                 # T main  (T: in text), U printf (U: undefined)
objdump -d m.o | head
```
Static libraries `.a` (copied into the executable at link time, `ar rcs`), shared libraries `.so` (loaded at run time, need `-fPIC -shared`, found via `LD_LIBRARY_PATH`/`ldconfig`).
