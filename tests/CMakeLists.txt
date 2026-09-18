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

pkg_check_modules(CHECK check IMPORTED_TARGET)

if(NOT NO_STATIC_LIB)
	set(MODE static)
else()
	set(MODE shared)
endif()

function(addtestfiles name)
	if(TARGET test-${name})
		set(lincs)
		foreach(inp ${ARGN})
			add_custom_command(
				OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${inp}.inc
				COMMAND sed "s:[\"\\\\]:\\\\&:g;s:^.*$:\"&\\\\n\":"
				${CMAKE_CURRENT_SOURCE_DIR}/${inp}
				> ${CMAKE_CURRENT_BINARY_DIR}/${inp}.inc
				DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${inp}
				VERBATIM
			)
			list(APPEND lincs ${CMAKE_CURRENT_BINARY_DIR}/${inp}.inc)
		endforeach()
		add_custom_target(incs-${name} DEPENDS ${lincs})
		target_include_directories(test-${name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
		add_dependencies(test-${name} incs-${name})
	endif()
endfunction(addtestfiles)

function(addtest name)
	set(ok TRUE)
	set(llibs)
	foreach(lib ${ARGN})
		if(TARGET rp-utils-${lib}-${MODE})
			list(APPEND llibs  rp-utils-${lib}-${MODE})
		else()
			set(ok FALSE)
		endif()
	endforeach()
	if(ok)
		add_executable(test-${name} test-${name}.c)
		target_link_libraries(test-${name} PkgConfig::CHECK)
		target_link_libraries(test-${name} ${llibs})
		target_include_directories(test-${name} PRIVATE ../src)
		add_test(NAME ${name} COMMAND test-${name})
		message("Adding test ${name}")
	else()
		message("NO test ${name}")
	endif()
endfunction(addtest)

if(CHECK_FOUND)
	addtest(expand-vars core)
	addtest(str2int core)
	addtest(base64 core)
	addtest(enum-map core)
#	addtest(globset core)
#	addtest(path-search file)
	addtest(expand-json json-c)
	addtest(default-expand-json json-c)
	addtestfiles(default-expand-json j1root j1inc1 j1inc2 j1resu)
#	addtest(json-locator json-c)
	addtest(json2int json-c)
	addtest(read-yaml-jsonc yaml json-c)
	addtestfiles(read-yaml-jsonc j0 y0)
	addtest(curl curl socket)
else()
	message(STATUS "no test because check library not found")
endif()
