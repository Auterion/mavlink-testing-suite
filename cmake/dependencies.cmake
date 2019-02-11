include(ExternalProject)

# add SDK
ExternalProject_Add(third_party_dronecode_sdk
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/DronecodeSDK
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/build_dronecode_sdk/install
        -DENABLE_MAVLINK_PASSTHROUGH=1
    GIT_REPOSITORY https://github.com/Dronecode/DronecodeSDK.git
    GIT_TAG v0.14.1
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build_dronecode_sdk
    INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install
    )
include_directories(${CMAKE_CURRENT_BINARY_DIR}/build_dronecode_sdk/install/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/build_dronecode_sdk/install/lib)

# add YAML (build from source)
ExternalProject_Add(third_party_yaml
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/yaml-cpp
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.6.2
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build_yaml
    INSTALL_COMMAND ""
    )
include_directories(${CMAKE_CURRENT_BINARY_DIR}/third_party/yaml-cpp/include)
