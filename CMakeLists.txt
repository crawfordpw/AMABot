cmake_minimum_required(VERSION 3.16.3)

set(PROJECT_NAME "AMAbot")
project(
    ${PROJECT_NAME}
    VERSION "0.1.0"
    LANGUAGES CXX
    DESCRIPTION ""
    )

set(EXTERNAL_DEPENDENCIES ${PROJECT_SOURCE_DIR}/external) 

# AMABot options
# TODO: Figure out how to build without main.
set(BUILD_WITH_MAIN_CPP "ON")
set(USE_LOGGER "ON")

# DPP options
set(BUILD_VOICE_SUPPORT "OFF")
set(RUN_LDCONFIG "OFF")
set(DPP_INSTALL "OFF")
set(DPP_BUILD_TEST "OFF")

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
    message(FATAL_ERROR "DPP submodules not downloaded! Please update submodule")
endif()

add_subdirectory(${EXTERNAL_DEPENDENCIES}/DPP)

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

if(BUILD_WITH_MAIN_CPP)
    set(SOURCES
        ${SOURCES} 
        ${PROJECT_SOURCE_DIR}/Main.cpp
    )

    add_executable(${PROJECT_NAME} ${SOURCES})

    target_link_libraries(${PROJECT_NAME}
        dpp
    )
endif()

#add_library(${PROJECT_NAME} OBJECT ${SOURCES})
target_compile_options(${PROJECT_NAME} PUBLIC -Wall)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${EXTERNAL_DEPENDENCIES}DPP/include
    ${INCLUDES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
