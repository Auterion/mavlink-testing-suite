#include <gtest/gtest.h>
#include "../environment.hpp"
#include <sys/time.h>

using namespace MavlinkTestingSuite;

// Get current timestamp in microseconds
inline uint64_t micros() {
    struct timeval t;
    gettimeofday(&t, nullptr);
    return t.tv_sec * ((uint64_t)1000000) + t.tv_usec;
}

class Ping : public ::testing::Test {
protected:
    const std::shared_ptr<PassthroughTester> link;
    const YAML::Node config;

    Ping() :
          link(Environment::getInstance()->getPassthroughTester()),
          config(Environment::getInstance()->getConfig()) {
        link->flushAll();
    }
};

TEST_F(Ping, PingPong) {
    auto cfg = config["Ping"]["PingPong"];
    if (cfg["skip"].as<bool>()) {
        GTEST_SKIP();
    }
    // broadcast systemid, componentid
    link->send<PING>(micros(), 0, 0, 0);
    auto res = link->receive<PING>();
    EXPECT_EQ(res.seq, 0);
    link->send<PING>(micros(), 1, 0, 0);
    res = link->receive<PING>();
    EXPECT_EQ(res.seq, 1);
}
