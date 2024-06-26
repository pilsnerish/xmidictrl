#----------------------------------------------------------------------------------------------------------------------
#   XMidiCtrl - MIDI Controller plugin for X-Plane
#
#   Copyright (c) 2021-2024 Marco Auer
#
#   XMidiCtrl is free software: you can redistribute it and/or modify it under the terms of the
#   GNU Affero General Public License as published by the Free Software Foundation, either version 3
#   of the License, or (at your option) any later version.
#
#   XMidiCtrl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
#   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License along with XMidiCtrl.
#   If not, see <https://www.gnu.org/licenses/>.
#----------------------------------------------------------------------------------------------------------------------

# Set required CMake version
cmake_minimum_required(VERSION 3.16)

# Set project details
project(XMidiCtrl VERSION 1.14 DESCRIPTION "MIDI Controller Plugin for X-Plane 11/12")
set(XMIDICTRL_BETA_LABEL .beta-1)
set(PROJECT_SIGNATURE mauer.xmidictrl)

message("-----------------------------------------------")
message("-- Configure ${PROJECT_NAME} ${PROJECT_VERSION}${XMIDICTRL_BETA_LABEL}")
message("-----------------------------------------------")

include(CTest)

# Create version file with project name and version numbers
message("-- Creating version file")
configure_file(
        "${PROJECT_SOURCE_DIR}/version.h.in"
        "${PROJECT_BINARY_DIR}/version.h"
)

if (MSVC)
    add_definitions("/FI${PROJECT_BINARY_DIR}/version.h")
else()
    add_definitions("-include ${PROJECT_BINARY_DIR}/version.h")
endif()
include_directories(${PROJECT_BINARY_DIR})

# use highest optimization level and optimize for size
if (MSVC)
    add_compile_options("$<$<CONFIG:RELEASE>:/O2>")
else()
    add_compile_options("$<$<CONFIG:RELEASE>:-O3>" "$<$<CONFIG:RELEASE>:-Os>")
endif()

# set compiler options
set(CMAKE_CXX_STANDARD 17)
if (MSVC)
    add_compile_options("/Zc:__cplusplus")
endif()

# disable warning no-free-nonheap-object because of toml11
if (MSVC)
    add_compile_options("/W4")
else()
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 10.0.0)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-sign-compare -fPIC")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-sign-compare -Wno-free-nonheap-object -fPIC")
    endif()
endif()

# enable universal binary for macOS
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

    # Apple deprecated OpenGL...yea, we know, keep quiet
    set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations")
endif()

# add external libraries
add_subdirectory(external)

# Add plugin
add_subdirectory(src)

# Enable the included unit tests
enable_testing()

# Add unit tests
add_subdirectory(tests)
