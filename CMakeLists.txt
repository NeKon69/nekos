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

if(CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 23)
    list(APPEND KERNEL_C_FLAGS
        -Wlifetime-safety-all
        -Xclang=-lifetime-safety-lifetimebound-macro=NEKOS_LIFETIMEBOUND
        -Xclang=-fexperimental-lifetime-safety-c
        -Xclang=-fexperimental-lifetime-safety-tu-analysis
    )
endif()

set(KERNEL_CXX_FLAGS
    ${KERNEL_C_FLAGS}
    -fno-exceptions
    -fno-rtti
)

enable_testing()

include(cmake/AddUnityTests.cmake)

add_subdirectory(libc)
add_subdirectory(libcxx)
add_subdirectory(kernel)
