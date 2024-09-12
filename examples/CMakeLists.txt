# cmake-format: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

set(BEMAN_EXECUTION26_LIBRARY beman_execution26)
set(BEMAN_NET29_LIBRARY beman_net29)
set(LIBRARIES
    ${BEMAN_NET29_LIBRARY}
    ${BEMAN_EXECUTION26_LIBRARY}
)

include(GNUInstallDirs)

set(EXAMPLES
  server
  http-server
  client
  task
)

set(DATA
   index.html
   favicon.ico
)

foreach(FILE ${DATA})
  file(GENERATE OUTPUT data/${FILE} INPUT data/${FILE})
endforeach()

foreach(EXAMPLE ${EXAMPLES})
  add_executable(${EXAMPLE})
  target_sources(${EXAMPLE} PRIVATE ${EXAMPLE}.cpp)
  target_link_libraries(${EXAMPLE} PRIVATE ${LIBRARIES})
endforeach()
