# Copyright (C) 2018. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

cmake_minimum_required(VERSION 2.8.10)
project(cigre_network C CXX)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)

get_filename_component(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../tools/cmake/ ABSOLUTE)
get_filename_component(SRC ${CMAKE_SOURCE_DIR}/../sources ABSOLUTE)
get_filename_component(BIN ${CMAKE_SOURCE_DIR}/../binaries ABSOLUTE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN})

find_package(PythonLibs REQUIRED)
find_package(SimGrid REQUIRED)
find_package(SimGridFMI REQUIRED)

include_directories(${SimGrid_INCLUDE_DIR} SYSTEM)
include_directories(${SimGridFMI_INCLUDE_DIR} SYSTEM)

add_executable(cigre_network ${SRC}/cigre_network.cpp)
target_link_libraries(cigre_network simgrid-fmi ${SimGrid_LIBRARY})
