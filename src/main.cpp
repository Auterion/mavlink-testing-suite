
#include <mavsdk/mavsdk.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <chrono>
#include "environment.hpp"

#include "gtest/gtest.h"



int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    
    if (argc < 2) {
        std::cout << "Usage: mavlink_test_suite CONNECTION_URL [yaml-config file] [gtest arguments]" << std::endl;
        return 1;
    }

    const std::string yaml_path = (argc > 2) ? argv[2] : "./config/config.yaml";
    const std::string connection_url{argv[1]};

    try {
        MavlinkTestingSuite::Environment::create(connection_url, yaml_path);
    } catch(YAML::BadFile &e) {
        std::cerr << "YAML file load \""<< yaml_path << "\" failed: " << e.msg << std::endl;
        exit(1);
    }

    ::testing::AddGlobalTestEnvironment(MavlinkTestingSuite::Environment::getInstance());
    return RUN_ALL_TESTS();
}
