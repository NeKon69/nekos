#!/usr/bin/env python3
import base64
import json
import os
import pathlib
import re
import subprocess
import time
import urllib.error
import urllib.request

from mcp.server.fastmcp import FastMCP


SCRIPT_DIR = pathlib.Path(__file__).resolve().parent
mcp = FastMCP("review-bot")

# --- GitHub App auth ---

_gh_token: str | None = None
_token_expires: float = 0
_app_token_refresh_lock = False


def _b64(data: bytes) -> str:
    return base64.urlsafe_b64encode(data).rstrip(b"=").decode()


def _make_jwt(app_id: str, key_path: str) -> str:
    header = _b64(json.dumps({"alg": "RS256", "typ": "JWT"}).encode())
    now = int(time.time())
    payload = _b64(json.dumps({"iat": now - 60, "exp": now + 600, "iss": app_id}).encode())
    msg = f"{header}.{payload}".encode()
    proc = subprocess.run(
        ["openssl", "dgst", "-sha256", "-sign", key_path],
        input=msg, capture_output=True, check=True, timeout=30,
    )
    sig = _b64(proc.stdout)
    return f"{header}.{payload}.{sig}"


def _gh_api_jwt(jwt: str, method: str, path: str, body: bytes | None = None) -> dict | None:
    url = f"https://api.github.com/{path.lstrip('/')}"
    headers = {
        "Authorization": f"Bearer {jwt}",
        "Accept": "application/vnd.github+json",
        "User-Agent": "review-bot",
    }
    req = urllib.request.Request(url, data=body, headers=headers, method=method)
    try:
        resp = urllib.request.urlopen(req, timeout=30)
        return json.loads(resp.read())
    except urllib.error.HTTPError:
        return None


def _get_installation_token() -> str | None:
    app_id = os.environ.get("GH_APP_ID")
    key_path = os.environ.get("GH_APP_PRIVATE_KEY_PATH")
    if not app_id or not key_path:
        return None
    key_path = os.path.expanduser(key_path)
    if not os.path.isfile(key_path):
        return None

    jwt = _make_jwt(app_id, key_path)
    owner, repo = _get_repo()
    if not owner or not repo:
        return None

    inst = _gh_api_jwt(jwt, "GET", f"repos/{owner}/{repo}/installation")
    if not inst:
        return None
    inst_id = inst.get("id")
    if not inst_id:
        return None

    result = _gh_api_jwt(jwt, "POST", f"app/installations/{inst_id}/access_tokens", body=b"{}")
    if not result:
        return None
    return result.get("token")


def _ensure_token() -> str | None:
    global _gh_token, _token_expires
    if _gh_token and time.time() < _token_expires - 60:
        return _gh_token
    token = _get_installation_token()
    if token:
        _gh_token = token
        _token_expires = time.time() + 3600
    return _gh_token


def _gh_env() -> dict[str, str]:
    env = os.environ.copy()
    token = _ensure_token()
    # If token_exchanged is truthy, we have an app token; set GH_TOKEN to it.
    # If token is None but GH_APP_ID is set, the app auth failed — fall through.
    if token:
        env["GH_TOKEN"] = token
    return env


# --- Repo detection ---

def _get_repo():
    proc = subprocess.run(
        ["git", "remote", "get-url", "origin"],
        check=False, text=True, capture_output=True, timeout=10,
    )
    url = proc.stdout.strip()
    match = re.search(r"(?:github\.com[/:])([\w.-]+)/([\w.-]+?)(?:\.git)?$", url)
    if match:
        return match.group(1), match.group(2).removesuffix(".git")
    return None, None


def gh_api(method: str, endpoint: str, body: dict | None = None) -> dict:
    cmd = ["gh", "api", endpoint, "--method", method]
    input_data = None
    if body is not None:
        input_data = json.dumps(body)
        cmd.extend(["--input", "-"])
    proc = subprocess.run(
        cmd, input=input_data, env=_gh_env(),
        check=False, text=True, capture_output=True, timeout=120,
    )
    if proc.returncode != 0:
        return {"error": (proc.stderr or proc.stdout or f"gh api {endpoint} failed").strip()}
    try:
        data = json.loads(proc.stdout) if proc.stdout.strip() else {}
        return {"data": data}
    except json.JSONDecodeError:
        return {"data": {"raw": proc.stdout.strip()}}


