---
name: kernel-compile-test-tool
description: Semantics of review-bot_kernel_compile_test, how to write and call the build-QEMU test tool, its parameters and limits.
---

Applies when you (any agent, but usually the reviewer) call the `review-bot_kernel_compile_test`
MCP tool, or when the user asks you to "test this snippet by compiling it", or to explore edge
cases / stress-test the tool itself.

## What it does

Builds a temporary kernel-test ISO containing the source file you give it, runs that ISO under
qemu, and returns the serial test log. It writes a tiny test harness that calls a single entry
point — everything else (existing repository test entrypoints) is excluded from the build so
only your snippet runs.

## The contract the source must satisfy

The source file you pass must define:

```
namespace tests { void runReviewBotTests(); }
```

Inside that function you call the project's kernel test helpers — `NEKOS_EXPECT_TRUE`,
`NEKOS_EXPECT_VGA_TEXT`, the other `NEKOS_EXPECT_*` macros from the test headers — and call
`tests::log("message\n")` to write progress to the serial log. `kprintf()` goes to VGA only and
is invisible to the tool's output, so use `tests::log()` for any human-readable debug text.

You can include any project header that's freestanding-safe. You get libc (the same libc the
kernel links) for free — string functions, `itoa`/`utoa`, etc. all work. C++ templates,
`constexpr`, references, `static_assert` all work. You don't get host stdlib (`<stdio.h>`,
`setjmp`, etc.) — this is freestanding.

## Parameters

`source` (required): absolute path to the `.cpp` you wrote. Put your file under `/tmp/opencode`
first — the tool expects that path prefix and writable space there.

`output_name` (optional): names the temporary work directory; defaults to the source filename
stem. Doesn't otherwise change behavior — don't rely on it for anything important.

`timeout_seconds` (optional): default ~10s. Raise for slow tests; lower to fail fast.

`dry_run` (optional): true shows the generated build/QEMU commands without compiling — useful
to verify the tool sees what you think it sees.

`clean` (optional): true deletes the temporary work dir after a successful run; `/tmp/opencode`
otherwise accumulates dirs.

## Known sharp edges (the user has surfaced all of these)

- Empty test files (no `NEKOS_EXPECT_*` calls) print `passed=0 failed=0` and `ALL TESTS
  PASSED`. That's misleading — treat zero-assertion output as "no tests ran", not success.
- Compilation diagnostic paths are in the build tree, not the source path you wrote. Map back
  mentally before quoting line numbers to the user.
- A "Build failed with exit code 0" is contradictory but real — it means CMake configured OK
  but the build step failed. Check the build step's output, not the CMake return code.
- `tests::log()` is an invisible dependency — its declaration lives in the test support
  header, but a newcomer reading only `test.hpp` may not realize it exists. If you're
  extending the tool docs, mention it; don't expose the underlying script path to users
  (they should think of it as a tool, not a `.sh`).

## When you don't need to use it

Don't call it for host-side libc tests, build/CI questions, or anything that doesn't actually
need a kernel-boot environment. It's slow (~seconds per run) and creates a temp dir per call.
For pure C/C++ logic checks against libc, the host Unity tests are cheaper.

## How to talk about it

When you document the tool or mention it in a prompt, describe it as `review-bot_kernel_compile_test`,
the MCP tool — **never** as "the kernel-compile-test script" or expose its `.sh` path. The
user has explicitly corrected this: users (downstream agents, humans reading the prompt)
should only know about the tool abstraction.
