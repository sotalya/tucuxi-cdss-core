# SPDX-License-Identifier: AGPL-3.0-or-later
cmake_minimum_required(VERSION 3.20)
project (tucucdss-core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(config_coverage "Builds for coverage")
if(config_coverage)
    set(CMAKE_CXX_FLAGS "--coverage -O0 -g")
endif(config_coverage)

include_directories(
    # ${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core/libs/
    ${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core/src/
    ${CMAKE_CURRENT_LIST_DIR}/../../libs/
    ${CMAKE_CURRENT_LIST_DIR}/../
)

include(${CMAKE_CURRENT_LIST_DIR}/../../libs/tucuxi-core/libs/tucuxi-core_libs)

option(config_withpdf "Enable PDF configuration" OFF)

set(HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/computer.h
    ${CMAKE_CURRENT_LIST_DIR}/exporter/abstracthtmlexport.h
    ${CMAKE_CURRENT_LIST_DIR}/exporter/abstractxpertrequestresultexport.h
    ${CMAKE_CURRENT_LIST_DIR}/exporter/static/filestring.h
    ${CMAKE_CURRENT_LIST_DIR}/exporter/xpertrequestresulthtmlexport.h
    ${CMAKE_CURRENT_LIST_DIR}/exporter/xpertrequestresultxmlexport.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/abstract/abstractxpertflowstep.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/abstract/abstractxpertflowstepprovider.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/covariatevalidatorandmodelselector.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/adjustmenttraitcreator.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/datevalidator.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/dosevalidator.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/generalxpertflowstepprovider.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/reportprinter.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/requestexecutor.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/samplevalidator.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/targetvalidator.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/covariatechecker.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/timeafterdoseexporter.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/padcollector.h
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/justificationcreator.h
    ${CMAKE_CURRENT_LIST_DIR}/query/admindata.h
    ${CMAKE_CURRENT_LIST_DIR}/query/xpertquerydata.h
    ${CMAKE_CURRENT_LIST_DIR}/query/xpertqueryimport.h
    ${CMAKE_CURRENT_LIST_DIR}/query/xpertquerytocoreextractor.h
    ${CMAKE_CURRENT_LIST_DIR}/query/xpertrequestdata.h
    ${CMAKE_CURRENT_LIST_DIR}/query/justification.h
    ${CMAKE_CURRENT_LIST_DIR}/language/languageexception.h
    ${CMAKE_CURRENT_LIST_DIR}/language/languagemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/result/abstractvalidationresult.h
    ${CMAKE_CURRENT_LIST_DIR}/result/covariatevalidationresult.h
    ${CMAKE_CURRENT_LIST_DIR}/result/dosedatevalidationresult.h
    ${CMAKE_CURRENT_LIST_DIR}/result/sampledatevalidationresult.h
    ${CMAKE_CURRENT_LIST_DIR}/result/dosevalidationresult.h
    ${CMAKE_CURRENT_LIST_DIR}/result/samplevalidationresult.h
    ${CMAKE_CURRENT_LIST_DIR}/result/xpertqueryresult.h
    ${CMAKE_CURRENT_LIST_DIR}/result/xpertrequestresult.h
    ${CMAKE_CURRENT_LIST_DIR}/utils/xpertutils.h
    ${CMAKE_CURRENT_LIST_DIR}/query/configimport.h
    ${CMAKE_CURRENT_LIST_DIR}/query/configuration.h
)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/computer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exporter/static/filestring.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exporter/xpertrequestresulthtmlexport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exporter/xpertrequestresultxmlexport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/covariatevalidatorandmodelselector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/adjustmenttraitcreator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/datevalidator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/dosevalidator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/generalxpertflowstepprovider.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/reportprinter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/requestexecutor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/samplevalidator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/targetvalidator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/covariatechecker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/timeafterdoseexporter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/padcollector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flow/general/justificationcreator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/query/admindata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/query/xpertquerydata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/query/xpertqueryimport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/query/justification.cpp
    ${CMAKE_CURRENT_LIST_DIR}/language/languageexception.cpp
    ${CMAKE_CURRENT_LIST_DIR}/language/languagemanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/query/xpertquerytocoreextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/query/xpertrequestdata.cpp
    ${CMAKE_CURRENT_LIST_DIR}/result/covariatevalidationresult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/result/dosedatevalidationresult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/result/sampledatevalidationresult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/result/dosevalidationresult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/result/samplevalidationresult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/result/xpertqueryresult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/result/xpertrequestresult.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/xpertutils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/query/configimport.cpp
    ${CMAKE_CURRENT_LIST_DIR}/query/configuration.cpp
)

if(config_withpdf)
    list(APPEND HEADERS ${CMAKE_CURRENT_LIST_DIR}/exporter/xpertrequestresultpdfexport.h)
    list(APPEND SOURCES ${CMAKE_CURRENT_LIST_DIR}/exporter/xpertrequestresultpdfexport.cpp)
endif()

add_library(tucucdss-core STATIC ${HEADERS} ${SOURCES})

if(NOT config_withpdf)
    add_definitions(-DCONFIG_WITHPDF)
else()
    if(WIN32)
        target_link_libraries(tucucdss-core PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../libs/wkhtmltox/windows/wkhtmltox.dll)
        set(COPY_FROM_PATH "${CMAKE_CURRENT_LIST_DIR}/../../libs/wkhtmltox/windows/wkhtmltox.dll")
        set(COPY_TO_PATH "${CMAKE_BINARY_DIR}")
    else()
        target_link_libraries(tucucdss-core PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../libs/wkhtmltox/ubuntu/libwkhtmltox.so.0.12.6)
        set(COPY_FROM_PATH "${CMAKE_CURRENT_LIST_DIR}/../../libs/wkhtmltox/ubuntu/libwkhtmltox.so.0.12.6")
        set(COPY_TO_PATH "${CMAKE_BINARY_DIR}")
    endif()

    # Copy wkhtmltopdf library
    add_custom_target(copydata
        COMMAND ${CMAKE_COMMAND} -E copy ${COPY_FROM_PATH} ${COPY_TO_PATH}
        COMMENT "Copying wkhtmltopdf library"
    )
    add_dependencies(tucucdss-core copydata)
endif()
