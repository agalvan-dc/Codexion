#!/usr/bin/env python3
import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / "codexion"
ANSI = re.compile(r"\x1b\[[0-9;]*m")
EVENT = re.compile(r"^(\d+) (\d+) (.+)$")
MESSAGES = {
    "has taken a dongle",
    "is compiling",
    "is debugging",
    "is refactoring",
    "burned out",
}
COLOR_MAP = {
    "has taken a dongle": "\x1b[36m",
    "is compiling": "\x1b[32m",
    "is debugging": "\x1b[33m",
    "is refactoring": "\x1b[34m",
    "burned out": "\x1b[1;31m",
}
FAILURES = []
OBSERVED_MESSAGES = set()


def check(condition, label):
    if condition:
        print(f"  PASS  {label}")
    else:
        print(f"  FAIL  {label}")
        FAILURES.append(label)


def run(cmd, timeout=30, env=None, cwd=ROOT):
    merged = os.environ.copy()
    if env:
        merged.update(env)
    return subprocess.run(
        [str(part) for part in cmd],
        cwd=cwd,
        env=merged,
        capture_output=True,
        text=True,
        timeout=timeout,
    )


def build():
    print("[build]")
    result = run(["make", "re"], timeout=120)
    check(result.returncode == 0, "make re")
    if result.returncode != 0:
        print(result.stdout)
        print(result.stderr)


def norminette():
    print("[norminette]")
    if not shutil.which("norminette"):
        print("  SKIP  norminette not installed")
        return
    result = run(["norminette", "coders"], timeout=60)
    check(result.returncode == 0, "norminette coders")


def parse_events(raw):
    clean = ANSI.sub("", raw)
    events = []
    malformed = []
    unknown = []
    for line in clean.splitlines():
        if not line.strip():
            continue
        match = EVENT.match(line)
        if match is None:
            malformed.append(line)
            continue
        timestamp, coder_id, message = match.groups()
        if message not in MESSAGES:
            unknown.append(message)
        events.append((int(timestamp), int(coder_id), message))
    check(not malformed, f"log format ({len(events)} lines)")
    check(not unknown, f"known messages ({len(unknown)} unknown)")
    for line in malformed[:5]:
        print(f"        malformed: {line!r}")
    for message in unknown[:5]:
        print(f"        unknown: {message!r}")
    return events


def check_colors(name, raw):
    lines = [line for line in raw.splitlines() if line]
    seen = {}
    bad = []
    for line in lines:
        match = re.match(r"^(\x1b\[[0-9;]*m)(.*)\x1b\[0m$", line)
        if match is None:
            bad.append(line)
            continue
        clean = ANSI.sub("", line)
        message = clean.split(" ", 2)[2]
        expected = COLOR_MAP.get(message)
        if match.group(1) != expected:
            bad.append(line)
        seen[message] = match.group(1)
    check(not bad, f"{name}: every line uses its state color")
    for line in bad[:5]:
        print(f"        bad color: {line!r}")
    check(len(set(seen.values())) == len(seen),
          f"{name}: observed colors are distinct")
    return seen


def scenario(name, args, compiles, burnout=False, exact_post=False, timeout=30):
    print(f"[scenario] {name}")
    result = run([BIN, *args], timeout=timeout)
    check(result.returncode == 0, f"{name}: exit code 0")
    seen = check_colors(name, result.stdout)
    OBSERVED_MESSAGES.update(seen)
    events = parse_events(result.stdout)
    burned = [event for event in events if event[2] == "burned out"]
    taken = [event for event in events if event[2] == "has taken a dongle"]
    compiling = [event for event in events if event[2] == "is compiling"]
    debugging = [event for event in events if event[2] == "is debugging"]
    refactoring = [event for event in events if event[2] == "is refactoring"]
    check(len(burned) == (1 if burnout else 0), f"{name}: burnout count")
    if burnout:
        if burned:
            burn_time = int(args[1])
            check(burn_time <= burned[0][0] <= burn_time + 10,
                  f"{name}: burnout logged within 10 ms")
            check(not [event for event in compiling
                       if event[0] > burned[0][0]],
                  f"{name}: no compile after burnout")
    else:
        check(len(compiling) == compiles, f"{name}: compile count")
        check(len(taken) == compiles * 2, f"{name}: dongle take count")
        if exact_post:
            check(len(debugging) == compiles - 1, f"{name}: debug count")
            check(len(refactoring) == compiles - 1, f"{name}: refactor count")
        else:
            check(0 < len(debugging) <= compiles, f"{name}: debug count")
            check(0 < len(refactoring) <= len(debugging),
                  f"{name}: refactor count")
    timestamps = [event[0] for event in events]
    check(timestamps == sorted(timestamps), f"{name}: timestamps ordered")
    return events