def gh_api_paginate(endpoint: str) -> list:
    results = []
    page = 1
    per_page = 100
    env = _gh_env()
    while True:
        sep = "&" if "?" in endpoint else "?"
        url = f"{endpoint}{sep}per_page={per_page}&page={page}"
        proc = subprocess.run(
            ["gh", "api", url],
            env=env, check=False, text=True, capture_output=True, timeout=120,
        )
        if proc.returncode != 0:
            break
        try:
            page_data = json.loads(proc.stdout)
        except json.JSONDecodeError:
            break
        if not isinstance(page_data, list):
            if page == 1:
                return page_data if isinstance(page_data, list) else []
            break
        if not page_data:
            break
        results.extend(page_data)
        page += 1
    return results


def _format_review_threads(pr_number: int, comments: list) -> str:
    roots = {}
    replies = {}
    for c in comments:
        cid = c.get("id", 0)
        parent = c.get("in_reply_to_id")
        if parent:
            replies.setdefault(parent, []).append(c)
        else:
            roots[cid] = c

    lines = [f"# Review Threads for PR #{pr_number}"]

    if not roots:
        lines.append("")
        lines.append("No review threads found.")
        return "\n".join(lines)

    thread_num = 0
    for cid in sorted(roots):
        thread_num += 1
        c = roots[cid]
        path = c.get("path", "")
        line = c.get("line", "") or c.get("position", "")
        user = c.get("user", {}).get("login", "?")
        body = c.get("body", "")
        created = c.get("created_at", "")
        reactions = c.get("reactions", {})

        loc = f"{path}:{line}" if path else "(no file)"
        lines.append("")
        lines.append(f"### Thread {thread_num}: {user} — {loc}")
        lines.append(f"Comment ID: {cid}")
        lines.append(f"Author: {user}")
        lines.append(f"Created: {created}")
        lines.append(f"Reactions: {reactions.get('summary', {}).get('total_count', 0) if isinstance(reactions, dict) else 'N/A'}")
        lines.append("")
        for bl in body.strip().split("\n"):
            lines.append(f"> {bl}")

        for r in replies.get(cid, []):
            r_id = r.get("id", "?")
            r_user = r.get("user", {}).get("login", "?")
            r_body = r.get("body", "")
            r_created = r.get("created_at", "")

            lines.append("")
            lines.append(f"  Reply by {r_user} (comment_id: {r_id}, {r_created}):")
            for rl in r_body.strip().split("\n"):
                lines.append(f"  > {rl}")

    lines.append("")
    return "\n".join(lines)


@mcp.tool()
def review_submit(
    pr_number: int,
    body: str,
    event: str = "COMMENT",
    line_comments: list | None = None,
) -> str:
    """Submit a complete PR review with summary and optional inline comments.

    Posts as an actual GitHub PR review (not a single comment). Use this
    at the end of your analysis to post all findings at once.

    Args:
        pr_number: GitHub PR number.
        body: Top-level review summary / description (shown at top of review).
        event: "COMMENT" (neutral), "APPROVE" (approve), or "REQUEST_CHANGES" (request changes).
        line_comments: Optional list of inline comments, each with:
            "path" (file path relative to repo root),
            "line" (line number in new file, or end line for a range),
            "start_line" (optional start line for a multi-line range),
            "body" (comment text).
    """
    owner, repo = _get_repo()
    if not owner or not repo:
        return "error: could not detect GitHub repo (origin remote)"

    event_upper = event.upper().replace(" ", "_")
    if event_upper not in ("COMMENT", "APPROVE", "REQUEST_CHANGES"):
        return f"error: invalid event '{event}'. Use COMMENT, APPROVE, or REQUEST_CHANGES."

    payload = {
        "body": body,
        "event": event_upper,
    }

    if line_comments:
        api_comments = []
        for lc in line_comments:
            path = lc.get("path", "").strip()
            line = lc.get("line", 0)
            text = lc.get("body", "").strip()
            if not path or not text:
                continue
            entry = {
                "path": path,
                "body": text,
            }
            if line:
                entry["line"] = int(line)
                entry["side"] = "RIGHT"
            start_line = lc.get("start_line", 0)
            if start_line:
                entry["start_line"] = int(start_line)
                entry["start_side"] = "RIGHT"
            api_comments.append(entry)
        if api_comments:
            payload["comments"] = api_comments

    result = gh_api("POST", f"repos/{owner}/{repo}/pulls/{pr_number}/reviews", payload)
    if "error" in result:
        return f"Review submission failed: {result['error']}"
    rid = result.get("data", {}).get("id", "?")
    return f"Review #{rid} submitted successfully ({event_upper})."


