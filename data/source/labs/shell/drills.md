# shell — 20 command drills

Type them for real in an Ubuntu terminal and write the commands and results in the blanks. The answers are in `ANSWERS.md` next to this file.

## Drill 01

Create a file `run.sh`, give it permissions `754`, and check with `ls -l` and `stat`. What permission string is shown? Who may do what?

> Hint: each octal digit = r(4) + w(2) + x(1).

```sh
# your commands:


# what you observed:

```

## Drill 02

Show the current `umask`. With `umask 022`, what permissions does a new file created by `touch` get, and a new directory created by `mkdir`?

```sh
# your commands:


# what you observed:

```

## Drill 03

Create `a.txt`, a hard link `hard.txt` and a soft link `soft.txt` to it. Delete `a.txt`. What does `cat` print for the two links? What does `ls -li` show about the inodes?

```sh
# your commands:


# what you observed:

```

## Drill 04

Use `ls /etc/hostname /does_not_exist` (it prints to both stdout and stderr). Compare `cmd > f 2>&1` with `cmd 2>&1 > f`: where does the error go?

```sh
# your commands:


# what you observed:

```

## Drill 05

Distinguish `>`, `>>`, `2>/dev/null`, `&>` and `tee`. Write the output of `date` to `log.txt` while still seeing it on screen, without overwriting the old content.

```sh
# your commands:


# what you observed:

```

## Drill 06

In the `bosch-lab` directory, find every line containing `fork` in the `.c` files, with line numbers. Then: count the files containing `pthread`, search case-insensitively, and print the lines of a file that do NOT contain `#include`.

```sh
# your commands:


# what you observed:

```

## Drill 07

Use `find`: find every `.cpp` file; find files larger than 1 MB in `/var/log`; find files modified in the last day; delete every `.o` file under `build/`.

```sh
# your commands:


# what you observed:

```

## Drill 08

Use `awk` to print field 1 (the user name) of `/etc/passwd`. Print the user name and shell (field 7) of users with UID ≥ 1000.

```sh
# your commands:


# what you observed:

```

## Drill 09

Count how many users use each shell (field 7 of `/etc/passwd`), sorted in descending order.

```sh
# your commands:


# what you observed:

```

## Drill 10

Count the lines, words and bytes of `/etc/passwd`. Count the `.c` files in `bosch-lab`.

```sh
# your commands:


# what you observed:

```

## Drill 11

Print the first 5 lines, the last 5 lines, and everything from line 2 onwards of `/etc/passwd`. Follow a log in real time.

```sh
# your commands:


# what you observed:

```

## Drill 12

Find the running `bash` processes with `ps aux | grep` and with `pgrep`. What do the `VSZ`, `RSS` and `STAT` columns of `ps aux` mean?

```sh
# your commands:


# what you observed:

```

## Drill 13

Open `top`: sort by memory, by CPU, show each core, quit. What do the 3 numbers of `load average` mean?

```sh
# your commands:


# what you observed:

```

## Drill 14

Run `sleep 100`, press `Ctrl+Z`, continue it in the background, list the jobs, bring it back to the foreground, then end it. Start a command directly in the background.

```sh
# your commands:


# what you observed:

```

## Drill 15

List the signal names. How do `kill PID`, `kill -9 PID`, `pkill` and `killall` differ?

```sh
# your commands:


# what you observed:

```

## Drill 16

Compare `df -h` and `du -sh`. Which directory under `/var` takes the most space?

```sh
# your commands:


# what you observed:

```

## Drill 17

Run `make run T=linux-process/05_zombie` in terminal 1. In terminal 2, look at the files that process has open using `lsof` and `/proc`. Find which process listens on port 22.

```sh
# your commands:


# what you observed:

```

## Drill 18

Use `strace` to look at the system calls of `01_fork_count`: get call counts, and show only `clone`/`write`. Use `ltrace` for library calls.

```sh
# your commands:


# what you observed:

```

## Drill 19

Print the exit code of the previous command, the PID of the current shell and the PID of the most recent background job. Add `~/bin` to PATH. How do shell variables and environment variables differ?

```sh
# your commands:


# what you observed:

```

## Drill 20

Watch the 4 compilation stages of `data/source/labs/c/08_macro_traps.c`. Look at the shared libraries the executable uses and the symbols in the `.o` file.

```sh
# your commands:


# what you observed:

```
