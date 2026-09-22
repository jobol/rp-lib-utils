
# ensure relocatable
set(alldirs http json misc sys yaml)

set_property(GLOBAL PROPERTY anasrcs)
set_property(GLOBAL PROPERTY targetlibs)

#
macro(chkpkg pkg descpkg)
	set_property(GLOBAL PROPERTY ${pkg}-descpkg ${descpkg})
	if(NOT NO_${pkg})
		pkg_check_modules(${pkg} ${descpkg})
	endif()
	if(${pkg}_FOUND)
		message(STATUS "using ${pkg}")
		add_compile_definitions(WITH_${pkg}=1 WITHOUT_${pkg}=0)
	else()
		message(STATUS "not using ${pkg}")
		add_compile_definitions(WITH_${pkg}=0 WITHOUT_${pkg}=1)
	endif()
endmacro()

# search modules
chkpkg(JSON_C  json-c>=0.13)
chkpkg(YAML    yaml-0.1)
chkpkg(CURL    libcurl)
chkpkg(LIBUUID uuid)

# macro for adding a source file
function(addlibll outname description)
	set_property(GLOBAL APPEND PROPERTY targetlibs ${outname})
	set_property(GLOBAL PROPERTY ${outname}-description ${description})
	if(NOT NO_STATIC_LIB)
		message("Adding target ${outname}-static")
		add_library(${outname}-static STATIC)
		set_target_properties(${outname}-static PROPERTIES
			OUTPUT_NAME ${outname}
			POSITION_INDEPENDENT_CODE ON
			VERSION "${LIBRP_UTILS_VERSION}")
		target_compile_options(${outname}-static PRIVATE -ffunction-sections -fdata-sections)
		target_include_directories(${outname}-static PUBLIC ${alldirs})
		install(TARGETS ${outname}-static)
	endif()
	if(NOT NO_SHARED_LIB)
		message("Adding target ${outname}-shared")
		add_library(${outname}-shared SHARED)
		set_target_properties(${outname}-shared PROPERTIES
			OUTPUT_NAME ${outname}
			VERSION "${LIBRP_UTILS_VERSION}"
			SOVERSION 0)
		target_include_directories(${outname}-shared PUBLIC ${alldirs})
		install(TARGETS ${outname}-shared)
	endif()
endfunction(addlibll)

#
if(NOT WITH_ZEPHYR)
	addlibll(rp-utils "all in one components" rp-utils)
else()
	zephyr_library_named(rp-lib-utils)
	target_include_directories(${ZEPHYR_CURRENT_LIBRARY} PUBLIC
		${alldirs})
endif()

# macro for adding a source file
function(addlib target description)
	if(NOT WITH_ZEPHYR)
		addlibll(rp-utils-${target} ${description})
	endif()
endfunction(addlib)

# macro for adding dependencies of libs
function(addlibpkgll outname pkg)
	set_property(GLOBAL APPEND PROPERTY ${outname}-pkgs ${pkg})
	if(NOT NO_STATIC_LIB)
		target_compile_options(${outname}-static PUBLIC ${${pkg}_CFLAGS})
		target_link_libraries(${outname}-static ${${pkg}_LDFLAGS})
	endif()
	if(NOT NO_SHARED_LIB)
		target_compile_options(${outname}-shared PUBLIC ${${pkg}_CFLAGS})
		target_link_libraries(${outname}-shared PUBLIC ${${pkg}_LDFLAGS})
	endif()
endfunction(addlibpkgll)

function(addlibpkg target)
	foreach(pkg ${ARGN})
		addlibpkgll(rp-utils ${pkg})
		addlibpkgll(rp-utils-${target} ${pkg})
	endforeach()
endfunction(addlibpkg)

# macro for adding dependencies of libs
function(addlibdeps target)
	foreach(dep ${ARGN})
		set_property(GLOBAL APPEND PROPERTY rp-utils-${target}-deps ${dep})
		if(NOT NO_STATIC_LIB)
			target_link_libraries(rp-utils-${target}-static rp-utils-${dep}-static)
		endif()
		if(NOT NO_SHARED_LIB)
			target_link_libraries(rp-utils-${target}-shared PUBLIC rp-utils-${dep}-shared)
		endif()
	endforeach()
endfunction(addlibdeps)

# macro for adding source files
function(addsrcll outname)
	if(NOT NO_STATIC_LIB)
		target_sources(${outname}-static PRIVATE ${ARGN})
	endif()
	if(NOT NO_SHARED_LIB)
		target_sources(${outname}-shared PRIVATE ${ARGN})
	endif()
endfunction(addsrcll)

function(addsrc target)
	if(WITH_ZEPHYR)
		foreach(src ${ARGN})
			zephyr_library_sources(${src})
		endforeach()
	else()
		addsrcll(rp-utils ${ARGN})
		addsrcll(rp-utils-${target} ${ARGN})
	endif()
	set_property(GLOBAL APPEND PROPERTY anasrcs ${ARGN})
endfunction(addsrc)

# macro for adding header files
function(addhdr target)
	install(FILES ${ARGN} DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR}/rp-utils)
endfunction(addhdr)

# macro for adding header files
function(addch target)
	foreach(src ${ARGN})
		addsrc(${target} ${src}.c)
		addhdr(${target} ${src}.h)
	endforeach()
