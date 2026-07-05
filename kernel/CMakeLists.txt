file(GLOB_RECURSE KERNEL_CXX_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

option(NEKOS_KERNEL_TESTS "Build kernel test harness into the kernel" OFF)

if(NEKOS_KERNEL_TESTS OR CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    set(NEKOS_KERNEL_TESTS_EFFECTIVE ON)
    file(GLOB_RECURSE KERNEL_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )
else()
    set(NEKOS_KERNEL_TESTS_EFFECTIVE OFF)
endif()

file(GLOB_RECURSE KERNEL_ASM_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.asm"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.s"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.S"
)

set(IDT_VECTORS_FILE "${CMAKE_CURRENT_BINARY_DIR}/kernel/include/idt_vectors.inc")

add_custom_command(OUTPUT ${IDT_VECTORS_FILE}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/kernel/include"
    COMMAND bash -c "printf 'X(%d) ' {0..255} > '${IDT_VECTORS_FILE}'"
    COMMENT "Generating IDT vector list"
    VERBATIM
)

set(KERNEL_SOURCES ${KERNEL_CXX_SOURCES} ${KERNEL_ASM_SOURCES} ${KERNEL_TEST_SOURCES} ${IDT_VECTORS_FILE})

add_executable(kernel ${KERNEL_SOURCES})
set_target_properties(kernel PROPERTIES
    OUTPUT_NAME "nekos.elf"
    LINKER_LANGUAGE C
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

foreach(flag IN LISTS KERNEL_C_FLAGS)
    target_compile_options(kernel PRIVATE "$<$<COMPILE_LANGUAGE:C>:${flag}>")
endforeach()

foreach(flag IN LISTS KERNEL_CXX_FLAGS)
    target_compile_options(kernel PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${flag}>")
endforeach()

target_compile_options(kernel PRIVATE
    "$<$<COMPILE_LANGUAGE:ASM>:--target=${KERNEL_TARGET}>"
)

target_include_directories(kernel PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/kernel/include"
)

if(NEKOS_KERNEL_TESTS_EFFECTIVE)
    target_include_directories(kernel PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/tests"
    )
endif()

target_compile_definitions(kernel PRIVATE
    "$<$<COMPILE_LANGUAGE:C>:NEKOS_KERNEL_TESTS=$<BOOL:${NEKOS_KERNEL_TESTS_EFFECTIVE}>>"
    "$<$<COMPILE_LANGUAGE:CXX>:NEKOS_KERNEL_TESTS=$<BOOL:${NEKOS_KERNEL_TESTS_EFFECTIVE}>>"
)

target_link_libraries(kernel PRIVATE c)

target_link_options(kernel PRIVATE
    --target=${KERNEL_TARGET}
    -fuse-ld=lld
    -nostdlib
    -nostartfiles
    -nodefaultlibs
    -static
    -Wl,--no-pie
    -Wl,--build-id=none
)

set(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/linker.ld")

if(EXISTS "${LINKER_SCRIPT}")
    target_link_options(kernel PRIVATE "-T${LINKER_SCRIPT}")
    set_target_properties(kernel PROPERTIES LINK_DEPENDS "${LINKER_SCRIPT}")
endif()

set(SYSROOT_DIR "${CMAKE_BINARY_DIR}/sysroot")
add_custom_target(sysroot ALL
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${SYSROOT_DIR}/usr/include" "${SYSROOT_DIR}/usr/lib"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SYSROOT_DIR}/usr/include/c++/v1" "${SYSROOT_DIR}/usr/lib"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/libc/include" "${SYSROOT_DIR}/usr/include"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/libcxx/include" "${SYSROOT_DIR}/usr/include/c++/v1"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:c>" "${SYSROOT_DIR}/usr/lib/"
    DEPENDS c
)
