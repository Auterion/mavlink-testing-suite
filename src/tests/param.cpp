#include <gtest/gtest.h>
#include "../environment.hpp"
#include "../passthrough_messages.hpp"


// Basic passthrough test infra test
TEST(Params, ParamValue) {
    auto link = Environment::getInstance()->getPassthroughTester();

    link->send<PARAM_REQUEST_READ>(1, 1, "SYS_AUTOSTART", -1);
    auto r = link->receive<PARAM_VALUE>();

    std::cout << "Param " << r.param_id << " val: " << floatAs<uint32_t>(r.param_value) << std::endl;
}

