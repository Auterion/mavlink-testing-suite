#include <gtest/gtest.h>
#include "../environment.hpp"
#include "../passthrough_messages.hpp"

using namespace MavlinkTestingSuite;

class Params : public ::testing::Test {
protected:
    const std::shared_ptr<PassthroughTester> link;
    const YAML::Node config;

    Params() : 
    link(Environment::getInstance()->getPassthroughTester()), 
    config(Environment::getInstance()->getConfig()) {}
};

std::string paramIdString(const char* param_id) {
    return std::string(param_id, strnlen(param_id, 16));
}

TEST_F(Params, ParamReadWriteInteger) {
    auto conf = config["ParamReadWriteInteger"];
    auto param_id = conf["param_id"].as<std::string>();
    auto default_value = conf["default_value"].as<int>();
    auto change_value = conf["change_value"].as<int>();

    // Read current value
    link->send<PARAM_REQUEST_READ>(1, 1, param_id.c_str(), -1);
    auto r1 = link->receive<PARAM_VALUE>();
    EXPECT_EQ(paramIdString(r1.param_id), param_id) << "Returned param ID does not match requested param ID";
    EXPECT_EQ(floatUnpack<int32_t>(r1.param_value), default_value) << "Returned value for param " << param_id << " does not have configured default value";
    EXPECT_EQ(r1.param_type, MAV_PARAM_TYPE_INT32) << "Returned param type is wrong";

    // Write new value
    link->send<PARAM_SET>(1, 1, param_id.c_str(), floatPack(change_value), MAV_PARAM_TYPE_INT32);
    auto r2 = link->receive<PARAM_VALUE>();
    EXPECT_EQ(paramIdString(r2.param_id), param_id) << "Returned param ID does not match requested param ID";

    // Re-read new value
    link->send<PARAM_REQUEST_READ>(1, 1, param_id.c_str(), -1);
    auto r3 = link->receive<PARAM_VALUE>();
    EXPECT_EQ(paramIdString(r3.param_id), param_id) << "Returned param ID does not match requested param ID";
    EXPECT_EQ(floatUnpack<int32_t>(r3.param_value), change_value) << "Returned value for param " << param_id << " is not changed value";
    EXPECT_EQ(r3.param_type, MAV_PARAM_TYPE_INT32) << "Returned param type is wrong";

    // Restore default value
    link->send<PARAM_SET>(1, 1, param_id.c_str(), floatPack(default_value), MAV_PARAM_TYPE_INT32);
    auto r4 = link->receive<PARAM_VALUE>();
    EXPECT_EQ(paramIdString(r4.param_id), param_id) << "Returned param ID does not match requested param ID";
}

TEST_F(Params, ParamListAll) {
    link->send<PARAM_REQUEST_LIST>(1, 1);
    int count = 0;

    std::set<std::string> received_param_ids;
    do {
        auto r = link->receive<PARAM_VALUE>();
        count = r.param_count;
        // The _HASH_CHECK param does not count towards the total count
        if (paramIdString(r.param_id) != "_HASH_CHECK") {
            received_param_ids.insert(paramIdString(r.param_id));
        }
    } while(static_cast<int>(received_param_ids.size()) < count);

    std::cout << received_param_ids.size() << std::endl;

    EXPECT_EQ(received_param_ids.size(), count) << "Did not receive all params";
    // next receive should time out
    try {
        link->receive<PARAM_VALUE>();
        FAIL() << "Received more params";
    } catch(TimeoutError&) {}
}

