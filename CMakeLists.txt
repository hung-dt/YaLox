#------------------------------------------------------------------------------
#   Author: HungDT
#   Date  : 2025-12-26
#
#   Main CMake project file
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.26.0)

#------------------------------------------------------------------------------
# Prohibit in-source build

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "DO NOT build in-source!!!\n"
        "Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif()

project(YaLox
    VERSION 0.1.0
    DESCRIPTION
        "Yet another Lox interpreter port in C++, from the book 'Crafting Interpreter' (https://craftinginterpreters.com) by Robert Nystrom."
    LANGUAGES C CXX)

#------------------------------------------------------------------------------
# Set default build type
#------------------------------------------------------------------------------
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug' as non was specified")
    set(CMAKE_BUILD_TYPE Debug
        CACHE STRING "Choose the type of build." FORCE)

    # set possible values of build type for cmake-gui, ccmake
    set_property(CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#------------------------------------------------------------------------------
# Compiler configs
#------------------------------------------------------------------------------
if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
set(CMAKE_CXX_EXTENSIONS OFF)

# Common warning flags
if (MSVC)
    # Warning level 4 for Microsoft Visual C++
    add_compile_options(/W4)
else()
    # Additional warnings for other compilers like GCC or Clang
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)
endif()

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" FALSE)
if (WARNINGS_AS_ERRORS)
    set(CMAKE_CXX_FLAGS "${CMAXE_CXX_FLAGS} -Werror")
endif()

#------------------------------------------------------------------------------
# Use ccache if available

find_program(CCACHE ccache)
if (CCACHE)
    message(STATUS "Found ccache and enabled")
    set(CMAKE_CXX_COMPILE_LAUNCHER ${CCACHE})
else()
    message("ccache not found: cannot use")
endif()

#------------------------------------------------------------------------------
# Option to enable LTO

option(ENABLE_IPO "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)" OFF)
if (ENABLE_IPO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if (result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(SEND_ERROR "IPO is not supported: ${output}")
    endif()
endif()

#------------------------------------------------------------------------------
# Enhance error reporting and compiler messages

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fdiagnostics-color=always)
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    add_compile_options(-fcolor-diagnostics)
endif()

#------------------------------------------------------------------------------
# General cmake settings
#------------------------------------------------------------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_lib)

#------------------------------------------------------------------------------
# Import cmake modules
#------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(GenerateVersion)
#include(StandardConfigs)
#include(ClangFormat)
#include(CppCheck)

#------------------------------------------------------------------------------
# Other project's CMakeLists.txt
#------------------------------------------------------------------------------

add_subdirectory(src)
