# Copyright 2020 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

project("Mumble Positional Audio Helper")

option(warnings-all "Enable all warnings." ON)
option(warnings-as-errors "All warnings are treated as errors." ON)

set(PLUGIN_HEADER_DIR "../mumble/plugins" CACHE STRING "Path to the directory containing the plugin headers.")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(mumble-pahelper
	"main.cpp"
	"mumblepahelper.cpp"
	"mumblepahelper.h"
	"mumblepahelper.ui"
	"Plugins.cpp"
	"Plugins.h"

	$<$<PLATFORM_ID:Windows>:mumblepahelper.rc>
)

set_target_properties(mumble-pahelper PROPERTIES
	AUTOMOC ON
	AUTORCC ON
	AUTOUIC ON
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	target_compile_definitions(mumble-pahelper
		PRIVATE
			"UNICODE"
			"WIN32_LEAN_AND_MEAN"
	)
endif()

target_include_directories(mumble-pahelper PRIVATE ${PLUGIN_HEADER_DIR})

target_link_libraries(mumble-pahelper PRIVATE Qt5::Widgets)

if(warnings-all)
	target_compile_options(mumble-pahelper
		PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:/W4>
			$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
	)
endif()

if(warnings-as-errors)
	target_compile_options(mumble-pahelper
		PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:/WX>
			$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
	)
endif()
