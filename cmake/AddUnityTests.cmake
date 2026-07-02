option(NEKOS_HOST_TESTS_32BIT "Build host tests as 32-bit executables" ON)

set(HOST_TEST_COMPILE_OPTIONS -fno-builtin)
set(HOST_TEST_LINK_OPTIONS)
set(HOST_TEST_OUTPUT_DIR "${CMAKE_BINARY_DIR}/tests")

if(NEKOS_HOST_TESTS_32BIT)
    list(APPEND HOST_TEST_COMPILE_OPTIONS -m32)
    list(APPEND HOST_TEST_LINK_OPTIONS -m32)
endif()

add_library(test_unity STATIC
    "${PROJECT_SOURCE_DIR}/third_party/unity/src/unity.c"
)
set_target_properties(test_unity PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${HOST_TEST_OUTPUT_DIR}/lib"
)
target_compile_options(test_unity PRIVATE ${HOST_TEST_COMPILE_OPTIONS})
target_include_directories(test_unity PUBLIC
    "${PROJECT_SOURCE_DIR}/third_party/unity/src"
)

function(add_unity_tests suite_name test_dir sources_var includes_var)
    file(GLOB_RECURSE test_sources CONFIGURE_DEPENDS
        "${test_dir}/*_test.c"
        "${test_dir}/*_test.cc"
        "${test_dir}/*_test.cpp"
    )

    set(test_targets)

    foreach(test_source IN LISTS test_sources)
        file(RELATIVE_PATH test_name "${test_dir}" "${test_source}")
        string(REGEX REPLACE "\\.[^.]*$" "" test_name "${test_name}")
        string(REPLACE "/" "_" test_name "${test_name}")
        string(MAKE_C_IDENTIFIER "${suite_name}_${test_name}" test_target)

        add_executable(${test_target}
            "${test_source}"
            ${${sources_var}}
        )
        set_target_properties(${test_target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${HOST_TEST_OUTPUT_DIR}"
        )
        target_compile_options(${test_target} PRIVATE ${HOST_TEST_COMPILE_OPTIONS})
        target_link_options(${test_target} PRIVATE ${HOST_TEST_LINK_OPTIONS})
        target_include_directories(${test_target} PRIVATE ${${includes_var}})
        target_link_libraries(${test_target} PRIVATE test_unity)

        add_test(NAME ${test_target} COMMAND ${test_target})
        list(APPEND test_targets ${test_target})
    endforeach()

    add_custom_target(${suite_name}_tests DEPENDS ${test_targets})
endfunction()
