# cmake-format: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

set(EXAMPLES
  http-server
  empty
  server
  client
  task
  cppcon-2024
)

foreach(EXAMPLE ${EXAMPLES})
  set(EXAMPLE_TARGET ${TARGET_PREFIX}.examples.${EXAMPLE})
  add_executable(${EXAMPLE_TARGET})
  target_sources(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE}.cpp)
  target_link_libraries(${EXAMPLE_TARGET} PRIVATE ${TARGET_LIBRARY})
endforeach()
