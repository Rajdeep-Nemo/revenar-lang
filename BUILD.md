# Revenar Language Developer Guide

All terminal commands provided below are intended for the **Bash** shell.  
Users on Windows should use WSL (Windows Subsystem for Linux) or Git Bash.

---

## 1. Project Architecture

The Revenar interpreter follows a classic modular compiler architecture aimed at maintainability and strict separation of concerns.  
The codebase is written in C11 and relies on CMake for build orchestration.

### Directory Breakdown

The project is organized into discrete modules, each handling a specific stage of the execution pipeline:

```text
revenar-lang/
├── CMakeLists.txt           # Master build configuration
├── interpreter/             # Core implementation
│   ├── main.c               # Entry point
│   ├── common/              # Shared resources (Token definitions, Memory utils)
│   ├── lexer/               # Lexical Analysis (Source Code -> Tokens)
│   ├── parser/              # Semantic Analysis (Tokens -> AST)
│   └── evaluator/           # Runtime Engine (AST execution & Environment)
├── examples/                # Reference implementations (.rv files)
├── tests/                   # Unit testing suite (Optional build)
└── docs/                    # Technical specifications
```

---

## 2. Prerequisites

Before building, ensure the development environment has the necessary toolchain installed.

### Required Tools

- **C Compiler**: GCC or Clang (supporting C11 standard)  
- **Build System**: CMake 3.31+  
- **Version Control**: Git  

### Install Command (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install build-essential cmake git
```

---

## 3. Build Instructions

An "out-of-source" build workflow is used to keep the source directories clean.

### A. Standard Build (For Users)

This compiles the `revenar` executable optimized for release. It skips the testing suite to speed up compilation.

1. **Initialize the Build Directory**

```bash
mkdir build
cd build
```

2. **Configure the Project**

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

3. **Compile**
```bash
cmake --build .
```

4. **Run the Interpreter**

```bash
./interpreter/revenar
```

### B. Developer Build (For Contributors)

This configuration enables Unit Tests, Debug Symbols, and stricter compiler warnings.  
Use this mode when adding features or fixing bugs.

1. **Configure with Debugging and Tests Enabled**

```bash
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DREVENAR_BUILD_TESTS=ON ..
```

2. **Compile**
```bash
cmake --build .
```

3. **Run the Test Suite**

We use ctest (bundled with CMake) to validate the build.

```bash
ctest --output-on-failure
```

---

## 4. Module Dependency Graph

Understanding how the libraries link is crucial for avoiding circular dependencies:

- **Common**: No dependencies. Base definitions.  
- **Lexer**: Depends on `Common`.  
- **Parser**: Depends on `Lexer` and `Common`.  
- **Evaluator**: Depends on `Parser` (and transitively `Lexer`).  
- **Main**: Links all modules to produce the binary.

---

## 5. Adding a New Feature

When adding a new language feature (e.g., a "While Loop"):

1. **Common**: Add `TOKEN_WHILE` to `common/token.h`.  
2. **Lexer**: Update `lexer/lexer.c` to recognize the string `"while"`.  
3. **Parser**: Update `parser/ast.h` to define a `WhileNode` struct and `parser/parser.c` to parse the grammar.  
4. **Evaluator**: Update `evaluator/eval.c` to execute the loop logic.  
5. **Test**: Create a new test case in `tests/` to verify the behavior.
