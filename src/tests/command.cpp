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

TEST_F(Command, RequestMessage) {
    auto cfg = config["Command"]["RequestMessage"];
    if (cfg["skip"].as<bool>()) {
        GTEST_SKIP();
    }
    // make sure there is no PROTOCOL_VERSION being published
    try {
        link->receive<PROTOCOL_VERSION>();
        FAIL() << "PROTOCOL_VERSION published before requesting. Can not do test";
    } catch(...) {}
    link->send<COMMAND_LONG>(1, 1,
                             MAV_CMD_REQUEST_MESSAGE, 0,
                             static_cast<float>(msg_helper<PROTOCOL_VERSION>::ID), NAN, NAN, NAN, NAN, NAN, NAN);
    auto ack = link->receive<COMMAND_ACK>();
    EXPECT_EQ(ack.result, MAV_RESULT_ACCEPTED);
    EXPECT_EQ(ack.command, MAV_CMD_REQUEST_MESSAGE);
    auto version = link->receive<PROTOCOL_VERSION>(1000);
    EXPECT_GE(version.version, 200);
}

TEST_F(Command, RequestProtocolVersion) {
    auto cfg = config["Command"]["RequestProtocolVersion"];
    if (cfg["skip"].as<bool>()) {
        GTEST_SKIP();
    }
    // make sure there is no PROTOCOL_VERSION being published
    try {
        link->receive<PROTOCOL_VERSION>();
        FAIL() << "PROTOCOL_VERSION published before requesting. Can not do test";
    } catch(...) {}
    link->send<COMMAND_LONG>(1, 1,
                             MAV_CMD_REQUEST_PROTOCOL_VERSION, 0,
                             1.f, NAN, NAN, NAN, NAN, NAN, NAN);
    auto ack = link->receive<COMMAND_ACK>();
    EXPECT_EQ(ack.result, MAV_RESULT_ACCEPTED);
    EXPECT_EQ(ack.command, MAV_CMD_REQUEST_PROTOCOL_VERSION);
    auto version = link->receive<PROTOCOL_VERSION>(1000);
    EXPECT_GE(version.version, 200);
}

TEST_F(Command, RequestAutopilotCapabilities) {
    auto cfg = config["Command"]["RequestAutopilotCapabilities"];
    if (cfg["skip"].as<bool>()) {
        GTEST_SKIP();
    }
    // make sure there is no PROTOCOL_VERSION being published
    try {
        link->receive<AUTOPILOT_VERSION>();
        FAIL() << "AUTOPILOT_VERSION published before requesting. Can not do test";
    } catch(...) {}
    link->send<COMMAND_LONG>(1, 1,
                             MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES, 0,
                             1.f, NAN, NAN, NAN, NAN, NAN, NAN);
    auto ack = link->receive<COMMAND_ACK>();
    EXPECT_EQ(ack.result, MAV_RESULT_ACCEPTED);
    EXPECT_EQ(ack.command, MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES);
    link->receive<AUTOPILOT_VERSION>(1000);
}
