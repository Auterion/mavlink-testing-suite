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

    Ping() :
          link(Environment::getInstance()->getPassthroughTester()) {
        link->flushAll();
    }
};

TEST_F(Ping, PingPong) {
    auto conf = Environment::getInstance()->getConfig({"Ping", "PingPong"});
    if (!conf || conf["skip"].as<bool>(false)) {
        GTEST_SKIP();
    }
    // broadcast systemid, componentid
    link->send<PING>(micros(), 0, 0, 0);
    auto res = link->receive<PING>(1, 1);
    EXPECT_EQ(res.seq, 0);
    link->send<PING>(micros(), 1, 0, 0);
    res = link->receive<PING>(1, 1);
    EXPECT_EQ(res.seq, 1);
}
