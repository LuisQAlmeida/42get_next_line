# get_next_line

[![CI](https://github.com/LuisQAlmeida/42get_next_line/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/LuisQAlmeida/42get_next_line/actions/workflows/ci.yml)

> Part of my [42 Common Core portfolio](https://github.com/LuisQAlmeida/42Portfolio).

A C implementation of the mandatory `get_next_line` project from the 42
curriculum.

The public function reads from a file descriptor and returns one dynamically
allocated line per call, preserving unread bytes between consecutive calls.

```c
char *get_next_line(int fd);
```

The maintained portfolio version focuses on the mandatory single-descriptor
implementation and is backed by automated regression testing, GCC and Clang
continuous integration, Valgrind memory validation, and reproducible
repository-controlled tooling.

---

## Overview

`get_next_line()` provides a line-oriented interface on top of the lower-level
`read()` system call.

Each successful call returns the next available line from the supplied file
descriptor.

When a newline exists, the returned allocation includes the terminating
newline character.

For example, an input file containing:

```text
alpha
beta
gamma
```

is returned over successive calls conceptually as:

```text
"alpha\n"
"beta\n"
"gamma\n"
NULL
```

If the final line does not end with `\n`, that final line is still returned
before EOF.

The caller owns every non-NULL returned pointer and must release it with
`free()`.

---

## Maintained Scope

This repository maintains the **mandatory single-file-descriptor version** of
the 42 `get_next_line` project.

The implementation contains one persistent buffer:

```c
static char buffer[BUFFER_SIZE + 1];
```

That buffer preserves unread bytes between consecutive calls.

Because there is only one persistent buffer, the maintained implementation does
not preserve independent buffered state for file descriptors read in an
interleaved pattern.

For example, code that alternates reads between two descriptors:

```text
fd A
fd B
fd A
fd B
```

must not be assumed to behave like the historical bonus implementation.

Multi-file-descriptor bonus support is intentionally outside the maintained
scope of this repository.

---

## Function Contract

```c
char *get_next_line(int fd);
```

### Parameters

`fd`

: File descriptor from which data is read.

### Return value

A non-NULL pointer

: A newly allocated string containing the next line.

`NULL`

: EOF with no remaining line, an invalid descriptor, a read failure, an
  allocation failure, or an invalid `BUFFER_SIZE`.

### Ownership

Every non-NULL line returned by `get_next_line()` is dynamically allocated.

The caller is responsible for:

```c
free(line);
```

after the line is no longer needed.

---

## Behaviour

The maintained implementation and regression suite cover the following
behaviour.

| Case | Maintained behaviour |
|---|---|
| Empty file | Returns `NULL` |
| One byte without newline | Returns that byte as the final line |
| Newline-only file | Returns `"\n"` |
| Multiple lines | Returns one line per call |
| Final line without newline | Returns the final line before EOF |
| Consecutive empty lines | Returns each newline-delimited empty line independently |
| Line shorter than `BUFFER_SIZE` | Returned normally |
| Line exactly `BUFFER_SIZE` bytes | Returned normally |
| Line larger than `BUFFER_SIZE` | Built across multiple reads |
| Very long line | Built across repeated reads |
| Invalid negative descriptor | Returns `NULL` |
| Descriptor causing `read()` failure | Returns `NULL` |
| Calls after EOF | Continue to return `NULL` |

---

## BUFFER_SIZE

Input is read in chunks controlled by the compile-time `BUFFER_SIZE` macro.

The maintained header provides a default:

```c
#ifndef BUFFER_SIZE
# define BUFFER_SIZE 10
#endif
```

A different value can be supplied during compilation:

```sh
-D BUFFER_SIZE=42
```

For example:

```sh
cc -Wall -Wextra -Werror \
   -D BUFFER_SIZE=42 \
   main.c \
   get_next_line/get_next_line.c \
   get_next_line/get_next_line_utils.c \
   -Iget_next_line \
   -o gnl_example
```

`BUFFER_SIZE` changes the size of each `read()` operation. It does not change
the public contract that one complete line is returned per successful call.

The maintained regression suite validates several substantially different
values rather than relying on one preferred buffer size.

---

## Implementation

The implementation is split between the public read loop and a small collection
of helpers.

### `get_next_line()`

`get_next_line()` owns the persistent static buffer and controls the read loop.

At a high level it:

1. validates the descriptor and `BUFFER_SIZE`;
2. reuses any unread content already stored in the static buffer;
3. calls `read()` only when that buffer is empty;
4. appends available buffer content to the line being constructed;
5. stops when a newline has been assembled;
6. continues reading when the current chunk does not complete a line;
7. returns the accumulated line;
8. returns `NULL` when EOF is reached without remaining content or when an
   error prevents a line from being returned.

The persistent state is:

```c
static char buffer[BUFFER_SIZE + 1];
```

The extra byte is used for the terminating null character after a successful
`read()`.

### `ft_build()`

`ft_build()` is a static helper used by `get_next_line()`.

It performs two operations:

```text
current accumulated line
        +
current useful buffer content
        ↓
new accumulated line
```

and then removes the consumed bytes from the persistent buffer.

Conceptually:

```text
ft_build()
├── ft_strjoin_gnl()
└── ft_excess()
```

### `ft_strjoin_gnl()`

`ft_strjoin_gnl()` allocates a new string large enough to contain:

- the line already accumulated by previous reads;
- the useful portion of the current buffer;
- the terminating `'\0'`.

Only buffer content up to and including the first newline is appended.

If the buffer contains:

```text
beta\ngamma\n
```

the first build step consumes:

```text
beta\n
```

while the remaining:

```text
gamma\n
```

is preserved for the following call.

After creating the replacement allocation, the previous accumulated line is
freed.

### `ft_concat()`

`ft_concat()` copies the existing accumulated line and the selected portion of
the buffer into the newly allocated destination.

It does not allocate memory itself.

### `ft_excess()`

After part of the static buffer has been consumed, `ft_excess()` moves any
remaining unread bytes to the beginning of that buffer.

Conceptually:

```text
before:
alpha\nbeta
       ^ unread

after:
beta
```

If no bytes remain after the consumed content, the buffer is cleared so that
the next loop iteration performs another `read()`.

This mechanism allows one read operation to contain the end of the current line
and the beginning of the following line without losing those excess bytes.

### `ft_strchr()`

`ft_strchr()` is used to determine whether the accumulated line already
contains a newline.

Once a newline is present, `get_next_line()` can return the completed line
without reading more data.

### `ft_free()`

`ft_free()` is a small static cleanup helper used by the main implementation.

It frees a dynamically allocated line and sets the corresponding pointer to
`NULL`.

---

## Example Flow

Consider:

```text
BUFFER_SIZE = 5
input = "hello\nworld\n"
```

A possible sequence is:

```text
read #1
buffer = "hello"

line = "hello"
buffer becomes empty

read #2
buffer = "\nworl"

line = "hello\n"
buffer = "worl"

return "hello\n"
```

On the next call:

```text
existing buffer = "worl"

line = "worl"
buffer becomes empty

read #3
buffer = "d\n"

line = "world\n"

return "world\n"
```

The exact number of reads depends on `BUFFER_SIZE`, but the external interface
continues to return one line at a time.

---

## Memory Management

Line construction uses dynamic allocation.

As additional input chunks are needed, `ft_strjoin_gnl()` allocates a
replacement string containing the previously accumulated line plus the newly
consumed buffer content.

The previous accumulated allocation is then freed.

This means a long line can involve several successive allocations while it is
assembled across multiple `read()` calls.

The final returned allocation belongs to the caller:

```c
char *line;

line = get_next_line(fd);
if (line)
{
    /* use line */
    free(line);
}
```

The maintained regression suite is also executed under Valgrind to verify that
the tested paths finish without memory leaks or invalid memory accesses.

---

## Repository Structure

```text
42get_next_line/
├── .github/
│   └── workflows/
│       └── ci.yml
├── get_next_line/
│   ├── get_next_line.c
│   ├── get_next_line.h
│   └── get_next_line_utils.c
├── tester/
│   ├── fixtures/
│   │   └── giant_line.txt
│   ├── run_tests.sh
│   └── tests.c
├── .gitignore
├── Doxyfile
├── LICENSE
└── README.md
```

### `get_next_line/`

Contains the maintained mandatory implementation and public header.

### `tester/`

Contains the automated regression harness and the canonical validation runner.

`tester/fixtures/giant_line.txt` preserves the historical 20,000-byte
newline-free input used for long-line validation.

### `.github/workflows/`

Contains the GitHub Actions continuous-integration workflow.

---

## Usage

A minimal program can repeatedly call `get_next_line()` until EOF.

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_next_line.h"

int main(int argc, char **argv)
{
    char    *line;
    int     fd;

    if (argc != 2)
        return (1);
    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
        return (1);
    while ((line = get_next_line(fd)) != NULL)
    {
        fputs(line, stdout);
        free(line);
    }
    close(fd);
    return (0);
}
```

Compile with strict warnings and an explicit buffer size:

```sh
cc -Wall -Wextra -Werror \
   -D BUFFER_SIZE=42 \
   main.c \
   get_next_line/get_next_line.c \
   get_next_line/get_next_line_utils.c \
   -Iget_next_line \
   -o gnl_example
```

Run it against a text file:

```sh
./gnl_example README.md
```

---

## Automated Testing

The canonical maintained validation command is:

```sh
./tester/run_tests.sh
```

The regression harness contains **13 behavioural tests**.

Each test configuration validates:

- empty input;
- one byte without a final newline;
- newline-only input;
- normal multiple-line input;
- a final line without newline;
- consecutive empty lines;
- a line shorter than `BUFFER_SIZE`;
- a line exactly `BUFFER_SIZE` bytes long;
- a line one byte larger than `BUFFER_SIZE`;
- a line spanning several reads;
- the historical 20,000-byte giant line;
- an invalid negative descriptor;
- a closed descriptor producing a `read()` failure.

EOF behaviour is also checked where applicable, including repeated calls after
EOF.

---

## BUFFER_SIZE Test Matrix

The canonical runner executes the same behavioural suite with:

```text
BUFFER_SIZE=1
BUFFER_SIZE=2
BUFFER_SIZE=10
BUFFER_SIZE=42
BUFFER_SIZE=1024
```

Current maintained result:

```text
BUFFER_SIZE=1       13/13 PASS
BUFFER_SIZE=2       13/13 PASS
BUFFER_SIZE=10      13/13 PASS
BUFFER_SIZE=42      13/13 PASS
BUFFER_SIZE=1024    13/13 PASS
```

That represents:

```text
13 tests × 5 BUFFER_SIZE configurations
= 65 behavioural test/configuration combinations
```

per compiler run.

---

## Compiler Validation

The default compiler can be used with:

```sh
./tester/run_tests.sh
```

A specific compiler can be selected through the conventional `CC` environment
variable.

For example:

```sh
CC=clang ./tester/run_tests.sh
```

The maintained repository is continuously validated with both:

```text
GCC
Clang
```

using the same repository-controlled test runner.

---

## Valgrind

When Valgrind is available locally, the canonical runner performs a
representative memory-validation pass using:

```text
BUFFER_SIZE=42
```

The maintained expected result is:

```text
0 bytes in use at exit
All heap blocks were freed
ERROR SUMMARY: 0 errors
```

A Valgrind-detected memory error causes the maintained runner to fail.

---

## Continuous Integration

GitHub Actions configuration lives at:

```text
.github/workflows/ci.yml
```

CI runs automatically for:

- pull requests targeting `main`;
- pushes to `main`.

The workflow validates independent GCC and Clang jobs using Ubuntu 24.04 as the
**CI reference environment**.

Each compiler job:

1. checks out the repository;
2. installs the required build tools, Clang, and Valgrind;
3. executes the canonical `./tester/run_tests.sh` interface;
4. verifies that validation leaves the repository unchanged;
5. verifies that ignored generated artefacts were not left behind.

A separate `CI / documentation` job validates the repository-controlled
Doxygen API documentation. It generates HTML from `Doxyfile`, verifies the
expected output and representative maintained API symbols, removes the
generated files, and verifies repository cleanliness.

Ubuntu 24.04 is the environment used for reproducible CI validation, not a claim
that the implementation is restricted exclusively to that platform.

---

## Maintained Validation Summary

Current maintained validation:

| Validation | Result |
|---|---:|
| `BUFFER_SIZE=1` | 13/13 PASS |
| `BUFFER_SIZE=2` | 13/13 PASS |
| `BUFFER_SIZE=10` | 13/13 PASS |
| `BUFFER_SIZE=42` | 13/13 PASS |
| `BUFFER_SIZE=1024` | 13/13 PASS |
| GCC | PASS |
| Clang | PASS |
| Valgrind errors | 0 |
| Memory remaining at exit | 0 bytes |
| CI pull-request validation | PASS |
| CI push-to-main validation | PASS |

---

## Doxygen Documentation

The maintained interface in `get_next_line/get_next_line.h` is documented
using Doxygen-style comments.

The generated API documentation describes:

- `get_next_line()` and its ownership contract;
- newline, EOF, and error behaviour;
- the configurable `BUFFER_SIZE` interface;
- the persistent static-buffer model;
- the maintained single-file-descriptor scope;
- the lack of independent buffered state for interleaved descriptors;
- the support functions exposed by the maintained header;
- the memory-transfer behaviour of `ft_strjoin_gnl()`;
- the buffer-compaction role of `ft_excess()`.

The documentation reflects the actual maintained implementation. It does not
claim support for the historical bonus multi-file-descriptor behaviour.

The repository tracks a canonical `Doxyfile`.

Install Doxygen on Ubuntu if required:

```sh
sudo apt install doxygen
```

Generate the documentation from the repository root:

```sh
doxygen Doxyfile
```

Generated HTML is written to:

```text
docs/html/
```

The main entry point is:

```text
docs/html/index.html
```

Generated HTML is intentionally ignored by Git. The repository-controlled
`Doxyfile` remains tracked.

Doxygen warnings are treated as validation failures. The dedicated
`CI / documentation` job generates the documentation, verifies representative
API entries, removes the generated files, and confirms repository cleanliness.

---

## Historical Baseline

The repository state before professional portfolio modernization is preserved
through the annotated tag:

```text
portfolio-baseline-2026-09
```

which resolves to:

```text
9c99ac24c8c429b51bfbb804dc0bad80206ebe51
```

This keeps the original project state independently inspectable while allowing
the maintained branch to add testing, CI, documentation, and focused
maintenance improvements.

The maintained modernization does not retroactively present those later
infrastructure improvements as part of the original 42 submission.

---

## Limitations

The maintained implementation intentionally reflects the mandatory project
scope.

In particular:

- it uses one persistent static buffer;
- it does not maintain independent buffered state for interleaved file
  descriptors;
- the historical bonus multi-FD implementation is not part of the maintained
  source tree;
- `BUFFER_SIZE` must be greater than zero;
- no performance or asymptotic-complexity guarantees are claimed by this
  repository.

---

## License

This repository is distributed under the MIT License.

See [`LICENSE`](LICENSE) for the full license text.
