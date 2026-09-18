
# Unix Shell 

A small command-line shell written in C that demonstrates command parsing, process management, signal handling, and file redirection using POSIX APIs.

## Source code
Source code available upon request

## Features

- Execute external programs with arguments using `fork()` and `execvp()`.
- Wait for child processes with `waitpid()` and report termination by signals.
- Change directories with the built-in `cd` command.
- Exit with an optional status code using `exit`.
- Redirect standard input and output with `<` and `>`.
- Keep the shell running when Ctrl+C is pressed while allowing child programs to receive the interrupt.

## Requirements

- Linux, macOS, or Windows Subsystem for Linux (WSL)
- A C compiler such as GCC

## Build and Run

From the directory containing `myshell.c`:

```sh
gcc -Wall -Wextra -std=c11 myshell.c -o myshell
./myshell
```

## Usage

Enter commands at the `myshell>` prompt:

```text
myshell> pwd
myshell> ls -l
myshell> cd /tmp
myshell> echo hello > greeting.txt
myshell> cat < greeting.txt
myshell> exit 0
```

Separate redirection operators and filenames with spaces. Use `cd` with an explicit directory path. Output redirection overwrites the target file.

## Design

The shell reads an input line, splits it into whitespace-separated tokens, and checks for built-in commands. For external commands, it creates a child process, applies any redirection, and replaces the child with the requested program. The parent waits for the child before displaying the next prompt.

## Current Limitations

- No pipelines, background jobs, append redirection, or wildcard expansion.
- Quoted strings and escape sequences are not parsed.
- Input uses a fixed 300-byte buffer.
- End-of-file handling and error handling for built-in commands are limited; use `exit` to close the shell.

## Concepts Practiced

C strings and pointers, tokenization, POSIX process APIs, standard streams, and signal handling.
