---
description: Generating PR title and description from a diff; following existing PR style; resolving rebase conflicts.
---

# pr-description-from-diff

Applies when the user asks you to "generate a description and name for this branch / commits /
diff", "create a PR", or to write a PR title following repo style.

## Before writing

1. Read `gh pr list --state all` first to see how prior PRs in this repo are titled. Follow
   the same style — verb form, capitalization, scope prefix if any. Don't invent a new
   convention.
2. Read the actual diff before writing. The user has caught agents writing a description from
   the commit message summary without looking at the diff and getting it wrong ("you have not
   viewed complete diff!!!"). Run `git --no-pager diff main...HEAD` (or the relevant base)
   and skim every hunk; if the diff is large, the description should still reflect what
   actually changed, not what the commit message claimed.
3. If the user pastes a function-list table (e.g. cppreference section listing all functions
  in a stdlib header), that's a hint about what they want mentioned in the description —
  typically "implemented X, Y, Z; not yet: A, B" — enumerate both halves.

## What to write

- A **title** matching prior PR style (short, imperative, scoped if the repo uses scopes).
- A **body** describing what changed and why. Length should match the scope of the PR; the
  user has rejected fixed length caps ("shouldn't limit it") because scope varies.
- If the PR adds tests, mentions of what's covered and what intentionally isn't (UB cases
  noted but not exercised, etc.) are useful.
- If there are unimplemented items left for follow-up, list them at the bottom so the user /
  reviewers see scope explicitly.

## Write to a file

When the user says "can you write that to a file", write the title + body to a file (usually
`/tmp/opencode` or wherever they direct) so they can paste it into `gh pr create`.

## Resolve conflicts cleanly

When the user asks you to "resolve the conflicts please" while creating the PR or rebasing:
- rebase / merge without `-i` (no interactive editors),
- resolve conflicts touching **only** the hunks in conflict — don't rewrite unrelated code,
  don't reformat the file while you're in there,
- don't introduce changes the user didn't ask for just because the editor was open.
- After resolving, `just build` (and `just test` if relevant) before saying done.