def parity_test():
    events = scenario(
        "startup parity",
        ["4", "5000", "300", "10", "10", "3", "0", "edf"],
        compiles=12,
    )
    taken_ids = []
    for event in events:
        if event[2] == "has taken a dongle" and event[1] not in taken_ids:
            taken_ids.append(event[1])
    compile_ids = [event[1] for event in events if event[2] == "is compiling"]
    check(set(taken_ids[:2]) == {2, 4}, "first granted coders are even")
    check(set(compile_ids[:2]) == {2, 4}, "first compiling coders are even")
    check(any(coder_id % 2 != 0 for coder_id in taken_ids[2:]),
          "odd coders start after even coders")


def final_cycle_test():
    events = scenario(
        "single required compile",
        ["3", "5000", "200", "10", "10", "1", "0", "edf"],
        compiles=3,
        exact_post=True,
    )
    compile_ids = [event[1] for event in events
                   if event[2] == "is compiling"]
    debug_ids = {event[1] for event in events if event[2] == "is debugging"}
    refactor_ids = {event[1] for event in events
                    if event[2] == "is refactoring"}
    check(debug_ids == set(compile_ids[:-1]),
          "each non-final coder logs debugging after its last compile")
    check(refactor_ids == set(compile_ids[:-1]),
          "each non-final coder logs refactoring after its last compile")


def dongles(coder_id, total):
    index = coder_id - 1
    return {index, (index + 1) % total}


def cooldown_test():
    args = ["2", "5000", "100", "10", "10", "3", "50", "fifo"]
    events = scenario("cooldown", args, compiles=6, exact_post=True)
    starts = [(event[0], event[1]) for event in events
              if event[2] == "is compiling"]
    violations = []
    for index, (timestamp, coder_id) in enumerate(starts):
        for previous, previous_id in starts[:index]:
            if dongles(coder_id, 2) & dongles(previous_id, 2):
                earliest = previous + 100 + 50 - 2
                if timestamp < earliest:
                    violations.append((previous_id, coder_id, timestamp))
    check(not violations, "cooldown respected on shared dongles")


def stress_test():
    runs = 10
    ok = True
    for _ in range(runs):
        result = run([BIN, "6", "5000", "5", "0", "0", "3", "0", "edf"],
                     timeout=15)
        clean = ANSI.sub("", result.stdout)
        if result.returncode != 0 or clean.count("is compiling") != 18:
            ok = False
            break
    check(ok, f"{runs} repeated runs complete without deadlock")


def invalid_args():
    print("[scenario] invalid arguments")
    base = ["3", "100", "10", "10", "10", "1", "0", "edf"]
    cases = [
        [],
        base[:7],
        ["0", *base[1:]],
        ["abc", *base[1:]],
        ["3", "-1", *base[2:]],
        ["3", "100", "10", "10", "10", "0", "0", "edf"],
        ["3", "100", "10", "10", "10", "1", "-1", "edf"],
        [*base[:7], "sjf"],
        ["3", "100", "10", "10", "10", "1", "0", "edf", "extra"],
    ]
    for args in cases:
        result = run([BIN, *args], timeout=5)
        check(result.returncode != 0, f"invalid args rejected: {args!r}")


def compile_binary(name, flags, target):
    sources = sorted(str(path) for path in (ROOT / "coders").rglob("*.c"))
    command = [
        "cc",
        "-Wall",
        "-Wextra",
        "-Werror",
        "-I",
        str(ROOT / "coders"),
        "-g",
        "-O1",
        "-pthread",
        *sources,
        "-o",
        str(target),
        *flags,
    ]
    result = run(command, timeout=120)
    check(result.returncode == 0, f"{name}: compile")
    if result.returncode != 0:
        print(result.stderr)
        raise SystemExit(1)


