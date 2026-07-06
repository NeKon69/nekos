# Reviewer Knowledge

This directory stores global, reusable reviewer knowledge for this NekOS
workspace. It is not a log for the current review.

Use this database when it seems relevant to the code under review, especially
for unfamiliar subsystems, surprising invariants, non-obvious test locations,
or review gotchas that may apply across future branches.

Do not read the whole database by default. Prefer:

```bash
.opencode/reviewer/scripts/reviewer-facts-search <terms...>
```

## What Belongs Here

Record facts that are likely useful across unrelated future reviews:

- Non-obvious subsystem invariants.
- Relationships between files, symbols, tests, or generated output.
- Focused testing commands or flags that are easy to miss.
- Workflow gotchas specific to NekOS review in this checkout.
- Hardware-specific quirks (x86 port I/O, interrupt safety, boot protocol).
- Repeated traps found after reading code, tests, or PR discussion.

Do not record:

- Findings specific to the current branch or PR.
- Temporary behavior from a half-built or mixed checkout.
- Generic review advice.
- Guesses without code, test, or command evidence.

## Data Format

Facts are stored in `facts.jsonl`, one JSON object per line. Required fields:

- `kind`: one of `invariant`, `testing`, `workflow`, `gotcha`, `relationship`, `tooling`.
- `subsystems`: array of broad subsystem names.
- `files`: array of relevant file paths.
- `symbols`: array of relevant functions, classes, or test names.
- `aliases`: array of natural-language search terms.
- `fact`: concise statement of the reusable fact.
- `review_uses`: array explaining how future reviewers should apply it.
- `evidence`: array explaining why the fact is trusted.

Optional fields such as `id`, `created_at`, and `created_by` may be filled by
the add helper.
