---
description: Writing libc or kernel tests against a spec; tests for strcpy/itoa/kprintf/etc.
---

# write-tests

Applies when the user asks you to write tests for a libc function or a kernel module. Two
tiers exist; both obey the same durable rules.

## The single most important rule

**Tests are written against the spec, never the implementation.**

Do not open the source file for the function under test. You may read:
1. The header declaration — the contract.
2. The spec the user pastes.
3. Existing sibling tests for structural inspiration (naming, runner grouping, assertion style).

Nothing else. If you catch yourself reading the impl, stop and close the file.

## What to include

- Edge cases the spec **implies**: empty input, exact-fit buffer, sign wraparound, base max
  for radix conversion, null terminator handling, count==0 cases, count longer than source,
  source longer than count (truncation = UB → note, don't test).
- One test per behavior the spec distinguishes. If two cases test the same path, drop one.
- The plain form of cppreference functions by default; the bounds-checked variant only when
  the user explicitly asks. Note which form you're testing.
- UB conditions: flag in a comment, do not exercise.

## What to omit

- Filler cases that name a scenario rather than testing a new behavior path. If the basic
  case already covers it, the named variant adds nothing.
- Concrete filled-in test blocks in any prompt you write downstream. Describe the spec and
  the edge cases; let whoever writes the test fill in the code. Concrete examples make the
  model less creative.
- Re-implementations of the function under test inside the test file. The user already wrote
  the impl; trust the spec.

## Structure

- One test file per module under test, located next to that module's headers/sources.
 Mirror the declaration layout of the header.
- One test function per case, grouped under a runner function that calls each in
  **declaration order from the header**.
- Use the project's assertion macros from the relevant test-support header; do not invent
  new ones.
- When you add a new function to a module that already has tests, append — don't rewrite the
  runner. If the user says you rewrote everything again, you over-edited; use targeted edits
  next time.

## When the user rejects a test

The user pastes the rejected edit with a review note. Rework from the note:
- Do not re-add the dropped case verbatim.
- Do not swing to the opposite extreme (deleting all the cases).
- Make the smallest change the note implies and keep the rest.

## After writing

- Re-run `just test` (or the most specific recipe) before saying the tests pass.
- If a test you wrote catches a real bug in the impl, tell the user. Do not fix the impl
  yourself; the user writes that.