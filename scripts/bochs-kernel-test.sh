#!/usr/bin/env bash
set -euo pipefail

profile="${1:-Debug}"
name="${2:-nekos}"
timeout_seconds="${3:-10}"

build_dir="build_${profile}"
iso_path="${build_dir}/${name}.iso"
serial_log="${build_dir}/kernel-test-bochs-serial.log"
bochs_log="${build_dir}/kernel-test-bochs.log"
bochs_config="${build_dir}/bochsrc-kernel-test"

if ! command -v bochs >/dev/null 2>&1; then
    printf 'Missing required command: bochs\n' >&2
    exit 1
fi

if [[ ! -f "${iso_path}" ]]; then
    printf 'Missing ISO: %s\n' "${iso_path}" >&2
    exit 1
fi

rm -f "${serial_log}" "${bochs_log}" "${bochs_config}"

cat >"${bochs_config}" <<EOF
megs: 128
romimage: file=/usr/share/bochs/BIOS-bochs-latest
vgaromimage: file=/usr/share/bochs/VGABIOS-lgpl-latest.bin
boot: cdrom
ata0-master: type=cdrom, path=${iso_path}, status=inserted
com1: enabled=1, mode=file, dev=${serial_log}
display_library: term
log: ${bochs_log}
panic: action=fatal
error: action=report
info: action=ignore
debug: action=ignore
EOF

printf 'Kernel Bochs tests\n'
printf '  ISO:        %s\n' "${iso_path}"
printf '  Config:     %s\n' "${bochs_config}"
printf '  Serial log: %s\n' "${serial_log}"
printf '  Bochs log:  %s\n' "${bochs_log}"
printf '  Timeout:    %ss\n' "${timeout_seconds}"

bochs -q -f "${bochs_config}" &
bochs_pid=$!
printf '  Bochs PID:  %s\n' "${bochs_pid}"

cleanup() {
    if kill -0 "${bochs_pid}" 2>/dev/null; then
        kill -KILL "${bochs_pid}" 2>/dev/null || true
        wait "${bochs_pid}" 2>/dev/null || true
    fi
}

trap cleanup EXIT INT TERM

deadline=$((SECONDS + timeout_seconds))

while (( SECONDS < deadline )); do
    if ! kill -0 "${bochs_pid}" 2>/dev/null; then
        printf 'Kernel Bochs tests stopped before reporting a result.\n' >&2
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
            printf 'Kernel Bochs tests passed.\n'
            cleanup
            exit 0
        fi

        if grep -q 'NEKOS TESTS FAILED' "${serial_log}"; then
            printf 'Kernel Bochs tests failed.\n' >&2
            printf '\n--- serial log ---\n' >&2
            cat "${serial_log}" >&2
            exit 1
        fi
    fi

    sleep 0.1
done

printf 'Kernel Bochs tests timed out after %s seconds.\n' "${timeout_seconds}" >&2
if [[ -f "${serial_log}" ]]; then
    printf '\n--- serial log ---\n' >&2
    cat "${serial_log}" >&2
else
    printf 'Serial log was not created.\n' >&2
fi

if [[ -f "${bochs_log}" ]]; then
    printf '\n--- bochs log ---\n' >&2
    cat "${bochs_log}" >&2
fi

exit 1
