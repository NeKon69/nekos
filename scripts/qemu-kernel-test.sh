#!/usr/bin/env bash
set -euo pipefail

profile="${1:-Debug}"
name="${2:-nekos}"
timeout_seconds="${3:-10}"

build_dir="build_${profile}"
iso_path="${build_dir}/${name}.iso"
serial_log="${build_dir}/kernel-test-serial.log"

if [[ ! -f "${iso_path}" ]]; then
    printf 'Missing ISO: %s\n' "${iso_path}" >&2
    exit 1
fi

rm -f "${serial_log}"

printf 'Kernel QEMU tests\n'
printf '  ISO:        %s\n' "${iso_path}"
printf '  Serial log: %s\n' "${serial_log}"
printf '  Timeout:    %ss\n' "${timeout_seconds}"

qemu-system-i386 \
    -cdrom "${iso_path}" \
    -serial "file:${serial_log}" \
    -display none \
    -no-reboot \
    -no-shutdown &

qemu_pid=$!
printf '  QEMU PID:   %s\n' "${qemu_pid}"

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
        printf 'Kernel QEMU tests stopped before reporting a result.\n' >&2
        if [[ -f "${serial_log}" ]]; then
            printf '\n--- serial log ---\n' >&2
            cat "${serial_log}" >&2
        else
            printf 'Serial log was not created.\n' >&2
        fi
        exit 1
    fi

    if [[ -f "${serial_log}" ]]; then
        if grep -q 'NEKOS ALL TESTS PASSED' "${serial_log}"; then
            printf 'Kernel QEMU tests passed.\n'
            exit 0
        fi

        if grep -q 'NEKOS TESTS FAILED' "${serial_log}"; then
            printf 'Kernel QEMU tests failed.\n' >&2
            printf '\n--- serial log ---\n' >&2
            cat "${serial_log}" >&2
            exit 1
        fi
    fi

    sleep 0.1
done

printf 'Kernel QEMU tests timed out after %s seconds.\n' "${timeout_seconds}" >&2
if [[ -f "${serial_log}" ]]; then
    printf '\n--- serial log ---\n' >&2
    cat "${serial_log}" >&2
else
    printf 'Serial log was not created.\n' >&2
fi
exit 1
