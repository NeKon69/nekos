# AGENTS.md — NeKoS

Guidance for any opencode agent working in this repo. Per-task details are
available via skills which you can load when neccessary using the
"Skill" tool.

---

## 1. Project

NeKoS is a hobby i686 (32-bit x86) multiboot kernel. Freestanding C17 + C++20, no stdlib, no
exceptions, no RTTI, no SSE (intentionally disabled until SSE state is set up). It is a toy
project for learning — **no plans for other architectures, no long mode, no ambition to ship
a usable OS.** Recommendations to "plan for portability" or "switch to long mode" are noise.

The user follows the [osdev.org](https://wiki.osdev.org/) guide and treats it as the canonical
reference. When in doubt about a kernel concept, link the relevant osdev page; when the user
asks about a flag/behavior, prefer the local `~/llvm-project` RST docs and `man` pages over
web fetches (the machine is sometimes offline).

The user **writes all kernel and libc implementation themselves.** Agents do not write
`kernel_main`, descriptor-table code, ISR stubs, libc functions, or any feature impl for them.
Agents assist by:

- writing tests against a spec the user provides,
- refactoring and mechanical edits (renames, casts, include order),
- build / CMake / CI / justfile housekeeping,
- iterating tooling (reviewer bot, MCP tools, neovim config, clang flags),
- generating PR titles/descriptions from a diff,
- diagnosing logs, linker errors, and PR review comments the user pastes verbatim,
- explaining concepts and linking osdev pages when asked.

When the user says "no. literally. just build" or "stop explaining" — stop talking and run the
command.

---

## 2. Repo areas

- `kernel/` — the kernel itself: entry `_start` in `src/boot.asm`, `kernel_main` in C++,
  descriptor tables, drivers, in-kernel tests under `tests/`.
- `libc/` — freestanding C stdlib. Used by the kernel now and by future userland; do not
  introduce host-only assumptions.
- `libcxx/` — placeholder for a future C++ stdlib. Currently a header library; structure is
  being figured out incrementally.
- `third_party/unity/` — Unity test framework (git submodule) used for host-side libc tests.
- `cmake/` — CMake helpers (e.g. `AddUnityTests.cmake`).
- `scripts/` — qemu/bochs kernel-test shell scripts invoked from the justfile.
- `build_{{profile}}/` — build outputs (`nekos.elf`, `nekos.iso`, serial logs, `bochsrc-*`).
  Never hardcode an absolute path into one of these; always go through the build dir variable.

Each of `kernel/`, `libc/`, `libcxx/` has its own `CMakeLists.txt` and is `add_subdirectory`'d
from the root. Tests for a library live next to that library (e.g. `libc/tests/`), not in a
global `tests/` tree.

---

## 3. Build, run, test

The justfile is the single entry point. Run `just <recipe>` rather than reimplementing what a
recipe does:

- `just build` — configure + build + `grub-mkrescue` an ISO into `build_{{profile}}/nekos.iso`.
- `just run` — `just build` then `qemu-system-i386 -cdrom`.
- `just test` — build, then run the qemu and bochs kernel-test scripts, then `ctest` for the
  host-side Unity tests.
- `just format` — `git ls-files '*.cpp' '*.h' '*.hpp' '*.c' | xargs clang-format -i`.
- `just clean` — `rm -rf build_*`.

Profiles via `profile=Release just build` (default `Debug`).

Build/test failures: copy the failing command's actual output verbatim into your reply when
diagnosing — the user trusts logs over summaries. If a build is stale in a confusing way,
suggest the user wipe `build_{{profile}}/` and rebuild before chasing ghosts.

---

## 4. Naming conventions

- Variables and class members: **PascalCase**.
- C++ functions and methods: **camelCase**.
- libc functions and ASM labels: **snake_case**.
- Namespaces: lowercase (`nekos`, `drivers`, `tests`, `tests::vga`, `tests::gdt`).
- Macros and CMake variables: `UPPER_SNAKE` with a `NEKOS_` prefix when project-scoped.

Follow the convention of the file you are editing; do not mass-rename during an unrelated edit.

---

## 5. Header / source / extern C

- Header / source split is mandatory. Declarations in `.h`/`.hpp`, definitions in `.c`/`.cpp`.
  A freestanding stdlib cannot be header-only because the user wants the implementation
  linkable into multiple targets (kernel, userland, host tests).
- C++ headers use `#pragma once`. C headers use traditional include guards.
- A C header that C++ will consume wraps its prototype block in the project's `extern_c.h`
  macros: `EXTERN_C_BEGIN` / `EXTERN_C_END`. Never write raw `extern "C"` in a `.h` file and
  never put it in a `.hpp`. The reason is C++ name mangling of C-defined symbols at link time.
- Forward-declare in headers where you can; include only what the header's contract needs.

---

## 6. Code style

- `/// \param`, `/// \return` doxygen comments on public functions. Brief `//` line comments
  are allowed only where meaning is non-obvious; do not over-comment.
- `constexpr` helpers over `#define` macros for bit-packing and flag construction.
- Inline asm via `asm volatile(...)` with explicit clobber lists; standalone asm functions
  end with `.size <name>, . - <name>` so the debugger has frame info.
- GAS/AT&T syntax via the system assembler by default. NASM is acceptable if the user asks.
- C++ style: 2-space indent, opening brace on the same line, `reinterpret_cast` over C-style (basically, LLVM style).
  casts, no `auto` for raw numeric types where the type isn't obvious.

---

## 7. Behavioral rules (cross-cutting)

These apply in every session regardless of agent role.

- **Test before claiming.** Don't say "it works" without running `just test` (or the most
  specific relevant tool). The user pastes logs and expects diagnosis, not speculation.
