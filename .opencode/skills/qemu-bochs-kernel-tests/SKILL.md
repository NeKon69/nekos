---
name: qemu-bochs-kernel-tests
description: qemu and bochs kernel-test serial scripts, bochsrc discovery, running kernel tests under both emulators.
---

Applies when running kernel tests, debugging qemu/bochs serial output, fixing a kernel-test
script, or chasing an emulator-only bug.

## The dual-emulator rule

Kernel tests run under **both qemu and bochs**. The two emulators disagree on subtle CPU
behavior (the canonical example: whether the GDT accessed bit is set by `lgdt` alone, or only
after a segment register is reloaded through that descriptor). If one emulator passes and
the other doesn't, **the user wants to know** — do not "fix" the discrepancy by deleting one
run. Add both to CI.

This is intentional, not redundancy. qemu is fast; bochs is closer to real hardware on
fine-grained semantics. A passing qemu + failing bochs is signal, not noise.

## Scripts

Test scripts live under `scripts/` and are invoked from the justfile `test` recipe. They:
- boot the kernel ISO under the emulator,
- capture the serial log,
- wait for the test-summary sentinel line (or timeout and fail),
- kill the emulator the moment the summary appears (don't wait for the timeout to expire).

Read the actual scripts before changing them; don't reimplement from memory.

## bochsrc discovery

Bochs is sensitive to its environment (`BXSHARE`, `LTDL_LIBRARY_PATH`, VGA BIOS image
location). When a script needs a path that may vary across distros (VGA BIOS image, rom
directory), discover it by **glob / fallback** over candidate paths, not a single hardcoded
path. A path that works on the user's machine will break in CI and vice versa.

The bochsrc for kernel tests is generated into the build dir, not the global config dir.
Don't write to `~/.config/bochs/`.

## Timeout

Default test timeout is short (around 10s). A test that needs longer should expose a parameter
for it, but the default should stay short — a kernel hanging past a few seconds is almost
always a bug, not a slow test.

## Helper behavior

- The test framework prints a sentinel begin and a sentinel summary line over serial. Parse
  those, not arbitrary output. "No tests run" (zero passed, zero failed) is a **failure**,
  not a pass.
- `kprintf()` writes to VGA only and is **invisible** to the serial log. Only the test-harness
  log function appears in serial. Don't suggest debugging via `kprintf` when what you actually
  need is serial output.
- When the user pastes a "tests still fail" dump, actually run the tests yourself (`just test`)
  before proposing a fix — stale build dirs have sent people down hours-long wrong paths.
  Suggest wiping `build_{{profile}}/` if the behavior is suspicious.

## When something only fails in one emulator

1. Reproduce locally in **both** before guessing.
2. If one emulator is clearly wrong (diverges from the Intel SDM), say which and cite the
   SDM section if you can find it locally.
3. Never "resolve" by dropping the failing emulator. Add the discrepancy to the test
   expectations if it's an agreed emulator quirk (e.g. qemu not setting the accessed bit on
   `lgdt` alone is a known thing — the test was adjusted to match qemu, and the same test
   runs under bochs where the bit gets set). Both runs must pass.
