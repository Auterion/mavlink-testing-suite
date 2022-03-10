#include <gtest/gtest.h>
#include <sys/time.h>
#include "../environment.hpp"
using namespace MavlinkTestingSuite;

// Get current timestamp in microseconds
uint64_t micros() {
    struct timeval t;
    gettimeofday(&t, nullptr);
    return t.tv_sec * ((uint64_t)1000000) + t.tv_usec;
}


class Telemetry : public ::testing::Test {
protected:
    const std::shared_ptr<PassthroughTester> link;
    const YAML::Node config;

    Telemetry() :
          link(Environment::getInstance()->getPassthroughTester()),
          config(Environment::getInstance()->getConfig()) {
        link->flushAll();
    }

    template<int MSG>
    double measureInterval(int n_samples) {
        assert(n_samples > 1);
        link->flush<MSG>();
        uint64_t last_received = 0;
        uint64_t total_time = 0;
        for (int i=0; i<n_samples; i++) {
            link->template receive<MSG>(5000000);
            uint64_t now = micros();
            if (last_received != 0) {
                total_time += (now - last_received);
            }
            last_received = now;
        }
        if (last_received == 0) {
            return 0.;
        }
        return 1e6 * static_cast<double>(n_samples -1) / static_cast<double>(total_time);
    }

};


TEST_F(Telemetry, HaveAttitude) {
    double interval = measureInterval<ATTITUDE>(5);
    printf("Attitude interval %.2f Hz\n", interval);
    EXPECT_GT(interval, 0.);
}