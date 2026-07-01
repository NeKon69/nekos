# Auto-generated justfile from template: cpp_cmake.just

profile := "Debug"
name := "nekos"
build_dir := "build_" + profile
iso_dir := build_dir + "/iso"
kernel_elf := name + ".elf"
dap_bin := build_dir + "/" + name + ".iso"
dap_args := ""

build:
    #!/usr/bin/env bash
    cmake -B {{build_dir}} -DCMAKE_BUILD_TYPE={{profile}} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build {{build_dir}}
    cp {{build_dir}}/compile_commands.json .
    if grub-file --is-x86-multiboot {{build_dir}}/{{kernel_elf}}; then
        printf 'Multiboot OK\n'
    else
        printf 'Multiboot FAIL\n'
        exit 1
    fi

    rm -rf {{iso_dir}}
    mkdir -p {{iso_dir}}/boot/grub
    cp {{build_dir}}/{{kernel_elf}} {{iso_dir}}/boot/{{name}}
    cp grub.cfg {{iso_dir}}/boot/grub/grub.cfg
    grub-mkrescue -o {{dap_bin}} {{iso_dir}}

run: build
    qemu-system-i386 -cdrom {{dap_bin}}

test: build
    cd {{build_dir}} && ctest -C {{profile}} --output-on-failure

clean:
    rm -rf build_*

format:
    git ls-files '*.cpp' '*.h' '*.hpp' '*.c' | xargs clang-format -i