@mcp.tool()
def review_threads(pr_number: int) -> str:
    """Get all existing review threads on a PR, grouped by conversation.

    Call this when you need to see previous feedback, replies to your
    comments, or ongoing discussions before posting a new review.
    Returns formatted text with each thread's comments and metadata.
    """
    owner, repo = _get_repo()
    if not owner or not repo:
        return "error: could not detect GitHub repo (origin remote)"

    comments = gh_api_paginate(f"repos/{owner}/{repo}/pulls/{pr_number}/comments")
    if not comments:
        return f"No review comments found on PR #{pr_number}."

    return _format_review_threads(pr_number, comments)


@mcp.tool()
def kernel_compile_test(
    source: str,
    output_name: str | None = None,
    timeout_seconds: int | None = None,
    dry_run: bool = False,
    clean: bool = False,
) -> str:
    """Build a temporary kernel-test ISO, run it in QEMU, and return serial output.

    The source is copied into a temporary kernel test tree and must define
    `namespace tests { void runReviewBotTests(); }`. Only that function is
    run by the temporary test harness. Use kernel test helpers from `test.hpp`,
    such as `NEKOS_EXPECT_TRUE`, inside that function. Use `tests::log()` for
    progress messages; `kprintf()` writes to VGA and is not included in the
    serial output returned by this tool.

    Tool usage:
    `kernel_compile_test(source="/tmp/opencode/mytest.cpp", output_name="mytest")`

    `output_name` controls the temporary work directory name. `dry_run` shows
    the generated build/QEMU commands without compiling or booting. `clean`
    removes the temporary work directory after a successful run.

    Place the test source file under /tmp/opencode first, then pass its path.

    Args:
        source: Absolute path to the source file (e.g. /tmp/opencode/mytest.cpp).
        output_name: Optional work-directory name (defaults to the source filename stem).
        timeout_seconds: Optional QEMU result timeout in seconds (defaults to 10).
        dry_run: If true, prepare the temp tree and print the commands without building/running.
        clean: If true, delete the temp work directory after a successful run.
    """
    script = str(SCRIPT_DIR / "kernel-compile-test")
    args = [script]
    if timeout_seconds is not None:
        args += ["--timeout", str(timeout_seconds)]
    if dry_run:
        args.append("--dry-run")
    if clean:
        args.append("--clean")
    args.append(source)
    if output_name:
        args.append(output_name)

    proc = subprocess.run(
        args,
        capture_output=True, text=True, check=False, timeout=300,
    )
    output = proc.stdout.strip() + "\n" + proc.stderr.strip()
    output = output.strip()

    if proc.returncode != 0:
        return f"Kernel QEMU test failed:\n{output}"
    return f"Kernel QEMU test passed:\n{output}"


@mcp.tool()
def review_reply(
    pr_number: int,
    comment_id: int,
    body: str,
) -> str:
    """Reply to an existing review thread.

    Use this to respond to human feedback on your previous review comments.
    The reply will be added to the same thread as the comment you're replying to.

    Args:
        pr_number: GitHub PR number.
        comment_id: The ID of the comment to reply to (see review_threads output).
        body: The reply text.
    """
    owner, repo = _get_repo()
    if not owner or not repo:
        return "error: could not detect GitHub repo (origin remote)"

    payload = {
        "body": body,
        "in_reply_to": comment_id,
    }
    result = gh_api("POST", f"repos/{owner}/{repo}/pulls/{pr_number}/comments", payload)
    if "error" in result:
        return f"Reply failed: {result['error']}"
    cid = result.get("data", {}).get("id", "?")
    return f"Reply posted as comment #{cid}."


if __name__ == "__main__":
    mcp.run()
