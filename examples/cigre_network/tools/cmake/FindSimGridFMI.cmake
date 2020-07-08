# Copyright (C) 2018. The SimGrid Team. All rights reserved.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Affero Licence (see in file LICENCE).

# search for FMI4cpp
find_path(SimGridFMI_INCLUDE_DIR
	NAMES simgrid-fmi.hpp
	PATHS /usr/local/include ${SimGridFMI_PATH}/include)

include_directories("${SimGridFMI_INCLUDE_DIR}" SYSTEM)

enable_testing()

# Build the library
add_library(SimGridFMI::SimGridFMI SHARED IMPORTED)
find_library(SimGridFMI_LIBRARY NAMES simgrid-fmi PATHS ${SimGridFMI_PATH}/lib  /usr/local/lib)
SET_TARGET_PROPERTIES(SimGridFMI::SimGridFMI PROPERTIES
	INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${SimGridFMI_INCLUDE_DIR}
	INTERFACE_COMPILE_FEATURES cxx_alias_templates
	IMPORTED_LOCATION ${SimGridFMI_LIBRARY})

message("Found SimGridFMI: ${SimGridFMI_LIBRARY}")
