
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
#include "tests/base.h"
#include "environment.hpp"

#include "gtest/gtest.h"



int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    
    if (argc != 2) {
        std::cout << "Usage: mavlink_test_suite CONNECTION_URL [gtest arguments]" << std::endl;
        return 1;
    }

    const std::string connection_url{argv[1]};
    Environment::create(connection_url);

    ::testing::AddGlobalTestEnvironment(Environment::getInstance());
    return RUN_ALL_TESTS();
}
