---
name: bit-packing-descriptors
description: GDT/IDT descriptor encoding, constexpr bit-packing helpers, shift-overflow, shared-bit conflicts.
---

Applies when the user is writing GDT or IDT descriptor entry encoding helpers, or hits a
`-Wshift-count-overflow` / `-Wconstant-conversion` warning in one of those helpers.

## The design the user settled on

The user prefers small `constexpr` bit-packing helpers that take a `bool` and return the
packed byte/field, combinable with `|` at the call site, instead of the osdev wiki's
`#define SEG_FOO(x) ((x) << N)` macro block. Keep the `constexpr` helper style. Don't port
the macro block back.

Why:
- `constexpr` over `#define` is a project rule (see `AGENTS.md` section 6).
- One bool in, one field out, `|`-able with siblings — readable at the call site.
- Lives in the source file next to the descriptor builder, not in a header.

## Sharp edges to remember

- **Bit width.** These helpers return `uint8_t`. A shift count ≥ 8 is UB and clang warns
  (`-Wshift-count-overflow`, `-Wconstant-conversion` when the value is a known constant).
  The access byte is bits 40-47 of the descriptor (8 bits, fits `uint8_t`). The flags byte
  is bits 52-55 (4 bits, fits `uint8_t`). The **full flags field as osdev sometimes defines
  it** (DESCTYPE/SAVL/LONG/SIZE/GRAN, bits 52-63) is 12 bits and does **not** fit `uint8_t` —
  if you copy an osdev helper that shifts into bits 12-15 of a 16-bit flags field, you'll get
  the warning. Match the field width to the return type; if the field is wider, return
  `uint16_t`, not `uint8_t`.
- **Shared bits.** In the flags byte, bit 4 is "descriptor type (0=system, 1=data/code)"
  and bit 4 is *also* repurposed as "available" in some osdev tables — read the actual bit
  table the user is working from before merging two helpers into one. The user has hit this
  ("how is that possible though") and it's not a bug, it's the same bit with two
  interpretations depending on which table you're reading. Don't collapse two helpers into
  one without checking the bit table the user is using.
- **Returning bool shifts.** `return Value << N;` where `Value` is `bool` is fine; clang
  promotes to `int` first. Don't write `return Value ? 1 << N : 0;` — the user has corrected
  that as not cleaner; bit-shifting a bool is fine.

## The 64-bit entry builder

GDT and IDT entries are 64 bits. The user's builder takes a small entry struct and packs it
into a `uint64_t` with `|=` and shifts, documenting each chunk with which bits it maps to.
Don't reimplement it with a byte array or a separate register-style struct unless the user
explicitly asks — the user has rejected both alternatives and went with the single `uint64_t`
return.

## When the user is learning a bit field

The user often pastes an osdev bit table and asks "explain each entry". Walk through every
row and explain the *meaning* (what "system" means, what "present" means, what "available"
means), not just the bit position. The user has corrected reviewers who copied the table
verbatim ("you did not explain all of them!! explain from the start please") — they want
the concept, not the chart.

## IDT parity

IDT gate descriptors are also 64 bits and have the same access-byte/flags shape (with
gatetype, DPL, P in there). The same helper style applies; if the user asks for IDT helpers,
mirror the GDT helper pattern they already wrote rather than introducing a new style.
