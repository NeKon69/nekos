#!/usr/bin/env bash
# Shared helpers for kernel test scripts.

if [[ -t 1 || -n "${CI:-}" || -n "${GITHUB_ACTIONS:-}" ]]; then
    C_RESET='\033[0m'; C_BOLD='\033[1m'; C_DIM='\033[2m'
    C_RED='\033[31m'; C_GREEN='\033[32m'; C_YELLOW='\033[33m'; C_CYAN='\033[36m'
else
    C_RESET=''; C_BOLD=''; C_DIM=''; C_RED=''; C_GREEN=''; C_YELLOW=''; C_CYAN=''
fi

repeat() { local s=""; while (( ${#s} < $2 )); do s+="$1"; done; printf '%s' "$s"; }

info_box() {
    local title="$1"; shift
    local max_label=0 max_value=0
    local labels=() values=()
    for row in "$@"; do
        local label="${row%%|*}" value="${row#*|}"
        labels+=("$label"); values+=("$value")
        (( ${#label} > max_label )) && max_label=${#label}
        (( ${#value} > max_value )) && max_value=${#value}
    done
    local width=$((max_label + max_value + 4))

    printf "${C_DIM}╭─${C_RESET} ${C_BOLD}${C_CYAN}%s${C_RESET} ${C_DIM}%s╮${C_RESET}\n" \
        "$title" "$(repeat '─' "$width")"

    for i in "${!labels[@]}"; do
        printf "${C_DIM}│${C_RESET} ${C_DIM}%-*s${C_RESET}: %s\n" \
            "$max_label" "${labels[$i]}" "${values[$i]}"
    done

    printf "${C_DIM}╰${C_RESET}\n"
}

die() {
    printf "${C_RED}${C_BOLD}✗ %s${C_RESET}\n" "$*" >&2
    exit 1
}
