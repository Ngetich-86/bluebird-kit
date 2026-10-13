#!/usr/bin/env python3
"""Imperium | Blue Bird - test runner for the reconciliation exercise.

Usage (from the kit root or anywhere):
    python3 tests/run_tests.py -- ./reconcile            # C / C++ / Rust / Go binary
    python3 tests/run_tests.py -- java -cp . Reconcile   # any command works

Runs your program once per tests/cases/*.in (input on stdin) and compares its
stdout with the matching .out file. Account totals must match exactly; the
flagged request IDs may be printed in any order but each must appear once.
Requires Python 3.8+ and nothing else.
"""
import pathlib, subprocess, sys, time

def parse(text):
    lines = text.split()
    if "BAD" not in lines:
        raise ValueError("output has no 'BAD <count>' line")
    i = lines.index("BAD")
    net = [int(x) for x in lines[:i]]
    count = int(lines[i + 1])
    ids = [int(x) for x in lines[i + 2:]]
    if count != len(ids):
        raise ValueError(f"BAD says {count} IDs but {len(ids)} were printed")
    return net, ids

def main():
    if "--" not in sys.argv or sys.argv.index("--") == len(sys.argv) - 1:
        print(__doc__); return 2
    cmd = sys.argv[sys.argv.index("--") + 1:]
    cases = sorted((pathlib.Path(__file__).parent / "cases").glob("*.in"))
    passed = 0
    for case in cases:
        expected_net, expected_ids = parse(case.with_suffix(".out").read_text())
        start = time.perf_counter()
        try:
            with case.open("rb") as f:
                proc = subprocess.run(cmd, stdin=f, capture_output=True, timeout=120)
        except FileNotFoundError:
            print(f"cannot run {cmd[0]!r}: build your program first"); return 2
        except subprocess.TimeoutExpired:
            print(f"FAIL {case.stem}: timed out after 120 s"); continue
        secs = time.perf_counter() - start
        problem = None
        if proc.returncode != 0:
            problem = f"exit code {proc.returncode}: {proc.stderr.decode(errors='replace')[:300]}"
        else:
            try:
                net, ids = parse(proc.stdout.decode())
                if len(ids) != len(set(ids)):
                    problem = "a flagged request ID was printed more than once"
                elif net != expected_net:
                    bad = next(a for a in range(max(len(net), len(expected_net)))
                               if a >= len(net) or a >= len(expected_net) or net[a] != expected_net[a])
                    problem = f"account totals differ (first at account {bad})"
                elif sorted(ids) != sorted(expected_ids):
                    missing = sorted(set(expected_ids) - set(ids))[:5]
                    extra = sorted(set(ids) - set(expected_ids))[:5]
                    problem = f"flagged IDs differ; missing {missing}, unexpected {extra}"
            except ValueError as e:
                problem = str(e)
        if problem:
            print(f"FAIL {case.stem} ({secs:.2f}s): {problem}")
        else:
            passed += 1
            print(f"PASS {case.stem} ({secs:.2f}s)")
    print(f"\n{passed}/{len(cases)} tests passed")
    return 0 if passed == len(cases) else 1

if __name__ == "__main__":
    sys.exit(main())
