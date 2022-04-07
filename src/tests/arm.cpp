#include <gtest/gtest.h>
#include "../environment.hpp"
using namespace MavlinkTestingSuite;

class Arm : public ::testing::Test {
protected:
    const std::shared_ptr<PassthroughTester> link;

    Arm() :
          link(Environment::getInstance()->getPassthroughTester()) {
        link->flushAll();
    }
};

TEST_F(Arm, ArmDisarm) {
    auto conf = Environment::getInstance()->getConfig({"Arm", "ArmDisarm"});
    if (!conf || conf["skip"].as<bool>(false)) {
        GTEST_SKIP();
    }
    link->send<COMMAND_LONG>(1, 1, MAV_CMD_COMPONENT_ARM_DISARM,
                             0, 1., NAN, NAN, NAN, NAN, NAN, NAN);
    auto ack = link->receive<COMMAND_ACK>(1, 1);
    EXPECT_EQ(ack.result, MAV_RESULT_ACCEPTED);

    link->send<COMMAND_LONG>(1, 1, MAV_CMD_COMPONENT_ARM_DISARM,
                             0, 0.f, NAN, NAN, NAN, NAN, NAN, NAN);
    ack = link->receive<COMMAND_ACK>(1, 1);
    EXPECT_EQ(ack.result, MAV_RESULT_ACCEPTED);
}

