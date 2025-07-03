# my_shell - Simple xv6 Shell

This repository contains the implementation of a simple shell for the xv6 operating system, developed as a user-space program. This project demonstrates an understanding of process creation, file descriptors, pipes, and I/O redirection within a minimalistic operating system environment.

The shell supports fundamental command execution, I/O redirection, piping, and sequential command execution.

## Coursework Details

* **Module:** COMP2211
* **Environment:** xv6 operating system. Via QEMU.

## Features Implemented

The `my_shell` program provides the following functionalities:

### Part 1 & 2: Basic Command Execution

* **Command Prompt:** Displays `>>>` as a command prompt.
* **Simple Command Execution:** Executes single commands (e.g., `echo hello world`, `ls`).
* **Indefinite Loop:** Continues prompting and executing commands until explicitly exited (e.g., by typing `exit` if xv6 supports it, or `Ctrl+C`).
* **`cd` Command Handling:** Special handling for the `cd` (change directory) command, as it must be executed within the shell's process.
* **Robust Parsing:** Handles commands with varying amounts and locations of whitespace (e.g., `  echo   hello world`).

### Part 3: Input/Output Redirection

* **Output Redirection (`>`):** Redirects command output to a specified file (e.g., `echo "Hello world" > temp`).
* **Input Redirection (`<`):** Redirects command input from a specified file (e.g., `cat < temp`).

### Part 4 & 5: Pipes and Advanced Features

* **Two-Element Pipes (`|`):** Supports piping the output of one command as input to another (e.g., `cat README | grep github`).
* **Multi-Element Pipes:** Extends pipe functionality to multiple commands (e.g., `ls | grep test | cat`).
* **Non-trivial Combinations of Pipes and Redirection:** Handles complex command chains involving both pipes and I/O redirection (e.g., `ls | grep test | cat > myoutput`).
* **Sequential Command Execution (`;`):** Allows multiple commands to be executed sequentially (e.g., `ls | grep test | cat > myoutput; cat myoutput`).

## Project Structure

* `my_shell.c`: The core source code for the shell implementation.
* `Makefile`: Modified to include `my_shell.c` in the xv6 build process.
* `kernel/`, `user/`, `kernel/stat.h`, `user/user.h`, `kernel/fcntl.h`: Standard xv6 directories and header files used by the shell.

## Build and Run Instructions (within xv6)

1.  **Place `my_shell.c`:** Ensure `my_shell.c` is placed in the `user/` directory of your xv6 source code.
2.  **Modify `Makefile`:** Add `my_shell` to the `UPROGS` variable in the `Makefile` located in the root of your xv6 directory.
    * Find the line similar to `UPROGS=\`
    * Add `_my_shell\` to the list. For example:
        ```makefile
        UPROGS=\
             _cat\
             _echo\
             _forktest\
             _grep\
             _init\
             _kill\
             _ln\
             _ls\
             _mkdir\
             _my_shell\  # Add this line
             _rm\
             _sh\
             _stressfs\
             _usertests\
             _grind\
             _wc\
             _zombie\
        ```
3.  **Build xv6:** Navigate to the root of your xv6 directory in a terminal and run `make qemu`.
    ```bash
    cd /path/to/your/xv6
    make qemu
    ```
4.  **Execute the Shell:** Once QEMU starts and you see the xv6 shell prompt, type `my_shell` to run your custom shell.
    ```
    init: starting sh
    $ my_shell
    >>>
    ```
    You should now see the `>>>` prompt, indicating your shell is running.

## Testing

The shell has been tested against various scenarios, including:

* Basic `cd` commands (relative paths, multiple spaces).
* Simple `echo` and `cat` commands with varied spacing and special characters.
* I/O redirection (`>`, `<`).
* Single and multi-stage pipes (`|`).
* Combinations of pipes and redirection.
* Sequential command execution (`;`).

A list of successfully passed tests is provided in the coursework brief:
```
Passed Tests
Test 'cd .' and 'cd tempdir' (0.5/0.5)
Test 'cd dir1/dir2' and 'cd ..' (0.5/0.5)
'cd .' (with 5 spaces before cd) (0.25/0.25)
'cd dir1/dir2' and 'cd ..' (with 3 or 10 spaces after cd) (0.25/0.25)
'echo TeStInGtHiSsHeLl1@-+=~' (echo text with numbers and special characters) (0.5/0.5)
'echo AbitraryNumberOfSpaces' (with 3 space character before 'echo') (0.25/0.25)
'echo 21_spaces' (with 21 space characters between 'echo' and '21_spaces') (0.25/0.25)
'echo String1 String2 String3 String4' (0.25/0.25)
echo String1 String2 String3 String4 (with varied number of spaces between strings) (0.25/0.25)
Check that nothing breaks when entering 'cat README' command 8 times (0.5/0.5)
Test 'ls' and '../ls' by creating a directory, entering it with 'cd' and listing the contents (0.5/0.5)
Test 'ls' and '../ls' (with one space at the front) (0.25/0.25)
Test 'ls' and '../ls' (with various number of spaces at the front) (0.25/0.25)
Check prompt >>> is shown (0.5/0.5)
'echo SomeText Here > file2' followed by 'cat < file2' (3/3)
'echo Text With Spaces Numbers975 and Special chars@£ > file1' followed by 'cat file1' (3/3)
ls | grep stres (2.5/2.5)
cat README | grep xv6 (2.5/2.5)
Tricky case (no spacing around the pipe): 'cat README|grep xv6' (1/1)
ls | grep test | cat > file4.txt; cat file4.txt (1/1)
cat < README > file2 (1/1)
cat README | grep the | grep Version (1/1)
Tricky case (no spacing around pipes): 'cat README|grep the|grep Version' (1/1)
cat README | grep the | grep Version > file1 (2/2)
cat < README | grep the | grep Version > file3 (1/1)
```
