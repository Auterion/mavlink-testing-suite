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

# Specify C++17
set(CMAKE_CXX_STANDARD 17)

# Enable strict handling of warnings
if(MSVC)
    add_definitions("-WX -W2")
else()
    add_definitions("-Wall -Wextra -Werror")
endif()

include(cmake/dependencies.cmake)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCES
    src/tests/param.cpp
    src/tests/mission_sdk.cpp
    src/tests/mission.cpp
        )

enable_testing()
add_executable(mavlink_testing_suite
    src/main.cpp
    ${TEST_SOURCES}
)
add_dependencies(mavlink_testing_suite
    ${dependencies}
)

if(MSVC)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set (YAML_LIB libyaml-cppmd.lib)
    else()
        set (YAML_LIB libyaml-cppmdd.lib)
    endif()
else()
    set (YAML_LIB yaml-cpp)
endif()

target_link_libraries(mavlink_testing_suite
    mavsdk
    # mavsdk_action
    # mavsdk_mission
    # mavsdk_param
    # mavsdk_camera
    # mavsdk_mavlink_passthrough
    ${YAML_LIB}
    gtest_main
)

include(GoogleTest)
gtest_add_tests(mavlink_testing_suite SOURCES 
    ${TEST_SOURCES}
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
        format ${CMAKE_CURRENT_SOURCE_DIR}/tools/run_clang_format.sh ${CLANG_FORMAT_EXECUTABLE} ${MAVLINK_SUITE_FORMAT_SRCS})
else()
    message(STATUS "Not enabling format target (clang-format not found)")
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
    message(STATUS "Enabling clang-tidy target")
    add_custom_target(
        clang_tidy ${CLANG_TIDY_EXECUTABLE} -fix -header-filter="${CMAKE_CURRENT_LIST_DIR}/src/.*\\.[hpp|h]" -p ${CMAKE_CURRENT_BINARY_DIR} ${MAVLINK_SUITE_FORMAT_SRCS})
else()
    message(STATUS "Not enabling clang-tidy target (clang-tidy not found)")
endif()
