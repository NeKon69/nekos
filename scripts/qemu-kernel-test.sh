#!/usr/bin/env bash
set -euo pipefail

profile="${1:-Debug}"
name="${2:-nekos}"
timeout_seconds="${3:-10}"

build_dir="build_${profile}"
iso_path="${build_dir}/${name}.iso"
serial_log="${build_dir}/kernel-test-serial.log"

# shellcheck source=kernel-test-common.sh
source "$(dirname "${BASH_SOURCE[0]}")/kernel-test-common.sh"

[[ -f "${iso_path}" ]] || die "Missing ISO: ${iso_path}"

rm -f "${serial_log}"

qemu-system-i386 \
    -cdrom "${iso_path}" \
    -serial "file:${serial_log}" \
    -display none \
    -no-reboot \
    -no-shutdown &

qemu_pid=$!

info_box "Kernel QEMU tests" \
    "ISO|${iso_path}" \
    "Serial log|${serial_log}" \
    "Timeout|${timeout_seconds}s" \
    "QEMU PID|${qemu_pid}"

cleanup() {
    if kill -0 "${qemu_pid}" 2>/dev/null; then
        kill "${qemu_pid}" 2>/dev/null || true
        wait "${qemu_pid}" 2>/dev/null || true
    fi
}

trap cleanup EXIT INT TERM

deadline=$((SECONDS + timeout_seconds))

while (( SECONDS < deadline )); do
    if ! kill -0 "${qemu_pid}" 2>/dev/null; then
        printf "${C_YELLOW}${C_BOLD}! Kernel QEMU tests stopped before reporting a result.${C_RESET}\n" >&2
        if [[ -f "${serial_log}" ]]; then
            printf "${C_DIM}\n  ── serial log ──${C_RESET}\n" >&2
            cat "${serial_log}" >&2
        else
            printf "${C_DIM}  Serial log was not created.${C_RESET}\n" >&2
        fi
        exit 1
    fi

    if [[ -f "${serial_log}" ]]; then
        if grep -q 'NEKOS ALL TESTS PASSED' "${serial_log}"; then
            printf "${C_GREEN}${C_BOLD}✓ Kernel QEMU tests passed.${C_RESET}\n"
            exit 0
        fi

        if grep -q 'NEKOS TESTS FAILED' "${serial_log}"; then
            printf "${C_RED}${C_BOLD}✗ Kernel QEMU tests failed.${C_RESET}\n" >&2
            printf "${C_DIM}\n  ── serial log ──${C_RESET}\n" >&2
            cat "${serial_log}" >&2
            exit 1
        fi
    fi

    sleep 0.1
done

printf "${C_YELLOW}${C_BOLD}⏱ Kernel QEMU tests timed out after %ss.${C_RESET}\n" "${timeout_seconds}" >&2
if [[ -f "${serial_log}" ]]; then
    printf "${C_DIM}\n  ── serial log ──${C_RESET}\n" >&2
    cat "${serial_log}" >&2
else
    printf "${C_DIM}  Serial log was not created.${C_RESET}\n" >&2
fi
exit 1