def sanitizer_scenarios(name, binary, env, markers):
    print(f"[sanitizer] {name}")
    cases = [
        ["3", "1000", "100", "50", "50", "3", "10", "edf"],
        ["4", "2000", "80", "40", "40", "3", "20", "fifo"],
        ["2", "500", "50", "50", "50", "2", "0", "edf"],
    ]
    for index, args in enumerate(cases):
        result = run([binary, *args], timeout=60, env=env)
        combined = result.stdout + result.stderr
        check(result.returncode == 0, f"{name} case {index}: exit code 0")
        for marker in markers:
            check(marker not in combined, f"{name} case {index}: no {marker!r}")
        if result.stderr.strip():
            print(result.stderr)


def sanitizers():
    temp = Path(tempfile.mkdtemp(prefix="codexion_sanitizers_"))
    try:
        asan = temp / "codexion_asan"
        tsan = temp / "codexion_tsan"
        compile_binary("asan/ubsan", ["-fsanitize=address,undefined"], asan)
        compile_binary("tsan", ["-fsanitize=thread"], tsan)
        sanitizer_scenarios(
            "asan/ubsan",
            asan,
            {
                "ASAN_OPTIONS": "detect_leaks=1:abort_on_error=1",
                "UBSAN_OPTIONS": "halt_on_error=1:print_stacktrace=1",
            },
            [
                "ERROR: AddressSanitizer",
                "LeakSanitizer: detected memory leaks",
                "runtime error:",
            ],
        )
        sanitizer_scenarios(
            "tsan",
            tsan,
            {"TSAN_OPTIONS": "halt_on_error=1:second_deadlock_stack=1"},
            [
                "ThreadSanitizer: data race",
                "WARNING: ThreadSanitizer",
                "ThreadSanitizer: lock-order-inversion",
            ],
        )
    finally:
        shutil.rmtree(temp, ignore_errors=True)


def valgrind():
    print("[valgrind]")
    if not shutil.which("valgrind"):
        print("  SKIP  valgrind not installed")
        return
    result = run(
        [
            "valgrind",
            "--quiet",
            "--leak-check=full",
            "--show-leak-kinds=definite,indirect",
            "--errors-for-leak-kinds=definite,indirect",
            "--error-exitcode=99",
            BIN,
            "3",
            "1000",
            "100",
            "50",
            "50",
            "2",
            "10",
            "edf",
        ],
        timeout=120,
    )
    check(result.returncode == 0, "valgrind: no errors or definite leaks")
    if result.returncode != 0:
        print(result.stdout)
        print(result.stderr)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--quick", action="store_true",
                        help="skip ASan/UBSan, TSan and Valgrind")
    args = parser.parse_args()

    build()
    norminette()
    scenario("edf no burnout", ["3", "1000", "100", "50", "50", "3", "10", "edf"],
             compiles=9, exact_post=True)
    scenario("fifo cooldown", ["4", "2000", "80", "40", "40", "3", "20", "fifo"],
             compiles=12)
    scenario("zero debug/refactor", ["3", "2000", "50", "0", "0", "3", "0", "edf"],
             compiles=9, exact_post=True)
    scenario("single coder burnout", ["1", "120", "50", "50", "50", "2", "10", "edf"],
             compiles=0, burnout=True)
    scenario("burnout after state logs",
             ["3", "500", "200", "10", "10", "2", "0", "edf"],
             compiles=3, burnout=True)
    parity_test()
    final_cycle_test()
    cooldown_test()
    stress_test()
    invalid_args()
    if not args.quick:
        sanitizers()
        valgrind()

    check(len(set(COLOR_MAP.values())) == len(COLOR_MAP),
          "all five message types have distinct colors")
    check(MESSAGES <= OBSERVED_MESSAGES, "battery exercises every message type")
    print()
    if FAILURES:
        print(f"FAILED: {len(FAILURES)} check(s)")
        for failure in FAILURES:
            print(f"  - {failure}")
        return 1
    print("ALL TESTS PASSED")
    return 0


if __name__ == "__main__":
    sys.exit(main())
