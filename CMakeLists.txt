cmake_minimum_required(VERSION 2.8.12)

## Specify your project's name
project(mavlink-testing-suite)

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

