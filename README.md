<div id="top">

<!-- HEADER STYLE: CLASSIC -->
<div align="center">

<img src="Epita.png" width="30%" style="position: relative; top: 0; right: 0;" alt="Project Logo"/>


# 42SH

<!-- BADGES -->
<img src="https://img.shields.io/github/license/Luapsiaron/42sh?style=flat&logo=opensourceinitiative&logoColor=white&color=0080ff" alt="license">
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

42sh is a modular implementation of a Unix-like shell designed to interpret and execute command-line instructions.
The project focuses on building a robust and extensible architecture capable of handling command parsing, execution, and shell control flow.

Its design emphasizes maintainability and modularity, making it easier to understand, extend, and experiment with the internal components of a shell.

**Why 42sh?**

This project provides a solid foundation for understanding how modern command-line interpreters work and for building custom shell features.
Key aspects of the implementation include:

- **Modular architecture** – The shell is organized into separate components for lexing, parsing, AST construction, and command execution, improving maintainability and scalability.
- **Advanced command execution** – Support for pipelines, redirections, and shell functions allows the execution of complex command sequences.
- **Utility modules** – Helper components for string manipulation, input/output management, and other common operations simplify internal processing.
- **Control flow support** – Built-in handling of control structures such as loops and conditionals enables dynamic scripting capabilities.
- **Environment management** – Precise handling of the shell environment and internal state ensures consistent and predictable behavior.

---

## Features

|      | Component            | Details                                                                                                              |
| :--- | :------------------- | :------------------------------------------------------------------------------------------------------------------- |
| **Architecture**     | <ul><li>Modular design with separate components for lexer, parser, execution, builtins, and utils</li><li>Uses a layered approach separating parsing, execution, and expansion</li></ul> |
| **Code Quality**     | <ul><li>Consistent use of Makefile.am for build automation</li><li>Code organized into directories per functionality</li><li>Adheres to autotools conventions, facilitating maintainability</li></ul> |
| **Documentation**    | <ul><li>Basic README with project overview</li><li>Uses configure.ac and header files for API documentation</li><li>Limited inline comments observed in source files</li></ul> |
| **Integrations**     | <ul><li>Build system managed via autotools (`autoconf`, `automake`)</li><li>No external dependencies or package managers explicitly integrated</li></ul> |
| **Modularity**       | <ul><li>Source code split into multiple submodules (lexer, parser, execution, builtins)</li><li>Each module has dedicated Makefile.am for independent compilation</li></ul> |
| **Testing**          | <ul><li>No explicit test suite or framework detected in the codebase</li><li>Potential for manual testing or future integration</li></ul> |
| **Performance**      | <ul><li>Uses efficient parsing and execution flow</li><li>Minimal dynamic memory allocations observed</li></ul> |
| **Security**         | <ul><li>Basic input validation implied but no explicit security measures documented</li><li>Potential risks in command execution handling</li></ul> |
| **Dependencies**     | <ul><li>Primarily relies on standard C libraries</li><li>No external third-party dependencies specified</li></ul> |

---

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

## Getting Started

### Prerequisites

This project requires the following dependencies:

- **Programming Language:** C
- **Package Manager:** Autotools

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

3. **Install the dependencies:**

echo 'INSERT-INSTALL-COMMAND-HERE'

### Usage

Run the project with:

autoreconf -vfi
./configure
make

### Testing

BIN_PATH="$(pwd)/src/42sh" COVERAGE="yes" make check

---

## License

42sh is protected under the [LICENSE](https://choosealicense.com/licenses) License. For more details, refer to the [LICENSE](https://choosealicense.com/licenses/) file.

---

<div align="left"><a href="#top">⬆ Return</a></div>

---
