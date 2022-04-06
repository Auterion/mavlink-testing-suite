#include <gtest/gtest.h>
#include "../environment.hpp"
using namespace MavlinkTestingSuite;

class Arm : public ::testing::Test {
protected:
    const std::shared_ptr<PassthroughTester> link;
    const YAML::Node config;

    Arm() :
          link(Environment::getInstance()->getPassthroughTester()),
          config(Environment::getInstance()->getConfig()) {
        link->flushAll();
    }
};

TEST_F(Arm, ArmDisarm) {
    link->send<COMMAND_LONG>(1, 1, MAV_CMD_COMPONENT_ARM_DISARM,
                             0, 1., NAN, NAN, NAN, NAN, NAN, NAN);
    auto ack = link->receive<COMMAND_ACK>(1, 1);
    EXPECT_EQ(ack.result, MAV_RESULT_ACCEPTED);

    link->send<COMMAND_LONG>(1, 1, MAV_CMD_COMPONENT_ARM_DISARM,
                             0, 0.f, NAN, NAN, NAN, NAN, NAN, NAN);
    ack = link->receive<COMMAND_ACK>(1, 1);
    EXPECT_EQ(ack.result, MAV_RESULT_ACCEPTED);
}

