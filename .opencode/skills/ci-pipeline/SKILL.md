---
name: ci-pipeline
description: .github/workflows, CI jobs, clang-version gating in CI, dual-emulator CI, link errors in CI runners.
---

Applies when the user asks to add/fix CI, you're touching `.github/workflows/`, or a CI run
failed with a flag/linker/attribute error that didn't fail locally.

## The local-vs-CI gap

The user's machine has a locally-built LLVM trunk (often clang 23+). CI runners may have an
older clang from the distro repos. Anything trunk-only **must** be gated or CI will fail in
ways you can't reproduce locally:
- lifetime-safety flags (`-Wlifetime-safety-all`, `-Xclang -fexperimental-lifetime-safety-c`,
  …) → gate behind a clang-version check in CMake (see `clang-lifetime-safety` skill).
- `[[clang::lifetimebound]]` on C functions → CI emits `-Wunknown-attributes`. Replace with
  the `NEKOS_LIFETIMEBOUND` macro backed by `__has_c_attribute`, so it expands to nothing on
  older clang.
- any other trunk-only attribute or pragma → same pattern: macro + `__has_*` check.

## Workflow shape

- Separate jobs for build / format / test, or one job with steps — match what the user
  already has; don't restructure without asking.
- Format job should use `git-clang-format` style logic: detect whether the run is on a PR
  (diff against the base ref) or a push (diff against the push's before-sha), and only
  fail on lines the change touched. The user has rejected "format the whole tree and fail if
  anything drifts" because it blocks unrelated PRs.
- Test jobs run both qemu and bochs kernel tests (see `qemu-bochs-kernel-tests` skill).
  Don't drop one in CI even when one is flaky — the user wants both.

## Common CI-only breakage

- `collect2: fatal error: cannot find 'ld'` → the runner's `ld` isn't `ld.lld`; either
  install `lld` in the workflow or make sure the build uses the right linker by name.
- Missing `lib32-gcc-libs` or similar 32-bit runtime when linking test executables.
- `ld.lld: error: ... cannot be used against local symbol` when the link is accidentally
  PIE/dynamic — kernel linking needs `-static -no-pie`, gated appropriately.
- Bochs missing the VGA BIOS image at a hardcoded path → use glob/fallback discovery in the
  test script, not a single path (see `qemu-bochs-kernel-tests`).
- Bochs hanging past its timeout because the script waited for the process to exit on its
  own → kill bochs from the start the way the qemu script does.

## Linting

The user has tried adding a lint job and rejected it ("i dont like this lint, lets keep
without lint for now then"). Don't add a lint job unless they explicitly ask. `just format`
(clang-format) is enough for now.

## When CI fails

Don't summarize the failure — paste the failing lines verbatim, diagnose, and propose the
minimal gating fix. If you can't reproduce locally, say so and explain why (older clang,
different linker, missing distro package) before guessing.
