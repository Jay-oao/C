# Custom Unix Shell <0Sk>

A simple wrapper around existing system commands designed to help understand the basics of process management, I/O control, and signal handling.

---

## Requirements

- GCC compiler  
- Unix-like operating system (Linux, macOS)  

---

## Installation

1. Clone the repository.  
2. Run `make clean` to build the executable named `shell`.  
3. Run `./shell` to start the shell.

---

## Features

1. Basic tokenizer and parser for command input.  
2. I/O control using raw mode, including custom colors for enhanced terminal interaction.  
3. Support for command piping using `|`.  
4. Supports input/output redirection with `>`, `<`, and `>>`.  
5. Maintains command history supported by a Doubly Linked List data structure.  
6. Autocomplete functionality implemented using Trie data structures.  
7. Multi-command logical chaining supported through Abstract Syntax Trees (ASTs).  
8. Proper handling of signals including `SIGINT`, `SIGTSTP`, `SIGTERM`, and `SIGCHLD`.  

---

## Usage

1. Run unix commands upon startup.
2. Use `CTRL+T` or type `exit` to issue a `SIGTERM`.
