cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME "AMAbot")
project(
    ${PROJECT_NAME}
    VERSION "0.1.0"
    LANGUAGES CXX
    DESCRIPTION ""
    )

set(EXTERNAL_DEPENDENCIES ${PROJECT_SOURCE_DIR}/external) 

# AMABot options
set(USE_LOGGER "ON")

# DPP options
set(BUILD_VOICE_SUPPORT "OFF")
set(RUN_LDCONFIG "OFF")
set(DPP_INSTALL "OFF")
set(DPP_BUILD_TEST "OFF")

# Curlpp options
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FindPkgConfig)
#pkg_check_modules(CURLPP REQUIRED curlpp)

# Downloads all git submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "$PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule --init failed with ${GIT_SUBMOD_RESULT}, checkout submodules")
        endif()
    endif()
endif()

# Check all git submodules
if(NOT EXISTS "${EXTERNAL_DEPENDENCIES}/DPP/CMakeLists.txt")
    message(FATAL_ERROR "DPP submodule not downloaded! Please update submodule")
endif()
if(NOT EXISTS "${EXTERNAL_DEPENDENCIES}/curlpp/CMakeLists.txt")
    message(FATAL_ERROR "Curlpp submodule not downloaded! Please update submodule")
endif()

add_subdirectory(${EXTERNAL_DEPENDENCIES}/DPP)
add_subdirectory(${EXTERNAL_DEPENDENCIES}/curlpp)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/ MAIN_SOURCES)

configure_file(config.h.in AmaBotConfig.h)

set(SOURCES
    ${SOURCES} 
    ${MAIN_SOURCES}
)

set(INCLUDES
    ${INCLUDES} 
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    pthread
    dpp
    ${CURLPP_LDFLAGS}
)

#add_library(${PROJECT_NAME} OBJECT ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${EXTERNAL_DEPENDENCIES}DPP/include
    #${EXTERNAL_DEPENDENCIES}curlpp/include
    ${INCLUDES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
