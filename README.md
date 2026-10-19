# Calculator CLI

A simple C++ command-line calculator supporting basic arithmetic operations, exponentiation, and factorials.

---

## Supported Operations

| Operation | Description | Example |
| :---: | :--- | :--- |
| `+` | Addition | `{"first": 5, "second": 3, "operation": "+"}` |
| `-` | Subtraction | `{"first": 10, "second": 4, "operation": "-"}` |
| `x` | Multiplication | `{"first": 6, "second": 7, "operation": "x"}` |
| `/` | Division | `{"first": 20, "second": 4, "operation": "/"}` |
| `^` | Exponentiation | `{"first": 2, "second": 8, "operation": "^"}` |
| `!` | Factorial | `{"first": 5, "operation": "!"}` |

> **Note:** Multiplication uses the letter `x` to avoid shell wildcard expansion (globbing) issues with `*`.

---

## Prerequisites

* C++17 (or newer)
* CMake 3.22+
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
calculator '{"first": 5, "second": 3, "operation": "+"}'

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

# Display help
calculator --help
calculator -h

# Basic calculations using JSON input
calculator '{"first": 12, "second": 45, "operation": "+"}'
calculator '{"first": 542, "second": 83, "operation": "-"}'
calculator '{"first": 12, "second": 15, "operation": "x"}'
calculator '{"first": 100, "second": 3, "operation": "/"}'

# Exponentiation
calculator '{"first": 2, "second": 10, "operation": "^"}'

# Factorial
calculator '{"first": 5, "operation": "!"}'

# Read input from a JSON file
calculator input.json