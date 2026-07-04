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
bochs_rc="${build_dir}/bochsrc-kernel-test.rc"
bios_image="/usr/share/bochs/BIOS-bochs-latest"
vga_bios_image=""

# shellcheck source=kernel-test-common.sh
source "$(dirname "${BASH_SOURCE[0]}")/kernel-test-common.sh"

command -v bochs >/dev/null 2>&1 || die "Missing required command: bochs"
[[ -f "${iso_path}" ]] || die "Missing ISO: ${iso_path}"

shopt -s nullglob
for candidate in \
    /usr/share/bochs/VGABIOS-lgpl-latest.bin \
    /usr/share/vgabios/vgabios.bin \
    /usr/share/bochs/vgabios.bin* \
    /usr/share/bochs/VGABIOS-lgpl-latest*.bin \
    /usr/share/bochs/vgabios*.bin* \
    /usr/share/vgabios/vgabios*.bin*; do
    case "${candidate}" in
        *banshee*|*cirrus*|*debug*)
            continue
            ;;
    esac

    if [[ -f "${candidate}" ]]; then
        vga_bios_image="${candidate}"
        break
    fi
done

if [[ -z "${vga_bios_image}" ]]; then
    for candidate in \
        /usr/share/bochs/VGABIOS-lgpl-latest*.bin \
        /usr/share/bochs/vgabios*.bin* \
        /usr/share/vgabios/vgabios*.bin*; do
        if [[ -f "${candidate}" ]]; then
            vga_bios_image="${candidate}"
            break
        fi
    done
fi
shopt -u nullglob

[[ -f "${bios_image}" ]] || die "Missing Bochs BIOS image: ${bios_image}"
[[ -n "${vga_bios_image}" ]] || die "Missing Bochs VGA BIOS image. Checked /usr/share/bochs and /usr/share/vgabios."

rm -f "${serial_log}" "${bochs_log}" "${bochs_config}" "${bochs_rc}"

cat >"${bochs_config}" <<EOF
megs: 128
cpu: count=1, ips=10000000, reset_on_triple_fault=0
romimage: file=${bios_image}
vgaromimage: file=${vga_bios_image}
boot: cdrom
ata0-master: type=cdrom, path=${iso_path}, status=inserted
com1: enabled=1, mode=file, dev=${serial_log}
config_interface: textconfig
display_library: term
speaker: enabled=0
log: ${bochs_log}
panic: action=fatal
error: action=report
info: action=ignore
debug: action=ignore
EOF

for _ in {1..1000}; do
    printf 'c\n'
done >"${bochs_rc}"

TERM=xterm bochs -q -f "${bochs_config}" -rc "${bochs_rc}" &
bochs_pid=$!

info_box "Kernel Bochs tests" \
    "ISO|${iso_path}" \
    "Serial log|${serial_log}" \
    "Bochs log|${bochs_log}" \
    "Timeout|${timeout_seconds}s" \
    "Bochs PID|${bochs_pid}"

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
        printf "${C_YELLOW}${C_BOLD}! Kernel Bochs tests stopped before reporting a result.${C_RESET}\n" >&2
        if [[ -f "${serial_log}" ]]; then
            printf "${C_DIM}\n  ── serial log ──${C_RESET}\n" >&2
            cat "${serial_log}" >&2
        else
            printf "${C_DIM}  Serial log was not created.${C_RESET}\n" >&2
        fi
        if [[ -f "${bochs_log}" ]]; then
            printf "${C_DIM}\n  ── bochs log ──${C_RESET}\n" >&2
            cat "${bochs_log}" >&2
        fi
        exit 1
    fi

    if [[ -f "${serial_log}" ]]; then
        if grep -q 'NEKOS ALL TESTS PASSED' "${serial_log}"; then
            printf "${C_GREEN}${C_BOLD}✓ Kernel Bochs tests passed.${C_RESET}\n"
            trap - EXIT INT TERM
            cleanup
            exit 0
        fi

        if grep -q 'NEKOS TESTS FAILED' "${serial_log}"; then
            printf "${C_RED}${C_BOLD}✗ Kernel Bochs tests failed.${C_RESET}\n" >&2
            printf "${C_DIM}\n  ── serial log ──${C_RESET}\n" >&2
            cat "${serial_log}" >&2
            if [[ -f "${bochs_log}" ]]; then
                printf "${C_DIM}\n  ── bochs log ──${C_RESET}\n" >&2
                cat "${bochs_log}" >&2
            fi
            exit 1
        fi
    fi

    sleep 0.1
done

printf "${C_YELLOW}${C_BOLD}⏱ Kernel Bochs tests timed out after %ss.${C_RESET}\n" "${timeout_seconds}" >&2
if [[ -f "${serial_log}" ]]; then
    printf "${C_DIM}\n  ── serial log ──${C_RESET}\n" >&2
    cat "${serial_log}" >&2
else
    printf "${C_DIM}  Serial log was not created.${C_RESET}\n" >&2
fi

if [[ -f "${bochs_log}" ]]; then
    printf "${C_DIM}\n  ── bochs log ──${C_RESET}\n" >&2
    cat "${bochs_log}" >&2
fi

exit 1