- **File search.** Glob and Grep skip git-ignored files. Use the Read tool, `git ls-files`,
  `ls`, or `find` for anything that might be ignored — dotfiles (`.clangd`, `.nvim`), build
  outputs, and most of `.opencode/`.
- **Edits.** Prefer `sed`/`perl` for targeted changes. Never rewrite a whole file when the
  user asked for one rename or one macro addition. If the user rejects an edit, rework it
  using the review note — do not re-apply the same edit.
- **Diagnose before suggesting.** When shown a linker or compile error, reproduce it locally
  (or read the exact source line) before proposing a fix. "Are you sure…?" is welcome; a
  confident wrong fix is not.
- **Don't propose host stdlib.** `setjmp`, `math.h`, `<stdio.h>` etc. are not freestanding.
  If a dependency pulls them in (e.g. Unity internals), surface it explicitly rather than
  silently letting the build adopt host behavior.
- **Don't write concrete test examples in prompts.** When prompting the reviewer bot or any
  downstream agent, describe the spec and the edge cases — never paste a filled-in `void test_…`
  block. Concrete examples make the model less creative.
- **Offline mode.** If a web search fails or seems inappropriate, fall back to local docs
  (`~/llvm-project/clang/docs/`, `~/llvm-project/llvm/docs/`, `man`, `git log`).

---

## 8. About the user

The user is a hobby OSdev learner who:

- Writes in lowercase, runs words together, makes typos, and is not asking to be corrected on
  English. Match the register; do not mirror the typo into your own prose.
- Learns by being **told why**, not handed code. When the user pastes an osdev excerpt and
  asks "what does this mean?", explain the concept, link to the relevant osdev page, and let
  them write the impl. They will often say "ok i think i might need to create my own funcs…"
  and then go write the code themselves.
- Directs by rejection: "no not you", "no frick you", "no. literally. just build". A "no" is
  a hard redirect, not a debate. Re-read what they actually asked for, then do that.
- Pastes PR comments, build logs, and reviewer-bot output verbatim, often with "???". This is
  a request to diagnose and explain, not to summarize.
- Enjoys the rivalry between their local Qwen3.6-35B model and big proprietary models (GPT-5.5,
  Codex). When the reviewer agent is opinionated, that's a feature. The user finds it fun.
- Iterates tooling in tight loops: try the tool → see the output → adjust the prompt/script
  → try again. Don't propose a "long-term roadmap" for a tool the user is tuning this minute.
- Wants McGuiver-grade solutions. A "predebug" field can be a script. A test macro can be
  reused as a runtime check. Pragmatic beats pure.

