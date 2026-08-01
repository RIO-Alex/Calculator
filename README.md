# Calculator CLI

A simple C++ command-line calculator supporting basic arithmetic operations, exponentiation, and factorials.

---

## Supported Operations

| Operation | Description | Example |
| :---: | :--- | :--- |
| `+` | Addition | `calculator 5 + 3` |
| `-` | Subtraction | `calculator 10 - 4` |
| `x` | Multiplication | `calculator 6 x 7` |
| `/` | Division | `calculator 20 / 4` |
| `^` | Exponentiation | `calculator 2 ^ 8` |
| `!` | Factorial | `calculator 5!` or `calculator 5 !` |

> **Note:** Multiplication uses the letter `x` to avoid shell wildcard expansion (globbing) issues with `*`.

---

## Prerequisites

* C++17 (or newer)
* CMake 3.20+
* C++ compiler (`g++` or `clang++`)
* Build system (`make` / `ninja`)

---

## Build and Installation

### 1. Building the Project

Use the standard CMake workflow to build the project:

```bash
# Configure the project
cmake -B build

# Compile
cmake --build build

```

The executable will be located at `./build/calculator`.

---

### 2. Installing System-Wide (Optional)

Installing allows you to run the binary from any directory:

```bash
# System-wide installation (defaults to /usr/local/bin)
sudo cmake --build build --target install

# Run from anywhere
calculator 5 x 5

```

To install into a user directory without requiring `sudo`:

```bash
# Configure with a custom prefix
cmake -B build -DCMAKE_INSTALL_PREFIX=$HOME/.local

# Install
cmake --build build --target install

```

---

## Examples

```bash
# Display help
calculator --help
calculator -h

# Basic calculations
calculator 12 + 45
calculator 542 - 83
calculator 12 x 15
calculator 100 / 3

# Exponentiation
calculator 2 ^ 10

# Factorial
calculator 5!
calculator 5 !
```