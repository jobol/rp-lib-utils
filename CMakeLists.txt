###########################################################################
# Copyright (C) 2015-2026 IoT.bzh Company
# Author: José Bollo <jose.bollo@iot.bzh>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is furnished
# to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
###########################################################################

cmake_minimum_required(VERSION 3.13)

project(rp-lib-utils
	VERSION 0.4.1
	DESCRIPTION  "Programming utilities"
	HOMEPAGE_URL "https://github.com/redpesk-core/rp-lib-utils"
	LANGUAGES C)

set(LIBRP_UTILS_VERSION "${PROJECT_VERSION}")

option(NO_LIBUUID    "Do not use libuuid"             OFF)

# Detect the platform reliably
if(ZEPHYR_BASE)
	if (NOT CONFIG_AFB_ZEPHYR)
		return()
	endif()
	set(WITH_ZEPHYR YES)
	set(NO_JSON_C  ON)
	set(NO_YAML    ON)
	set(NO_CURL    ON)
	set(NO_SYSTEMD ON)
	set(NO_LIBUUID ON)
	set(NO_SHARED_LIB ON)
	add_definitions(-DWITH_DIRENT=0)
	find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
        zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
else()
	set(WITH_ZEPHYR NO)
	include(FindPkgConfig)
	include(GNUInstallDirs)
	include(CTest)
	add_compile_options(
		-g
		-Wall
		-Werror=format-security
		-Wextra
		-Wconversion
		-Wincompatible-pointer-types
		-Wno-unused-parameter
		-Wsign-compare
		-Wsign-conversion
		-Werror=maybe-uninitialized
		-Werror=implicit-function-declaration
		-ffunction-sections
		-fdata-sections
		-fanalyzer
		-pipe
	)
	string(TOLOWER "${CMAKE_BUILD_TYPE}" btype)
	message("build type is ${btype}")
	if(btype STREQUAL debug)
		message("compiling for DEBUG")
		add_compile_options(
			-O0
			-fstack-protector-strong
			-fasynchronous-unwind-tables
			-Wp,-U_FORTIFY_SOURCE,-D_FORTIFY_SOURCE=3
			-Wp,-D_GLIBCXX_ASSERTIONS
		)
	elseif(btype STREQUAL profile)
		message("compiling for PROFILE")
		add_compile_options(-O2 -pg -coverage)
		add_link_options(-pg)
		link_libraries(gcov)
	else() # if(btype STREQUAL release)
		message("compiling for RELEASE")
		add_compile_options(-O2)
	endif()
endif()

# build sources
add_subdirectory(src)
if(NOT ZEPHYR_BASE)
	add_subdirectory(tools)
	add_subdirectory(tests)
endif()
