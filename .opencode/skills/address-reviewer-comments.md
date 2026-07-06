---
description: Refactoring etiquette when applying reviewer or PR feedback; cast conventions; preserving file headers; not over-editing.
---

# address-reviewer-comments

Applies when the user asks you to "address codex's review / the comments on the PR", "apply
the review feedback", "fix the reviewer notes", etc.

## Before editing

1. Read the actual PR threads (or whatever comments the user pointed at). Don't summarize the
   feedback from memory after a glance — the user has caught this ("you did not read the new
   suggestions").
2. Identify which comments you're supposed to address and which to leave. The user often says
   "address all the comments on my PR (aside from the last one)" — meaning skip one explicitly.
3. Skim the actual current source state before changing it; a comment may already be moot
   after a later commit.

## Conventions when editing

- **Cast style:** `reinterpret_cast<T>(x)` over C-style `(T)(x)`. The user has said this
  multiple times ("use reinterpret cast instead?", "can you not reinterpret cast it to
  uint32_t from the start?"). Applies to pointer casts especially.
- **Naming:** follow the project's naming conventions (see `AGENTS.md` section 4). The user
  has rejected edits that drifted into a different case style ("follow naming convention
  please").
- **File headers:** preserve the existing comment block at the top of files you edit — don't
  rewrite, reorder, or strip it just because you're in there.
- **Targeted edits:** use `sed`/`perl`/targeted Edit for the change the reviewer asked for.
  Do not reformat the file, do not rename unrelated symbols, do not fix unrelated warnings
  in the same edit. The user has rejected "stop changing this!!!" repeatedly.
- **Don't redo work the user already accepted.** If a function's impl was just adjusted
  between rounds of review, don't "improve" it again — only do what the new comments ask.

## After editing

- Run `just build` (and `just test` if the changed code is covered by tests) before saying
  you're done. The user rejects "should be fine" claims without proof, doubly so after a
  reviewer already filed a comment on the same line.
- Paste the failing build output verbatim if it breaks; don't paraphrase.

## Reviewer-specific notes

- If the reviewer (your past review, Codex's review, the user's own comment) reply chain is
  going on, **reply to the latest comment first** before doing anything else (see
  `reviewer-workflow` skill).
- "Reviewer said X is UB" doesn't mean you should add a test that exercises the UB — note it
  and move on. Add a *normal* test if one is missing.
- If the user said "i brought back the github version locally. cause i left it by accident",
  ignore the bot's earlier suggestion entirely — the codebase state the bot reviewed no longer
  exists. Don't carry that feedback forward.