endfunction(addch)

configure_file(misc/rp-version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/misc/rp-version.h @ONLY)
addhdr(core misc/rp-version.h)

# core library isn't depending of much things
addlib(core "core functions")
addch(core
	http/rp-escape
	json/rp-jsonstr
	misc/rp-base64
	misc/rp-enum-map
	misc/rp-expand-vars
	misc/rp-pearson
	misc/rp-uuid
	misc/rp-str2int
	misc/sha1
	sys/rp-verbose
)
if(LIBUUID_FOUND)
	addlibpkg(core LIBUUID)
endif()

if(NOT WITH_ZEPHYR)
	# library depending of file system
	addlib(file "filesystem functions")
	addlibdeps(file core)
	addch(file
		misc/rp-file
		misc/rp-path-search
		misc/rp-whichprog
	)
endif()

# library depending of system and socket
addlib(socket "socket functions")
addlibdeps(socket core)
addch(socket
	sys/rp-socket
	sys/rp-systemd
)

# library for json-c
if(JSON_C_FOUND)
	addlib(json-c "json-c wrapper")
	addlibdeps(json-c core)
	addlibpkg(json-c JSON_C)
	addch(json-c
		json/rp-jconf
		json/rp-jsonc-expand
		json/rp-jsonc-default-expand
		json/rp-jsonc-path
		json/rp-jsonc
	)
endif()

# library for yaml
if(JSON_C_FOUND AND YAML_FOUND)
	addlib(yaml "YAML front end for json-c")
	addlibpkg(yaml JSON_C YAML)
	addch(yaml
		yaml/rp-yaml
		yaml/rp-yaml-default-expand
	)
endif()

# library for curl
if(CURL_FOUND)
	addlib(curl "curl wrapper")
	addlibdeps(curl core)
	addlibpkg(curl CURL)
	addch(curl
		http/rp-curl
	)
endif()

# generate installation
get_property(targetlist GLOBAL PROPERTY targetlibs)
foreach(outname ${targetlist})
	get_property(description GLOBAL PROPERTY ${outname}-description)
	get_property(deps GLOBAL PROPERTY ${outname}-deps)
	get_property(pkgs GLOBAL PROPERTY ${outname}-pkgs)
	if(NOT NO_STATIC_LIB)
		set(modules)
		set(privates)
		foreach(dep ${deps})
			list(APPEND modules librp-utils-${dep}-static)
			list(APPEND privates -l:librp-utils-${dep}.a)
		endforeach()
		foreach(pkg ${pkgs})
			get_property(descpkg GLOBAL PROPERTY ${pkg}-descpkg)
			list(APPEND modules ${descpkg})
			list(APPEND privates ${${pkg}_LDFLAGS})
		endforeach()
		list(REMOVE_DUPLICATES modules)
		list(REMOVE_DUPLICATES privates)
		list(JOIN modules ", " modules)
		string(REGEX REPLACE "([<=>]+)" " \\1 " modules "${modules}")
		list(JOIN privates " " privates)
		file(WRITE ${CMAKE_BINARY_DIR}/lib${outname}-static.pc "\
Name: lib${outname}-static
Description: A library of utilities, ${description}
Version: ${LIBRP_UTILS_VERSION}
Requires: ${modules}
Libs: -L${CMAKE_INSTALL_FULL_LIBDIR} -l:lib${outname}.a ${privates}
Cflags: -I${CMAKE_INSTALL_FULL_INCLUDEDIR}
")
		install(FILES ${CMAKE_BINARY_DIR}/lib${outname}-static.pc DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)
	endif()
	if(NOT NO_SHARED_LIB)
		set(modules)
		set(privates)
		foreach(dep ${deps})
			list(APPEND modules librp-utils-${dep})
			list(APPEND privates -lrp-utils-${dep})
		endforeach()
		foreach(pkg ${pkgs})
			get_property(descpkg GLOBAL PROPERTY ${pkg}-descpkg)
			list(APPEND modules ${descpkg})
			list(APPEND privates ${${pkg}_LDFLAGS})
		endforeach()
		list(REMOVE_DUPLICATES modules)
		list(REMOVE_DUPLICATES privates)
		list(JOIN modules ", " modules)
		string(REGEX REPLACE "([<=>]+)" " \\1 " modules "${modules}")
		list(JOIN privates " " privates)
		file(WRITE ${CMAKE_BINARY_DIR}/lib${outname}.pc "\
Name: lib${outname}
Description: A library of utilities, ${description}
Version: ${LIBRP_UTILS_VERSION}
Requires: ${modules}
Libs: -L${CMAKE_INSTALL_FULL_LIBDIR} -l${outname}
Cflags: -I${CMAKE_INSTALL_FULL_INCLUDEDIR}
Libs.private: ${privates}
")
		install(FILES ${CMAKE_BINARY_DIR}/lib${outname}.pc DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)
	endif()
endforeach()

get_property(anasrcs GLOBAL PROPERTY anasrcs)
list(TRANSFORM anasrcs PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
add_custom_target(analyze gcc -fanalyzer -S -I ${CMAKE_CURRENT_SOURCE_DIR}/rp-utils ${anasrcs})

