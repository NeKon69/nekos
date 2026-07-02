file(GLOB_RECURSE LIBC_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_library(c STATIC ${LIBC_SOURCES})
target_include_directories(c PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_options(c PRIVATE ${KERNEL_C_FLAGS})

set(LIBC_TEST_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_unity_tests(libc "${CMAKE_CURRENT_SOURCE_DIR}/tests" LIBC_SOURCES LIBC_TEST_INCLUDES)
