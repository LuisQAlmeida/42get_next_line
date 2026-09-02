#!/bin/sh

set -eu

SCRIPT_DIR=$(
    CDPATH= cd -- "$(dirname -- "$0")" &&
    pwd
)

REPO_ROOT=$(
    CDPATH= cd -- "$SCRIPT_DIR/.." &&
    pwd
)

CC_BIN="${CC:-cc}"
TEST_SOURCE="$REPO_ROOT/tester/tests.c"
GNL_DIR="$REPO_ROOT/get_next_line"
GNL_SOURCE="$GNL_DIR/get_next_line.c"
GNL_UTILS="$GNL_DIR/get_next_line_utils.c"

BUFFER_SIZES="1 2 10 42 1024"
VALGRIND_BUFFER_SIZE=42

TMP_DIR=$(mktemp -d "${TMPDIR:-/tmp}/42gnl-tests.XXXXXX")

cleanup()
{
    rm -rf "$TMP_DIR"
}

trap cleanup EXIT HUP INT TERM

compile_suite()
{
    size=$1
    binary=$2

    printf '\n=== BUILD / BUFFER_SIZE=%s / %s ===\n' \
        "$size" "$CC_BIN"

    "$CC_BIN" \
        -Wall -Wextra -Werror \
        -D "BUFFER_SIZE=$size" \
        -I"$GNL_DIR" \
        "$TEST_SOURCE" \
        "$GNL_SOURCE" \
        "$GNL_UTILS" \
        -o "$binary"
}

run_matrix()
{
    for size in $BUFFER_SIZES
    do
        binary="$TMP_DIR/gnl_tests_$size"

        compile_suite "$size" "$binary"

        printf '\n=== TEST / BUFFER_SIZE=%s ===\n' "$size"
        (
            cd "$REPO_ROOT"
            "$binary"
        )
    done
}

run_valgrind()
{
    binary="$TMP_DIR/gnl_tests_valgrind"

    if ! command -v valgrind >/dev/null 2>&1
    then
        printf '\n=== VALGRIND ===\n'
        echo 'INFO: Valgrind not installed; memory validation skipped'
        return 0
    fi

    compile_suite "$VALGRIND_BUFFER_SIZE" "$binary"

    printf '\n=== VALGRIND / BUFFER_SIZE=%s ===\n' \
        "$VALGRIND_BUFFER_SIZE"

    (
        cd "$REPO_ROOT"

        valgrind \
            --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --error-exitcode=42 \
            "$binary"
    )
}

printf 'Compiler: %s\n' "$CC_BIN"
printf 'Repository: %s\n' "$REPO_ROOT"

run_matrix
run_valgrind

printf '\n=== VALIDATION COMPLETE ===\n'
echo 'PASS: all maintained get_next_line checks passed'
