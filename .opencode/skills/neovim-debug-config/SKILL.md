---
name: neovim-debug-config
description: .nvim debug schema, leader-be/bO/br mappings, predebug background script, project-local debug overrides.
---

Applies when the user asks you to touch the neovim debug setup, fix a `<leader>b*` mapping,
or add a project-local debug override.

## Layout

- Global nvim config: `~/.config/nvim/`. The nvim-specific guidance that lives outside this
  repo is in `~/.config/nvim/AGENTS.md` — read it.
- Project-local debug schema: `.nvim/` at the repo root, gitignored. **Do not use Glob or
  Grep on `.nvim/`** — they skip gitignored files. Use the Read tool or `ls`.

## Loading model

The debug plugin loads project-local overrides from project files with a fallback to the
default. Don't hardcode debug args (executable path, program arguments, debug command) into
the plugin's source — pull them from the project file, fall back to the global default when
absent. The user has stated this multiple times: a new debug target should be settable from
the repo without touching shared config.

## The `<leader>b*` surface

There's a default debug target the plugin knows how to launch. There's also a per-target
override slot: the user enters a debug command (e.g. a custom executable path or test binary),
saves it (saved ≠ launched — saving just records the override and enables the launch-override
mapping), then launches via the override mapping. When the override slot is empty and the
user tries to launch-with-override, the plugin should say "go enter the command first" and
point at the enter-command mapping — not silently fall through.

The run-from-justfile mapping should run the justfile `run` recipe, **not** whatever the user
typed into the enter-command slot. If you find those two got tangled, untangle them — the
user has caught this exact mistake.

## predebug

A `predebug` config field that runs *something* in the background before the debug session
starts. It can be a one-shot command or a **script** — the user explicitly noted the script
case. The predebug process is **terminated when the debug session ends**, not left orphaned.
Model the lifecycle that way in any code you write.

## When you change something

- After touching the debug schema or mappings, run the build / start a debug session /
  actually verify the wiring — the user rejects "should work now" without proof, doubly so
  for nvim lua config where a typo silently breaks every mapping.
- Don't rewrite the entire nvim debug plugin when the user asked for one new mapping or one
  new fallback path. Targeted edits; the plugin is shared infrastructure.
- If you're adding support for a new debug target (kernel, a libc test binary, etc.), make it
  settable from the project-local file, not hardcoded into the plugin.
