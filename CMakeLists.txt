cmake_minimum_required(VERSION 3.5.1)
project(libdansdl2)

option(BUILD_STATIC "generate static release" ON)
option(BUILD_SHARED "generate shared release" ON)
option(BUILD_DEBUG "set the release as debug" OFF)
option(BUILD_DEBUG_EXTRA_LOG "Add extra stuff to debug logs" OFF)
option(BUILD_TESTS "Build test code" OFF)

#library version and naming...
set(MAJOR_VERSION 3)
set(MINOR_VERSION 2)
set(PATCH_VERSION 6)

if(${BUILD_DEBUG})

	set(RELEASE_VERSION "debug")
else()

	set(RELEASE_VERSION "prod")
endif()


if(${CMAKE_VERSION} VERSION_LESS "3.22.0")

	add_definitions(-DMAJOR_VERSION=${MAJOR_VERSION})
	add_definitions(-DMINOR_VERSION=${MINOR_VERSION})
	add_definitions(-DPATCH_VERSION=${PATCH_VERSION})
	add_definitions(-DRELEASE_VERSION=\"${RELEASE_VERSION}\")

else()

	add_compile_definitions(MAJOR_VERSION=${MAJOR_VERSION})
	add_compile_definitions(MINOR_VERSION=${MINOR_VERSION})
	add_compile_definitions(PATCH_VERSION=${PATCH_VERSION})
	add_compile_definitions(RELEASE_VERSION=\"${RELEASE_VERSION}\")
endif()

#generic compile options.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -pedantic -fno-rtti)

#Add the include directories
#The cache thing means that this is user settable from the command line.
include_directories("include")

set(SOURCE "")
add_subdirectory("${PROJECT_SOURCE_DIR}/lib")

#library type and filenames.

if(${BUILD_DEBUG})

	set(CMAKE_BUILD_TYPE Debug)
	set(LIB_FILENAME "dansdl2_debug")

else()

	set(CMAKE_BUILD_TYPE Release)
	set(LIB_FILENAME "dansdl2")
endif()

if(${BUILD_STATIC})

	add_library(dansdl2_static STATIC ${SOURCE})
	set_target_properties(dansdl2_static PROPERTIES OUTPUT_NAME ${LIB_FILENAME})
	target_compile_definitions(dansdl2_static PUBLIC "-DLIB_VERSION=\"static\"")
	install(TARGETS dansdl2_static DESTINATION lib)

	message("will build ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}-${RELEASE_VERSION}-static")

	if(${BUILD_DEBUG}) 

		target_compile_definitions(dansdl2_static PUBLIC "-DLIBDANSDL2_DEBUG")

		if(${BUILD_DEBUG_EXTRA_LOG})

			target_compile_definitions(dansdl2_static PUBLIC "-DLIBDANSDL2_DEBUG_EXTRA_LOG")
		endif()
	endif()
endif()

if(${BUILD_SHARED})

	add_library(dansdl2_shared SHARED ${SOURCE})
	set_target_properties(dansdl2_shared PROPERTIES OUTPUT_NAME ${LIB_FILENAME})
	target_compile_definitions(dansdl2_shared PUBLIC "-DLIB_VERSION=\"shared\"")
	install(TARGETS dansdl2_shared DESTINATION lib)

	message("will build ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}-${RELEASE_VERSION}-shared")

	if(${BUILD_DEBUG}) 

		target_compile_definitions(dansdl2_shared PUBLIC "-DLIBDANSDL2_DEBUG")

		if(${BUILD_DEBUG_EXTRA_LOG})

			target_compile_definitions(dansdl2_shared PUBLIC "-DLIBDANSDL2_DEBUG_EXTRA_LOG")
		endif()
	endif()
endif()

#windows specifics
if(WIN32)

	include_directories(../log/include)
	add_compile_definitions(WINBUILD=1)

	if(${BUILD_SHARED})

		target_compile_definitions(dansdl2_shared PUBLIC -DWINBUILD)

		set(IMPORTED_LIB_POSTFIX "")
		if(${BUILD_DEBUG})

			set(IMPORTED_LIB_POSTFIX "_debug")
		endif()

		add_library(lm SHARED IMPORTED)
		set_target_properties(lm PROPERTIES 
			IMPORTED_IMPLIB "../../log/build/liblm${IMPORTED_LIB_POSTFIX}.dll.a"
			IMPORTED_LOCATION "../../log/build/liblm${IMPORTED_LIB_POSTFIX}.dll"
		)

#	target_link_libraries(dansdl2_static lm SDL2 SDL2_ttf SDL2_mixer SDL2_image freeglut glew32 glu32 opengl32 stdc++fs)
#	target_link_libraries(dansdl2_shared lm SDL2 SDL2_ttf SDL2_mixer SDL2_image freeglut glew32 glu32 opengl32 stdc++fs)

		target_link_libraries(dansdl2_shared lm SDL2 SDL2_ttf SDL2_mixer SDL2_image glu32 opengl32 stdc++fs)
	endif()

	if(${BUILD_STATIC})
		target_compile_definitions(dansdl2_static PUBLIC -DWINBUILD)
		target_link_libraries(dansdl2_static lm SDL2 SDL2_ttf SDL2_mixer SDL2_image glu32 opengl32 stdc++fs)
	endif()

	include_directories("${CMAKE_CURRENT_LIST_DIR}/../log/include/")


endif()

install(DIRECTORY include/ DESTINATION include)

if(BUILD_TESTS) 

	if(!${BUILD_SHARED})
	
		message(FATAL_ERROR "The shared library must be built to build the examples!")
	endif()

	add_executable(input_filter tests/drivers/input_events.cpp)
	add_executable(gl_version tests/drivers/gl_version.cpp)
	add_executable(draw tests/drivers/draw.cpp)

	if(WIN32)

		target_link_libraries(input_filter dansdl2_shared lm SDL2 SDL2_image SDL2_ttf SDL2_mixer pthread)
		target_link_options(input_filter PRIVATE -lmingw32 -lSDL2main -lSDL2 -mwindows)

		target_link_libraries(gl_version dansdl2_shared lm SDL2 SDL2_image SDL2_ttf SDL2_mixer pthread)
		target_link_options(gl_version PRIVATE -lmingw32 -lSDL2main -lSDL2 -mwindows)

		target_link_libraries(draw dansdl2_shared lm SDL2 SDL2_image SDL2_ttf SDL2_mixer pthread)
		target_link_options(draw PRIVATE -lmingw32 -lSDL2main -lSDL2 -mwindows)
	else()

		target_link_libraries(input_filter dansdl2_shared lm SDL2 SDL2_image SDL2_ttf SDL2_mixer GL pthread)
		target_link_libraries(gl_version dansdl2_shared lm SDL2 SDL2_image SDL2_ttf SDL2_mixer GL pthread)
		target_link_libraries(draw dansdl2_shared lm SDL2 SDL2_image SDL2_ttf SDL2_mixer GL pthread)
	endif()
endif()


