<div id="top">

<!-- HEADER STYLE: CLASSIC -->
<div align="center">

<img src="Epita.png" width="30%" style="position: relative; top: 0; right: 0;" alt="Project Logo"/>


# 42SH

<em>Unix shell written in C — EPITA student project</em>

<!-- BADGES -->
<img src="https://img.shields.io/github/last-commit/Luapsiaron/42sh?style=flat&logo=git&logoColor=white&color=0080ff" alt="last-commit">
<img src="https://img.shields.io/github/languages/top/Luapsiaron/42sh?style=flat&color=0080ff" alt="repo-top-language">
<img src="https://img.shields.io/github/languages/count/Luapsiaron/42sh?style=flat&color=0080ff" alt="repo-language-count">

<em>Built with the tools and technologies:</em>

<img src="https://img.shields.io/badge/ReadMe-018EF5.svg?style=flat&logo=ReadMe&logoColor=white" alt="ReadMe">
<img src="https://img.shields.io/badge/C-A8B9CC.svg?style=flat&logo=C&logoColor=black" alt="C">

</div>
<br>

---

## Overview

42sh is a Unix-like shell written in C that we developed as part of a school project at EPITA.
The goal of the project was to understand how a real shell works internally by implementing everything ourselves: from command parsing to execution and shell control flow.

Instead of relying on existing parsing libraries or frameworks, the whole pipeline had to be implemented manually. This means handling lexing, building an AST, managing expansions, and executing commands — all in pure C.

Working on 42sh was a great way to explore what actually happens when you type a command in your terminal and press enter.


## Think for yourself

If you're an EPITA student working on a similar project:
feel free to read the code and take inspiration from it, but try to understand what is happening before copying anything.

The point of these projects is really to learn how things work under the hood and struggling with the implementation is part of the process.

So just don't copy/paste like an idiot, without thinkinh! Thanks!


## About the project

The project focuses on building a shell architecture that stays reasonably modular while still being written in low-level C.

The codebase is split into several components that reflect the typical lifecycle of a command:

- **Lexer** - transforms raw input into tokens
- **Parser** - builds the AST from the token stream
- **Execution** - walks the AST and executes commands
- **Expansion** - handles variable expansions and substitutions
- **Builtins** - implements shell built-in commands
- **Utilities and I/O helpers** - shared helpers used across modules

The idea was to keep the implementation understandable while still supporting features expected from a basic shell.

---

## Features

The shell currently supports a set of core features typically found in Unix shells:

- command execution
- pipelines (|)
- redirections (>, <, etc.)
- environment variable handling
- shell builtins
- control flow structures (loops and conditionals)

Internally, commands are parsed into an Abstract Syntax Tree (AST) before being executed, which makes it easier to handle more complex shell constructs.


## Architecture

The shell follows a classic pipeline architecture:

input → lexer → parser → AST → execution

Each stage is implemented in its own module, making the code easier to maintain and extend.


## Project Structure

```sh
└── 42sh/
    ├── Makefile.am
    ├── README
    ├── configure.ac
    └── src
        ├── 42sh.c
        ├── Makefile.am
        ├── ast
        ├── builtins
        ├── execution
        ├── expansion
        ├── io
        ├── lexer
        ├── parser
        └── utils
```

---

## Getting Started

### Installation

Build 42sh from the source and install dependencies:

1. **Clone the repository:**

    ```sh
    ❯ git clone https://github.com/Luapsiaron/42sh
    ```

2. **Navigate to the project directory:**

    ```sh
    ❯ cd 42sh
    ```


### Usage

Run the project with:

```sh
autoreconf -vfi
./configure
make
```

### Testing

```sh
BIN_PATH="$(pwd)/src/42sh" COVERAGE="yes" make check
```

---

## License

This project was developed as part of coursework at **EPITA**.

All work produced during coursework is technically the property of EPITA.  
However, this repository is shared publicly for educational and learning purposes, as no restriction was made against open-sourcing it.

If you are working on a similar project, please use this code responsibly.

---

<div align="left"><a href="#top">⬆ Return</a></div>

---
