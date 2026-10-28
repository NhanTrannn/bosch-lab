# Linux: commands, permissions and the filesystem

## Theory

### Directory tree (FHS)
`/bin`, `/usr/bin` user commands · `/sbin` admin commands · `/etc` configuration · `/home` user home directories · `/root` root's home · `/var` changing data (logs: `/var/log`) · `/tmp` temporary · `/dev` device files · `/proc` process/kernel information (virtual) · `/sys` devices and drivers (sysfs, virtual) · `/boot` kernel, initramfs · `/lib` libraries, kernel modules (`/lib/modules`) · `/opt` add-on software · `/mnt`, `/media` mount points.

### Permissions
- `-rwxr-xr--`: the first character is the file type (`-` regular, `d` directory, `l` symlink, `c` char device, `b` block device, `p` FIFO, `s` socket), then 3 groups owner/group/others.
- r = 4, w = 2, x = 1 → `chmod 754 f` = rwxr-xr--. `chmod u+x,g-w f`. `chown user:group f`.
- On a directory: r = list names, w = create/delete files in it, x = enter it (cd) and access files inside.
- `umask 022` → new files get 644 (666 & ~022), new directories 755.
- Special bits: **setuid** (4xxx, runs with the owner's privileges, e.g. `passwd`), **setgid** (2xxx), **sticky** (1xxx, on `/tmp`: only the owner can delete their own files).
- Everything is a file; permissions are checked against the process's UID/GID.

### Inodes and links
- An **inode** stores metadata (permissions, owner, size, timestamps, pointers to data blocks) — **not** the name. File names live in directories (name → inode mappings).
- **Hard link** (`ln a b`): another name for the same inode, increases the link count; deleting one name keeps the data. Not allowed for directories and cannot cross filesystems.
- **Soft/symbolic link** (`ln -s a b`): a separate file containing a path; deleting the target → a broken (dangling) link. Can cross filesystems and point to directories.
- `ls -li` shows inodes, `stat f` shows details, `df -i` shows remaining inodes.

### File descriptors and redirection
- 0 stdin, 1 stdout, 2 stderr. `>` overwrite, `>>` append, `<` read, `2>` stderr, `2>&1` stderr follows stdout, `&>` both.
- Order matters: `cmd > f 2>&1` (both into f) differs from `cmd 2>&1 > f` (stderr goes to the terminal).
- `|` connects the stdout of one command to the stdin of the next. `tee` writes to a file and prints.
- `/dev/null` discards output. `$?` is the exit code of the last command (0 = success). `&&` runs the next command on success, `||` on failure.

### Common commands
- Searching: `find / -name "*.c" -size +1M -mtime -7`, `grep -rn "pattern" dir` (`-i` case-insensitive, `-v` invert, `-c` count, `-l` file names only, `-E` extended regex), `which`, `locate`.
- Text processing: `cat`, `head -n`, `tail -n`/`tail -f`, `wc -l`, `sort`, `uniq -c` (needs sort first), `cut -d: -f1`, `awk -F: '{print $1}'`, `sed 's/a/b/g'`, `tr`, `diff`, `less`.
- Processes: `ps`, `top`, `kill`, `pkill`, `pgrep`, `jobs`, `fg`, `bg`, `nohup`, `&`.
- System: `df -h` (filesystem usage), `du -sh` (directory size), `free -h`, `uname -r`, `lsblk`, `mount`, `dmesg`, `journalctl`, `systemctl status|start|enable svc`, `lsof` (open files), `ss -tlnp`/`netstat` (listening ports), `ip addr`.
- Archives: `tar -czvf a.tar.gz dir` (create), `tar -xzvf a.tar.gz` (extract).
- Packages (Ubuntu): `apt install/update/upgrade`, `dpkg -l`.
- Environment: `export VAR=x`, `echo $PATH`, `env`. Scripts start with a shebang `#!/bin/bash`; `$0` script name, `$1..` arguments, `$#` argument count, `$@` all arguments, `$$` PID.

### Boot sequence (x86 Linux)
BIOS/UEFI → bootloader (GRUB) → loads kernel + initramfs → the kernel initializes and mounts the root filesystem → runs **init** (PID 1, usually systemd) → services/targets → login.
On embedded systems: ROM bootloader → U-Boot → kernel + device tree → rootfs → init.

## Quiz

### What permissions does `chmod 640 file` give?
- [ ] rwxr-----
- [x] rw-r-----
- [ ] rw-rw----
- [ ] r--r-----
> 6 = rw- (owner), 4 = r-- (group), 0 = --- (others).

### With `umask 022`, what permissions does a file created by `touch` get?
- [ ] 755
- [x] 644
- [ ] 666
- [ ] 600
> The base permission for files is 666, minus the umask bits → 644. Directories: 777 → 755.

### What does the x permission on a directory allow?
- [ ] Listing the file names in it
- [x] cd into it and access files inside by name
- [ ] Running every file in it
- [ ] Deleting the directory
> r allows listing names, w allows creating/deleting files (together with x).

### Which statements about hard links are true? (select all that apply)
- [x] It points to the same inode as the original file
- [x] After deleting the original, the data is still reachable through the hard link
- [ ] You can hard-link a directory with ln
- [ ] It can point to another filesystem
> Data is only removed when the link count reaches 0 and no process has the file open.

### Which information is NOT stored in an inode?
- [ ] Permissions
- [ ] File size
- [x] File name
- [ ] Modification time
> The name lives in a directory entry. That is why one inode can have several names (hard links).

### Which command sends both stdout and stderr to out.txt?
- [ ] `cmd 2>&1 > out.txt`
- [x] `cmd > out.txt 2>&1`
- [ ] `cmd > out.txt 1>&2`
- [ ] `cmd 2> out.txt`
> Redirections are processed left to right: first stdout → file, then stderr → wherever stdout points (the file). The other way round, stderr points to the old terminal.

### What does a leading `c` in `ls -l` (e.g. `crw-rw----`) mean?
- [ ] A configuration directory
- [x] A character device
- [ ] A compressed file
- [ ] A socket
> b is a block device (disk), p a FIFO, s a socket, l a symlink, d a directory.

### Which command counts the lines containing "error" (case-insensitive) in log.txt?
- [ ] `grep -v error log.txt | wc -l`
- [x] `grep -ci error log.txt`
- [ ] `grep -l error log.txt`
- [ ] `wc -l error log.txt`
> -c counts matching lines, -i ignores case.

### Which command prints the first field (the user name) of /etc/passwd?
- [x] `cut -d: -f1 /etc/passwd`
- [ ] `cut -f1 /etc/passwd`
- [ ] `awk '{print $1}' /etc/passwd`
- [ ] `sed -n 1p /etc/passwd`
> Fields are separated by `:`. `awk -F: '{print $1}' /etc/passwd` also works.

### Which command finds every .log file larger than 100 MB under /var?
- [ ] `grep -r "*.log" /var --size 100M`
- [x] `find /var -name "*.log" -size +100M`
- [ ] `ls -R /var | grep log`
- [ ] `locate /var/*.log > 100M`
> find supports -name, -type f/d, -size, -mtime, -user, -exec.

### What does `$?` contain in bash?
- [ ] The shell's PID
- [x] The exit code of the last command
- [ ] The number of arguments passed to the script
- [ ] The PID of the most recent background job
> 0 = success. `$$` is the shell's PID, `$#` the argument count, `$!` the PID of the last background process.

### What does the sticky bit on /tmp do?
- [ ] Files in /tmp survive a reboot
- [x] Only a file's owner (or root) can delete/rename it, even though everyone can write to the directory
- [ ] Files run with their owner's privileges
- [ ] The directory becomes read-only
> It shows as `t` in the last position: `drwxrwxrwt`.

### Why does `/usr/bin/passwd` have the setuid bit?
- [ ] To stop normal users from running passwd
- [x] So the program runs with its owner's (root's) privileges and can modify /etc/shadow
- [ ] So everyone can read /etc/shadow
- [ ] To log every run
> It shows as `s` in place of the owner's x: `-rwsr-xr-x`.

