#include <gtest/gtest.h>
#include "../environment.hpp"
using namespace MavlinkTestingSuite;

class Command : public ::testing::Test {
protected:
    const std::shared_ptr<PassthroughTester> link;
    const YAML::Node config;

    Command() :
          link(Environment::getInstance()->getPassthroughTester()),
          config(Environment::getInstance()->getConfig()) {
        link->flushAll();
    }
};