---

## 9. Toolchain posture

- The user's machine has a locally-built LLVM trunk (often clang 23+). CI runners may have
  older clang. Trunk-only flags and attributes must be gated behind a clang-version check in
  CMake, or behind `__has_c_attribute` / a project macro for attributes the older clang emits
  `-Wunknown-attributes` on.
- The linker in use is `ld.lld`. Don't propose GNU `ld`-only directives without checking.
- When a flag's exact spelling is uncertain, look it up in the local LLVM source
  (`~/llvm-project/clang/include/`, `~/llvm-project/clang/docs/`) — the user prefers this to
  hallucinated flag names.

Details of the lifetime-safety flags and the `NEKOS_LIFETIMEBOUND` macro live in the
`clang-lifetime-safety` skill.

---

## 10. Reviewer agent baseline

Applies only when this session is the `reviewer` agent. Full detail in the
`reviewer-workflow` skill; the durable parts:

- **Read prior threads first.** Call `review_threads` before posting. The user has penalized
  the reviewer repeatedly for submitting a review without reading previous discussion.
- Reply to the author's last comment **before** posting the new review.
- Use line **ranges** (`start_line` + `line`), not single lines.
- No JSON output. Include bots by default unless the user says otherwise.
- APPROVE only when there are 2-3 nits or none; otherwise COMMENT. REQUEST_CHANGES rarely.
- You may use `review-bot_kernel_compile_test` to actually compile and run a snippet before
  posting a claim about it. Verified comments are worth more than unverified ones.

## 11. Skills

`.opencode/skills/` holds per-task detail that auto-loads when its `description` matches the
current task. An active skill extends or overrides the matching section of this file. If no
skill matches, this file is the whole contract.

Current skills:

- `write-tests` — writing libc or kernel tests against a spec.
- `qemu-bochs-kernel-tests` — qemu + bochs serial-test scripts, bochsrc discovery, dual-emulator rule.
- `clang-lifetime-safety` — clang trunk lifetime-safety flags, `NEKOS_LIFETIMEBOUND` macro gating.
- `reviewer-workflow` — reviewer agent etiquette, MCP tools, approve thresholds.
- `kernel-compile-test-tool` — semantics of `review-bot_kernel_compile_test`.
- `neovim-debug-config` — `.nvim` debug schema, `<leader>b*` mappings, predebug.
- `pr-description-from-diff` — generating PR titles/descriptions in this repo's style.
- `ci-pipeline` — `.github/workflows`, clang-version gating, dual-emulator CI.
- `bit-packing-descriptors` — GDT/IDT descriptor encoding helpers.
- `address-reviewer-comments` — refactoring etiquette when applying review feedback.

---

## 12. Maintaining this file and skills

When the user says something like "add a rule that …", "remember that …", "from now on …",
"can you add a skill for …", or corrects an agent's behavior with a generalizable lesson:

1. If the lesson is **cross-cutting** (applies to many session types) → add it to the matching
   section of this `AGENTS.md`, or create a new short section.
2. If the lesson is **task-specific** (e.g. a test-writing nuance, a reviewer etiquette
   detail) → add it to the relevant skill in `.opencode/skills/`, or create a new skill file
   if no existing one covers the task.
3. Keep both this file and the skill files **durable** — no ephemeral specifics (current clang
   version number, current list of build-dir contents, today's PR numbers). When a specific is
   needed to make a point, put it in the skill and note that it may drift.
4. After an edit here or in a skill, the change applies to **all future sessions** in this
   repo. Do not silently override a durable rule for a single session without saying so.
5. Do not retroactively edit this file or skills to match what an agent did this session unless
   the user explicitly asks. Behavior that the user did not endorse does not become a rule.

If a rule in this file conflicts with what the user says in the current session, the user wins
for this session — and if the conflict is intentional and durable, update this file.

---

## 13. When in doubt

- Re-read the user's last few messages. They usually already said what they want.
- Prefer running a tool over describing what the tool would output.
- If the user pastes an error and you can reproduce it, reproduce it before diagnosing.
- If you are about to write a kernel feature or libc function, stop. The user writes those.
- "just build" is a sentence, not a suggestion.
