cmake_minimum_required(VERSION 2.8.12)

project(mavlink-testing-suite C CXX)

# required for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Git)

if (GIT_FOUND)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE SUITE_VERSION_GIT_REVISION
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	message(STATUS "Building in git revision ${SUITE_VERSION_GIT_REVISION}")
	# TODO: build into binary
else()
	set(SUITE_VERSION_GIT_REVISION "n/a")
	message(STATUS "Executable git is missing, choosing default value for git revision")
endif()
string(TIMESTAMP SUITE_VERSION_BUILD_TIMESTAMP "%Y-%m-%dT%H:%M:%S.000000Z" UTC)

# Specify at least C++11
set(CMAKE_CXX_STANDARD 11)

set(SDK_INSTALL_DIR "" CACHE STRING "SDK installation directory")
if("${SDK_INSTALL_DIR}" STREQUAL "")
	message( FATAL_ERROR "Pass the SDK installation directory via -DSDK_INSTALL_DIR=<directory>" )
endif()

# Enable strict handling of warnings
if(MSVC)
    add_definitions("-WX -W2")
else()
    add_definitions("-Wall -Wextra -Werror")
endif()

# add SDK
include_directories(${SDK_INSTALL_DIR}/include)
link_directories(${SDK_INSTALL_DIR}/lib)

# add YAML (build from source)
include(ExternalProject)
ExternalProject_Add(yaml
	SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party/yaml-cpp
	CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
	BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build_yaml
	INSTALL_COMMAND ""
	)
include_directories(${CMAKE_CURRENT_LIST_DIR}/third_party/yaml-cpp/include)


add_executable(mavlink_testing_suite
    src/main.cpp

    src/tests/base.cpp
    src/tests/mission.cpp
)
add_dependencies(mavlink_testing_suite yaml)

target_link_libraries(mavlink_testing_suite
    dronecode_sdk
    dronecode_sdk_action
    dronecode_sdk_telemetry
	dronecode_sdk_mission
	${CMAKE_CURRENT_BINARY_DIR}/build_yaml/libyaml-cpp.a
)

find_program(CLANG_FORMAT_EXECUTABLE
	NAMES clang-format clang-format-5.0
		clang-format-4.0 clang-format-3.9
		clang-format-3.8 clang-format-3.7
		clang-format-3.6 clang-format-3.5
		clang-format-3.4 clang-format-3.3
	DOC "clang-format executable"
)

file(GLOB_RECURSE MAVLINK_SUITE_FORMAT_SRCS
	src/*.h src/*.cpp)
if (CLANG_FORMAT_EXECUTABLE)
	message(STATUS "Enabling format target")

	add_custom_target(
		format ${CLANG_FORMAT_EXECUTABLE} -style=file -i ${MAVLINK_SUITE_FORMAT_SRCS})
endif()

find_program(CLANG_TIDY_EXECUTABLE
	NAMES clang-tidy clang-tidy-5.0
		clang-tidy-4.0 clang-tidy-3.9
		clang-tidy-3.8 clang-tidy-3.7
		clang-tidy-3.6 clang-tidy-3.5
		clang-tidy-3.4 clang-tidy-3.3
	DOC "clang-tidy executable"
)
if (CLANG_TIDY_EXECUTABLE)
	add_custom_target(
		clang_tidy ${CLANG_TIDY_EXECUTABLE} -fix -header-filter="${CMAKE_CURRENT_LIST_DIR}/src/.*\\.[hpp|h]" -p ${CMAKE_CURRENT_BINARY_DIR} ${MAVLINK_SUITE_FORMAT_SRCS})
endif()
