cmake_minimum_required(VERSION 3.5)
project(zero-epwing)

include(ExternalProject)
ExternalProject_Add(
	eb
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/eb
	CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/eb/configure --disable-shared --disable-ebnet --disable-nls
	PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/eb
	BUILD_COMMAND make
	BUILD_IN_SOURCE 1
	INSTALL_COMMAND ""
	)
include_directories(eb ${CMAKE_BINARY_DIR})
option(JANSSON_EXAMPLES "" OFF)
option(JANSSON_BUILD_DOCS "" OFF)
option(JANSSON_WITHOUT_TESTS "" ON)
add_subdirectory(jansson)
link_directories(eb/eb/.libs ${CMAKE_BINARY_DIR}/jansson/lib)
add_executable(zero-epwing main.c book.c convert.c hooks.c)
add_dependencies(zero-epwing eb jansson)
target_link_libraries(zero-epwing libeb.a libz.a libjansson.a)
if (WIN32 OR APPLE)
    target_link_libraries(zero-epwing libiconv.a)
endif (WIN32 OR APPLE)
target_link_libraries(zero-epwing)
