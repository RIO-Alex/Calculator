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

## PostgreSQL and verification

Install PostgreSQL server/client utilities, libpq development headers and Valgrind.
Connection settings use libpq environment variables: `PGHOST`, `PGPORT`,
`PGUSER`, `PGDATABASE`, and a password file (`~/.pgpass`) when needed.
Create a database owned by the application user:

```bash
createdb calculator_db
export PGDATABASE=calculator_db
./build/calculator '{"first":2,"second":3,"operation":"+"}'
psql -c 'TABLE calculations'
```

The table is created automatically; mathematical errors are stored too.
If initialization fails, the application logs an error and calculates without
persistent storage. On INSERT failure the result stays in memory; the failed
write is not automatically retried. Cache hits preserve the request's operands.

Run unit tests and real libpq operations under Valgrind:

```bash
cmake -S . -B build -DENABLE_POSTGRES_TESTS=ON
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

Integration tests require Python 3, `pg_config`, `initdb` and `pg_ctl`, and must
run as a non-root user. Each run creates and removes a temporary cluster with
a private Unix socket and TCP disabled. Existing databases are not modified.
Without the option, unit tests run independently of PostgreSQL. Valgrind checks
are registered when Valgrind is available.

For the psql video, run this disposable demonstration. It checks that swapped
addition does not add a row and displays a persisted mathematical error:

```bash
python3 tests/run_postgres.py "$(pg_config --bindir)" \
  bash tests/demo_database.sh "$PWD/build/calculator"
ctest --test-dir build -R memcheck -V
```

For a Perf recording use a build with symbols and an isolated database:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_POSTGRES_TESTS=ON
cmake --build build -j2
python3 tests/run_postgres.py "$(pg_config --bindir)" \
  perf record -g -o build/perf.data -- ./build/pq_database_tests --gtest_repeat=100
perf report -i build/perf.data --stdio
```

This profile includes connections, warm-up and queries, not just arithmetic.
Inspect their costs before drawing conclusions about caching. The environment
must allow perf events; a permission error is not profiling evidence. Record
the psql, Valgrind and Perf demonstrations, attach links to the homework PR,
and request review from the mentor.

All owned libpq resources (`PGconn`, `PGresult`) use RAII. Borrowed pointers
remain at C API boundaries (`argv`, libpq arguments and callbacks); giving them
owning smart pointers would be incorrect. No application worker threads are
created; logging uses synchronous single-threaded sinks.
