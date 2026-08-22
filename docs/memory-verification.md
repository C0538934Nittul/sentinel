# Memory verification

§13.2 requires demonstrating a reasonable method for detecting memory-management problems. This
project uses two complementary tools rather than one, because they catch different things and
neither is complete on its own — you should be able to re-run both live if asked.

`valgrind` is not used: it does not support Apple Silicon (arm64 macOS), which is what this
project is built on.

## Tool 1 — AddressSanitizer + UndefinedBehaviorSanitizer (`asan` CMake preset)

```bash
cd analyzer
cmake --preset asan
cmake --build --preset asan
ctest --preset asan
```

`analyzer/CMakePresets.json`'s `asan` preset compiles and links with
`-fsanitize=address,undefined -fno-omit-frame-pointer -g -O1` — a separate build directory
(`build/asan/`) from `debug`/`release`, so sanitizer instrumentation never leaks into the
binary you actually ship or profile.

**What this proves:** ASan instruments every memory access and catches, at the exact line it
happens, on the exact input that triggered it:
- use-after-free / use-after-scope
- heap/stack buffer overflow (reads and writes)
- double-free
- some invalid-free cases

UBSan (bundled in the same `-fsanitize=` flag) separately catches signed integer overflow, null
pointer dereference through a reference, misaligned/invalid enum values, and other undefined
behavior that doesn't necessarily corrupt memory but is still a real bug.

**What this does NOT prove:** ASan does not report a leak summary on macOS.
[LeakSanitizer](https://clang.llvm.org/docs/LeakSanitizer.html) (the leak-detection component
normally bundled with ASan on Linux) is **not supported on macOS** — running with
`ASAN_OPTIONS=detect_leaks=1` on this platform silently does nothing rather than erroring, which
is exactly the kind of false confidence worth naming explicitly rather than discovering on
video. ASan here is a correctness/UB tool, not a leak-detection tool — that's Tool 2's job.

It also only catches what the exercised code path actually touches: a bug in an untested branch
of `evaluate()` won't show up just because the binary was built with ASan. It's a runtime
detector, not a static analyzer — coverage is bounded by what your tests actually run.

## Tool 2 — macOS `leaks` (with `MallocStackLogging`)

```bash
MallocStackLogging=1 leaks --atExit -- \
  ./analyzer/build/debug/sentinel-analyzer \
  --events sample-data/mixed-events.json \
  --config config/rules.json
```

- `leaks` is Apple's built-in memory-graph leak detector (ships with Xcode Command Line Tools).
- `--atExit` runs the target process to completion and then walks its memory graph right before
  it would have exited, rather than attaching to a long-running process — appropriate here since
  `sentinel-analyzer` is a short-lived CLI, not a server.
- `MallocStackLogging=1` records allocation stack traces so a reported leak comes with *where it
  was allocated*, not just a byte count. Without it, `leaks` still finds leaks but can't tell you
  where they came from.

**What this proves:** `leaks` walks the process's live memory graph at exit and reports any
block that is still allocated but has become unreachable from any root (stack, registers,
globals, thread-local storage) -- a true leak, in the sense that nothing in the program could
free it even if it wanted to. This is the leak-detection coverage ASan doesn't provide on macOS.

**What this does NOT prove:** `leaks` finds *unreachable* memory, not *unnecessary* memory --
something the program still holds a live pointer to but never actually needed again (a slow
logical leak, e.g. an ever-growing cache) will not be reported, since it's technically still
reachable. It also only inspects the process's state at the single moment it's invoked
(`--atExit` here), so it says nothing about peak memory usage or growth over a long-running
process's lifetime — not a concern for this CLI's one-shot execution model, but worth naming as
a boundary of what this specific invocation proves.

You will also see this line in the output, which is expected and not an error:

```
Process <pid> is not debuggable. Due to security restrictions, leaks can only show or save
contents of readonly memory of restricted processes.
```

This is a macOS System Integrity Protection notice about *inspecting memory contents* (e.g. to
show you the bytes at a leaked address), not about whether leak detection itself ran — the leak
count and summary at the bottom of the output (`Process <pid>: N leaks for M total leaked
bytes`) is still accurate.

## Baseline result (current codebase)

Both commands were run against the current build (all four assessed methods still stubbed to
return `{}` or throw, per `docs/ASSUMPTIONS.md`):

- `ctest --preset asan`: 78/78 tests passed, no sanitizer reports.
- `leaks --atExit` against `sentinel-analyzer --events sample-data/mixed-events.json --config
  config/rules.json`: `0 leaks for 0 total leaked bytes`.

This is expected and not yet meaningful evidence of your own memory-management work — the
stubbed code paths don't yet exercise the ownership-heavy parts of the analyzer (rule
construction via `RuleFactory`, correlation state inside `evaluate()`) under real load. **Re-run
both commands after implementing the four assessed methods**, and record the new output here (or
in your own notes) before your video defense — a before/after comparison is stronger evidence
than either snapshot alone, and re-running these live on request is specifically what this
document exists to make easy.
