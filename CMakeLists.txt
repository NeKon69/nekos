cmake_minimum_required(VERSION 3.20)

# Avoid CMake compiler checks that try to link against the host runtime.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER clang)
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER clang++)
endif()

project(nekos LANGUAGES C CXX ASM)

set(KERNEL_TARGET i686-elf)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(KERNEL_C_FLAGS
    --target=${KERNEL_TARGET}
    -ffreestanding
    -fno-builtin
    -fno-stack-protector
    -mno-sse
    -mno-sse2
    -mno-mmx
    -Wall
    -Wextra
)

set(KERNEL_CXX_FLAGS
    ${KERNEL_C_FLAGS}
    -fno-exceptions
    -fno-rtti
)

string(JOIN " " KERNEL_C_FLAGS_STRING ${KERNEL_C_FLAGS})
string(JOIN " " KERNEL_CXX_FLAGS_STRING ${KERNEL_CXX_FLAGS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${KERNEL_C_FLAGS_STRING}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KERNEL_CXX_FLAGS_STRING}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} --target=${KERNEL_TARGET}")

file(GLOB_RECURSE KERNEL_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/src/*.cc"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.asm"
    "${CMAKE_SOURCE_DIR}/src/*.s"
    "${CMAKE_SOURCE_DIR}/src/*.S"
)

add_executable(kernel ${KERNEL_SOURCES})
set_target_properties(kernel PROPERTIES
    OUTPUT_NAME "nekos.elf"
    LINKER_LANGUAGE C
)

target_include_directories(kernel PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_options(kernel PRIVATE
    --target=${KERNEL_TARGET}
    -fuse-ld=lld
    -nostdlib
    -nostartfiles
    -nodefaultlibs
    -static
    -no-pie
    -Wl,--build-id=none
)

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/linker.ld")
if(NOT EXISTS "${LINKER_SCRIPT}")
    set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/src/linker.ld")
endif()

if(EXISTS "${LINKER_SCRIPT}")
    target_link_options(kernel PRIVATE "-T${LINKER_SCRIPT}")
    set_target_properties(kernel PROPERTIES LINK_DEPENDS "${LINKER_SCRIPT}")
endif()
