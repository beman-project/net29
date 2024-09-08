# src/beman/net29/tests/CMakeLists.txt
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

list(APPEND execution_tests
    sorted_list.pass
)

foreach(test ${execution_tests})
    add_executable(${test} ${test}.cpp)
    target_link_libraries(${test} PRIVATE ${TARGET_LIBRARY})
    add_test(NAME ${test} COMMAND $<TARGET_FILE:${test}>)
endforeach()
