---
name: clang-lifetime-safety
description: Clang trunk lifetime-safety flags, NEKOS_LIFETIMEBOUND macro gating, fexperimental-lifetime-safety-c, lifetimebound attribute.
---

Applies when the user asks to add/enable/fix clang's experimental lifetime-safety warnings,
the `NEKOS_LIFETIMEBOUND` macro, `[[clang::lifetimebound]]` / `[[gsl::Pointer]]` /
`[[gsl::Owner]]` / `[[clang::lifetime_capture_by(...)]]` attributes, or `-Wunknown-attributes`
errors from those attributes on older clang.

## Why this is fragile

The flags are clang-trunk-only (clang 23+). CI runners may run an older clang that:
- doesn't know `-Wlifetime-safety-all`, `-Xclang -fexperimental-lifetime-safety-c`,
  `-Xclang -fexperimental-lifetime-safety-tu-analysis` at all, or
- knows the flags but emits `-Wunknown-attributes` on `[[clang::lifetimebound]]` in C because
  the attribute support landed later than the flag support.

So every flag and attribute here gets gated, not added unconditionally.

## Flag gating

In CMake, only append the lifetime-safety flags when
`CMAKE_C_COMPILER_ID STREQUAL Clang AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 23`
(or whatever threshold the user is on — check what they actually want before writing the
number). Same for CXX. Apply to the kernel and libc target flags; don't globally mutate
`CMAKE_C_FLAGS` because the host-side tests must link with the host stdlib.

## Attribute gating

`[[clang::lifetimebound]]` on a C function parameter should be replaced with the project
macro `NEKOS_LIFETIMEBOUND`, defined in a dedicated header. The macro expands to
`[[clang::lifetimebound]]` when `__has_c_attribute(clang::lifetimebound)` is true, and to
nothing otherwise. Same idea for the other lifetime annotations.

To retrofit `[[clang::lifetimebound]]` → `NEKOS_LIFETIMEBOUND` across existing headers: use
`sed`/`perl` over the affected files **only**, add the include once, and don't touch files the
user didn't ask you to touch. The user has rejected wholesale rewrites here repeatedly.

The `-Xclang=-lifetime-safety-lifetimebound-macro=NEKOS_LIFETIMEBOUND` flag tells the analyzer
to treat that macro name as the lifetimebound annotation marker, so the analyzer still sees
the annotation even when the macro expands to nothing on older clang.

## Looking up flag spellings

When the user is unsure of a flag name ("or smth like that, im not sure, check flags"),
**do not guess**. Look up the real spelling in the local LLVM source:
- `~/llvm-project/clang/include/clang/Basic/` for flag defs,
- `~/llvm-project/clang/docs/` for RST docs (the lifetime safety doc lives there),
- `~/llvm-project/clang/include/clang/Basic/AttrDocs.td` for attribute docs.

The machine may be offline; `man clang` and `clang --help=--hidden 2>&1 | grep -i lifetime`
also work locally.

## When the user is exploring

The user sometimes asks you to "go explore what this can detect, write your own test files in
`/tmp/opencode`, give me complete freedom". Do that — write standalone test files, compile
with the gated flags, report what the analyzer catches and misses. Corrections to your claims
("incorrect lol, use-after-free is still caught with new/delete only") are welcome; the user
enjoyed this. Don't dump the full AST tree; the user found that unhelpful.
