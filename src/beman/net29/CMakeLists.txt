# cmake-format: off
# src/beman/net29/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

set(TARGET_LIBRARY beman_net29)

add_subdirectory(tests)
add_library(${TARGET_LIBRARY} STATIC)

target_sources(${TARGET_LIBRARY}
    PRIVATE
    net29.cpp
    PUBLIC
    FILE_SET ${TARGET_LIBRARY}_public_headers TYPE HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/include
    FILES
    ${PROJECT_SOURCE_DIR}/include/beman/net29/net.hpp
    PUBLIC
    FILE_SET ${TARGET_LIBRARY}_detail_headers TYPE HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/include
    FILES
    ${PROJECT_SOURCE_DIR}/include/beman/net29/detail/buffer.hpp
)
get_property(DETAIL_HEADER_FILES TARGET ${TARGET_LIBRARY} PROPERTY HEADER_SET_${TARGET_LIBRARY}_detail_headers)
source_group("Header Files\\detail" FILES ${DETAIL_HEADER_FILES})

include(GNUInstallDirs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

string(TOLOWER ${CMAKE_PROJECT_NAME} CMAKE_LOWER_PROJECT_NAME)

install(
  TARGETS ${TARGET_LIBRARY}
  EXPORT ${TARGETS_EXPORT_NAME}1
  ARCHIVE DESTINATION lib/$<CONFIG>
  FILE_SET ${TARGET_LIBRARY}_public_headers
  FILE_SET ${TARGET_LIBRARY}_detail_headers
)
target_include_directories(${TARGET_LIBRARY} PUBLIC $<INSTALL_INTERFACE:include>)

install(EXPORT ${TARGETS_EXPORT_NAME}1
  FILE ${TARGET_LIBRARY}-config.cmake
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_LIBRARY}"
  NAMESPACE ${TARGET_LIBRARY}::
)
