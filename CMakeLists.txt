# SPDX-License-Identifier: AGPL-3.0-or-later
cmake_minimum_required(VERSION 3.20)
project(tucuxi_cdss_cli LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(config_coverage "Builds for coverage" OFF)

option(TUCU_EASY_DEBUG "Builds the dates and times with a string representation for easy debug")
if (TUCU_EASY_DEBUG)
  add_compile_definitions(TUCU_EASY_DEBUG)
endif()

option(TUCU_CHECK_DATETIME "Checks DateTime initialization validity (debug only)")
if (TUCU_CHECK_DATETIME)
  add_compile_definitions(TUCU_CHECK_DATETIME)
endif()

option(TUCU_CHECK_TYPES "Use strong wrapper types to detect type-smells (debug only)")
if (TUCU_CHECK_TYPES)
  add_compile_definitions(CHECK_TYPES)
endif()

option(TUCU_TIME_DEBUG "DateTime embeds a string representation for debug")
if (TUCU_TIME_DEBUG)
  add_compile_definitions(TUCU_TIME_DEBUG)
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/tucuxi-core/src/tucucommon tucucommon)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/tucuxi-core/src/tucucore   tucucore)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/tucuxi-core/src/tucuquery  tucuquery)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src/tucucdss-core               tucucdss-core)

include(${CMAKE_CURRENT_LIST_DIR}/libs/tucuxi-core/libs/tucuxi-core_libs)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/libs
  ${CMAKE_CURRENT_SOURCE_DIR}/libs/tucuxi-core/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

file(GLOB CDSS_SOURCE_FILES
  ${CMAKE_CURRENT_LIST_DIR}/src/tucucdss-cli.cpp
)
file(GLOB CDSS_HEADERS_FILES)

# =============================================================================
# Templates pipeline (encapsulated)
# =============================================================================
include(${CMAKE_SOURCE_DIR}/cmake/templates.cmake)

tucu_package_html_templates(tucu_templates
  SOURCE_DIR    ${CMAKE_SOURCE_DIR}/html_template
  DEST_DIR      ${CMAKE_BINARY_DIR}/html_template
  INLINE_SCRIPT ${CMAKE_SOURCE_DIR}/cmake/build_template.cmake
  CLEAN_SCRIPT  ${CMAKE_SOURCE_DIR}/cmake/clean_templates.cmake
)

# =============================================================================
# Config copy
# =============================================================================
add_custom_target(copy_configfile ALL
  COMMAND ${CMAKE_COMMAND} -E copy
          ${CMAKE_SOURCE_DIR}/config.xml
          ${CMAKE_BINARY_DIR}/config.xml
  COMMENT "Copying configurations file to build directory"
  VERBATIM
)

# =============================================================================
# Executable
# =============================================================================
add_executable(tucuxi_cdss_cli ${CDSS_SOURCE_FILES} ${CDSS_HEADERS_FILES})

add_dependencies(tucuxi_cdss_cli copy_configfile)
add_dependencies(tucuxi_cdss_cli tucu_templates)

target_compile_definitions(tucuxi_cdss_cli PRIVATE
  TUCU_BUILD_DIR="${CMAKE_BINARY_DIR}"
  TUCU_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

set(LIBS_TO_LINK tucucdss-core tucuquery tucucore tucucommon)
if(UNIX)
  list(APPEND LIBS_TO_LINK pthread)
endif()

target_link_libraries(tucuxi_cdss_cli PRIVATE ${LIBS_TO_LINK})

if(config_coverage)
  target_compile_options(tucuxi_cdss_cli PRIVATE --coverage -O0 -g -fprofile-update=atomic)
  target_link_options(tucuxi_cdss_cli PRIVATE --coverage)

  if(TARGET tucucdss-core)
    target_compile_options(tucucdss-core PRIVATE --coverage -O0 -g -fprofile-update=atomic)
    target_link_options(tucucdss-core PRIVATE --coverage)
  endif()

  # foreach(t IN ITEMS tucuquery tucucore tucucommon)
  #   if(TARGET ${t})
  #     target_compile_options(${t} PRIVATE --coverage -O0 -g -fprofile-update=atomic)
  #     target_link_options(${t} PRIVATE --coverage)
  #   endif()
  # endforeach()
endif()

if(NOT WIN32)
  target_compile_options(tucuxi_cdss_cli PRIVATE -Wno-int-in-bool-context)
endif()
