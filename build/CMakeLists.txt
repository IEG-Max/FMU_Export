# Copyright (C) 2018. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

cmake_minimum_required(VERSION 2.8.10)
project (pandapower_fmu C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#~ set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Xlinker -export-dynamic")
#~ set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} "-L/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu -L/usr/lib -lpython3.6m -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions")

find_package(PythonLibs REQUIRED)
find_package(LibXml2 REQUIRED)

# build the pandapower FMU
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../binaries/linux64)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../binaries/linux64)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../binaries/linux64)
add_library(pandapower SHARED ${CMAKE_SOURCE_DIR}/../sources/pandapower_fmu.c)
#add_executable(pandapower ${CMAKE_SOURCE_DIR}/../sources/pandapower_fmu.c)
set_target_properties(pandapower PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_LIST_DIR}/../include/fmi2Functions.h)
target_include_directories(pandapower PRIVATE ../include ${LIBXML2_INCLUDE_DIR})
target_link_libraries(pandapower ${PYTHON_LIBRARIES} ${LIBXML2_LIBRARIES})
set_target_properties(pandapower PROPERTIES PREFIX "")
