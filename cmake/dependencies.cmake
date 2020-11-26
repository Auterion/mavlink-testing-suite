include(ExternalProject)


if(MAVSDK_INSTALL_DIR)
    message(STATUS "Using MAVSDK from: ${MAVSDK_INSTALL_DIR}")
    include_directories(${MAVSDK_INSTALL_DIR}/include)
    link_directories(${MAVSDK_INSTALL_DIR}/lib)
else()
    # clone and build MAVSDK via ExternalProject
    ExternalProject_Add(third_party_mavsdk
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/MAVSDK
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/build_mavsdk/install
            -DENABLE_MAVLINK_PASSTHROUGH=1
            GIT_REPOSITORY https://github.com/mavlink/MAVSDK.git
            GIT_TAG 730a0ef51d248a4c3f39fdb011ed07d28a7c37aa
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build_mavsdk
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install
        )
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/build_mavsdk/install/include)
    # Hack needed for plugin_base.h include.
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/build_mavsdk/install/include/mavsdk)
    # Whichever path works :)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/build_mavsdk/install/lib)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/build_mavsdk/install/lib64)
    list(APPEND dependencies third_party_mavsdk)
endif()


# add YAML (build from source)
ExternalProject_Add(third_party_yaml
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/yaml-cpp
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
       -DYAML_CPP_BUILD_TESTS=OFF
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.6.3
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build_yaml
    INSTALL_COMMAND ""
    )
include_directories(${CMAKE_CURRENT_BINARY_DIR}/third_party/yaml-cpp/include)
list(APPEND dependencies third_party_yaml)
