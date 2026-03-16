# SPDX-License-Identifier: AGPL-3.0-or-later
cmake_minimum_required(VERSION 3.20)
project (tucu-cdss-tests LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(config_coverage "Builds for coverage" OFF)
# if(config_coverage)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g")
#   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
#   set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
# endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core/src/tucucommon tucucommon)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core/src/tucucore tucucore)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core/src/tucuquery tucuquery)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../src/tucucdss-core tucucdss-core)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core/libs/tucuxi-core_libs)
include(${CMAKE_CURRENT_LIST_DIR}/add_gtest)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/tucuxi-core/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


file(GLOB TESTS_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_adjustmenttraitcreator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_configimport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_covariatechecker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_covariatevalidatorandmodelselector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_datevalidator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_dosevalidator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_justificationcreator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_languagemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_requestexecutor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_samplevalidator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_targetvalidator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_xpertqueryimport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_xpertqueryresultcreation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_xpertquerytocoreextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gtest_xpertutils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/testutils.cpp
)

file(GLOB TESTS_HEADERS_FILES
    ${CMAKE_CURRENT_LIST_DIR}/src/testutils.h
)

add_executable(tucu-cdss-tests ${TESTS_SOURCE_FILES} ${TESTS_HEADERS_FILES})

set(LIBS_TO_LINK gtest tucucdss-core tucuquery tucucore tucucommon)
if(UNIX)
  list(APPEND LIBS_TO_LINK pthread)
endif()

target_link_libraries(tucu-cdss-tests PRIVATE ${LIBS_TO_LINK})

if(config_coverage)
  target_compile_options(tucu-cdss-tests PRIVATE --coverage -O0 -g -fprofile-update=atomic)
  target_link_options(tucu-cdss-tests PRIVATE --coverage)

  if(TARGET tucucdss-core)
    target_compile_options(tucucdss-core PRIVATE --coverage -O0 -g -fprofile-update=atomic)
    target_link_options(tucucdss-core PRIVATE --coverage)
  endif()
endif()

if(NOT WIN32)
    target_compile_options(tucu-cdss-tests PRIVATE -Wno-int-in-bool-context)

    target_compile_options(tucu-cdss-tests PRIVATE -Wno-extra-semi-stmt)
endif()
