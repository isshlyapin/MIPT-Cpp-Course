#!/usr/bin/env python3

import os
import sys
import subprocess
from pathlib import Path

LSZ = 128
DATA_DIR = Path(__file__).resolve().parent / "data"
MAX_LSIZE = 256
TIMEOUT_SEC = 30


def find_program():
    default = Path("build/bitonic_verify")
    if default.exists():
        return default

    print("bitonic_verify not found", file=sys.stderr)
    sys.exit(1)


def parse_ints(text):
    return [int(x) for x in text.split()]


def main():
    prog = find_program()

    ins = sorted(DATA_DIR.glob("*.in"), key=lambda p: int(p.stem))
    if not ins:
        print(f"No .in files in {DATA_DIR}", file=sys.stderr)
        sys.exit(1)

    passed = 0
    failed = 0

    for in_path in ins:
        num = in_path.stem.ljust(2)
        out_path = in_path.with_suffix(".out")
        if not out_path.exists():
            print(f"#{num} test FAILED (missing {out_path.name})")
            failed += 1
            continue

        input_text = in_path.read_text()
        expected = parse_ints(out_path.read_text())

        n = len(parse_ints(input_text))
        lsize = LSZ
        cmd = [str(prog), "--size", str(n), "--lsize", str(lsize)]

        try:
            res = subprocess.run(
                cmd, input=input_text, text=True,
                capture_output=True, timeout=TIMEOUT_SEC,
            )
        except subprocess.TimeoutExpired:
            print(f"#{num} test FAILED  (timeout)")
            failed += 1
            continue

        if res.returncode != 0:
            print(f"#{num} test FAILED  (exit code {res.returncode})")
            failed += 1
            continue

        got = parse_ints(res.stdout)

        if got == expected:
            print(f"#{num} test PASSED")
            passed += 1
        else:
            print(f"#{num} test FAILED  (wrong answer)")
            failed += 1

    print(f"\nTotal: {passed} passed, {failed} failed")
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()