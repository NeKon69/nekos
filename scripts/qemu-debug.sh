#!/usr/bin/env bash
set -euo pipefail

profile="${1:-Debug}"
name="${2:-nekos}"
build_dir="build_${profile}"
iso_path="${build_dir}/${name}.iso"

exec qemu-system-i386 \
    -cdrom "${iso_path}" \
    -S \
    -gdb tcp::1234 \
    -no-reboot \
    -no-shutdown