### Which command shows the free space on each filesystem?
- [x] df -h
- [ ] du -sh
- [ ] free -h
- [ ] lsblk -f
> du measures space used by directories/files, free measures RAM.

### Which command follows a log file while it is being appended to?
- [ ] cat -f
- [ ] head -f
- [x] tail -f
- [ ] less -n
> `less +F` works too. `journalctl -f` for systemd logs.

### On most current Linux distributions, the first user-space process (PID 1) is?
- [ ] the kernel
- [x] systemd (init)
- [ ] bash
- [ ] grub
> The bootloader (GRUB/U-Boot) loads the kernel, and the kernel starts init.

### Which command shows listening TCP ports with their processes?
- [ ] ps -tcp
- [x] ss -tlnp
- [ ] ip route
- [ ] ping -l
> Or netstat -tlnp (older). -t TCP, -l listening, -n numeric, -p process.

### Which directory exposes devices and drivers as a virtual filesystem?
- [ ] /dev
- [x] /sys
- [ ] /etc
- [ ] /boot
> /sys (sysfs) mirrors the kernel's device model. /dev holds device files for access, /proc holds process information.

### What does `sort data.txt | uniq -c | sort -rn | head -3` do?
- [ ] Deletes the first 3 duplicate lines
- [x] Prints the 3 most frequent lines with their counts
- [ ] Prints the first 3 lines after sorting
- [ ] Counts the lines in the file
> uniq only merges **adjacent** duplicates, so you must sort first; sort -rn sorts numbers in descending order.
