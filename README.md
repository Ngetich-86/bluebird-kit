# Imperium | Blue Bird — Reconciliation exercise kit

Work in your own editor and terminal. Build and test locally, then paste your final code,
your build/run command and your test output into the assessment page.
The assessment page does not compile or run code.

## What is in this kit

| Path | Purpose |
|---|---|
| `starter/c/reconcile.c` | C starter: input/output already written; implement `reconcile()` |
| `starter/cpp/reconcile.cpp` | C++ starter, same structure |
| `tests/run_tests.py` | Test runner. Works with any language that reads stdin and writes stdout |
| `tests/cases/*.in, *.out` | 10 test cases with expected output |

## Requirements on your machine

- **Python 3.8 or newer** (only for the test runner; no packages needed).
- A compiler for your chosen language:
  - C: `gcc` or `clang` (Xcode Command Line Tools on macOS, `build-essential` on Ubuntu, MSYS2 or WSL on Windows)
  - C++: `g++` or `clang++` (C++17)
  - Rust: `rustc` 1.70+ · Go: `go` 1.20+ · Java: JDK 17+

Please check this **before** you start the recorded part. The assessment page includes a step
where you build the starter and paste the test output.

## Build and run (exact commands)

From the kit root:

```sh
# C
cc -O2 -std=c11 -o reconcile starter/c/reconcile.c
python3 tests/run_tests.py -- ./reconcile

# C++
c++ -O2 -std=c++17 -o reconcile starter/cpp/reconcile.cpp
python3 tests/run_tests.py -- ./reconcile
```

On Windows without WSL, use `reconcile.exe` and `py -3` in place of `./reconcile` and `python3`.

The unmodified starter **builds and runs, but fails the tests** (it returns nothing).
That is expected. Before you start, you should see `0/10 tests passed`.

**Rust, Go or Java:** there is no starter file. Write a program that follows the input and
output format below, then run it with the same runner:

```sh
rustc -O -o reconcile reconcile.rs        && python3 tests/run_tests.py -- ./reconcile
go build -o reconcile reconcile.go        && python3 tests/run_tests.py -- ./reconcile
javac Reconcile.java                      && python3 tests/run_tests.py -- java -Xmx2g Reconcile
```

## The task

The agent's reconciliation operator (shown on the assessment page) scans one batch of ledger
rows in memory. Write a faster version of `reconcile` that returns:

1. `net[M]`: the net delta per account (the sum of `delta` over that account's rows).
2. Every request ID that does **not** have **exactly one negative row and exactly one positive
   row whose deltas sum to zero**. Everything else is flagged: a single row, two debits, two
   credits, a debit and credit that don't cancel, or more than two rows (even if they sum to zero).

### Input bounds

| Field | Type | Bounds |
|---|---|---|
| `n` (rows) | count | 0 ≤ n ≤ 10,000,000 |
| `req_id[i]` | unsigned 64-bit | any value, **including 0 and 2^64−1**; up to 5,000,000 distinct |
| `acct[i]` | signed 32-bit | dense account index, 0 ≤ acct < M; always in range |
| `M` (accounts) | count | 1 ≤ M ≤ 100,000 |
| `delta[i]` | signed 64-bit cents | nonzero; \|delta\| ≤ 100,000,000,000 (10^11) |

Rows arrive in **arbitrary order**. They are not grouped or sorted by request or account.

### Integer-overflow assumptions

- Because \|delta\| ≤ 10^11 and n ≤ 10^7, any sum of deltas (per account or per request) has
  magnitude at most 10^18, which fits in a signed 64-bit integer (max ≈ 9.22 × 10^18).
  You don't need overflow checks on those sums if you use 64-bit accumulators.
- Per-request row counts can be as large as n. If you store small counters (for example
  8-bit), make sure they can't wrap around to look like "exactly one". Saturating is fine.
- Request IDs are unsigned 64-bit. In Java, read them with `Long.parseUnsignedLong` and print
  them with `Long.toUnsignedString`.

### Constraints

- Single-threaded. (You may talk about parallelism in your explanation.)
- **No heap allocation per row.** Allocate everything up front, sized from `n` and `M`.
- **No library hash map** (`std::unordered_map`, `HashMap`, Go `map`, and so on). Write your own
  table, or sort or partition. If you sort, say why it meets the constraints.
- Working memory: assume up to about 1 GB.
- Any of C, C++, Rust, Go or Java.

### Input and output format (stdin → stdout)

```
n M
req_id acct delta        (n lines)
```

Output:

```
net[0]
...
net[M-1]                 (M lines)
BAD k
id_1
...
id_k                     (k flagged request IDs, any order, each once)
```

## What to submit (on the assessment page)

- Your final source code (paste the whole file).
- Language and the exact build and run command you used.
- The full output of `python3 tests/run_tests.py -- <your command>`.
- A recorded verbal explanation of your approach and tradeoffs (the page prompts you).

If a test fails and you run out of time, submit anyway and say what you would fix.
Partial work with an honest explanation is reviewed.
