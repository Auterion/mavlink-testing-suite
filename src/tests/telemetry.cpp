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
            link->template receive<MSG>(5000);
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

TEST_F(Telemetry, HaveHeartbeat) {
    double freq = measureInterval<HEARTBEAT>(3);
    printf("HEARTBEAT interval %.2f Hz", freq);
    EXPECT_GT(freq, 0.);
}


TEST_F(Telemetry, HaveBatteryStatus) {
    double freq = measureInterval<BATTERY_STATUS>(2);
    printf("BATTERY_STATUS interval %.2f Hz", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HaveSysStatus) {
    double freq = measureInterval<SYS_STATUS>(2);
    printf("SYS_STATUS interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HaveExtendedSysStatus) {
    double freq = measureInterval<EXTENDED_SYS_STATE>(2);
    printf("EXTENDED_SYS_STATE interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HaveGPSRaw) {
    double freq = measureInterval<GPS_RAW_INT>(3);
    printf("GPS_RAW_INT interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HaveGlobalPosition) {
    double freq = measureInterval<GLOBAL_POSITION_INT>(3);
    printf("GLOBAL_POSITION_INT interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HaveAltitude) {
    double freq = measureInterval<ALTITUDE>(3);
    printf("ALTITUDE interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HaveVFRHUD) {
    double freq = measureInterval<VFR_HUD>(3);
    printf("VFR_HUD interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HaveAttitudeQuaternion) {
    double freq = measureInterval<ATTITUDE_QUATERNION>(5);
    printf("ATTITUDE_QUATERNION interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HaveAttitudeTarget) {
    double freq = measureInterval<ATTITUDE_TARGET>(5);
    printf("ATTITUDE_TARGET interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HaveHomePosition) {
    double freq = measureInterval<HOME_POSITION>(2);
    printf("HOME_POSITION interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HaveLocalPosition) {
    double freq = measureInterval<LOCAL_POSITION_NED>(5);
    printf("LOCAL_POSITION_NED interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}

TEST_F(Telemetry, HavePositionTarget) {
    double freq = measureInterval<POSITION_TARGET_LOCAL_NED>(5);
    printf("POSITION_TARGET_LOCAL_NED interval %.2f Hz\n", freq);
    EXPECT_GT(freq, 0.);
}



