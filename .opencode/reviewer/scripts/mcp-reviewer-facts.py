#!/usr/bin/env python3
import pathlib
import subprocess

from mcp.server.fastmcp import FastMCP


SCRIPT_DIR = pathlib.Path(__file__).resolve().parent
mcp = FastMCP("reviewer-facts")


def run_script(name: str, args: list[str]) -> str:
    proc = subprocess.run(
        [str(SCRIPT_DIR / name), *args],
        check=False,
        text=True,
        capture_output=True,
        timeout=120,
    )
    output = "\n".join(part.strip() for part in (proc.stdout, proc.stderr) if part.strip())
    if proc.returncode != 0:
        return output or f"{name} failed"
    return output or f"{name} produced no output"


@mcp.tool()
def reviewer_facts_search(terms: list[str], limit: int = 10) -> str:
    """Search global reusable reviewer facts for this project.

    Use paths, symbols, subsystems, diagnostics, test names, or aliases. This
    database is not for current-review notes. During review, use this after you
    know the changed files/symbols for a nuanced subsystem; do not browse the
    whole database.
    """
    args = [str(term) for term in terms]
    args.extend(["--limit", str(limit)])
    return run_script("reviewer-facts-search", args)


@mcp.tool()
def reviewer_facts_add(
    kind: str,
    fact: str,
    review_uses: list[str],
    evidence: list[str],
    subsystems: list[str] | None = None,
    files: list[str] | None = None,
    symbols: list[str] | None = None,
    aliases: list[str] | None = None,
) -> str:
    """Append one global reusable reviewer fact.

    Use this during review wrap-up when the investigation found a fact that
    would save a future reviewer time on an unrelated patch. Kind should be one
    of: invariant, testing, workflow, gotcha, relationship, tooling. The fact
    must be reusable across future unrelated reviews.
    Do not record current-PR findings, temporary branch observations, or generic
    review advice.
    """
    args = ["--kind", kind, "--fact", fact]
    for value in subsystems or []:
        args.extend(["--subsystem", str(value)])
    for value in files or []:
        args.extend(["--file", str(value)])
    for value in symbols or []:
        args.extend(["--symbol", str(value)])
    for value in aliases or []:
        args.extend(["--alias", str(value)])
    for value in review_uses:
        args.extend(["--review-use", str(value)])
    for value in evidence:
        args.extend(["--evidence", str(value)])
    return run_script("reviewer-facts-add", args)


if __name__ == "__main__":
    mcp.run()
