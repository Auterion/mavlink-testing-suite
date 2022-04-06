#include <gtest/gtest.h>
#include <sys/time.h>
#include "../environment.hpp"
using namespace MavlinkTestingSuite;

// Get current timestamp in microseconds
inline uint64_t micros() {
    struct timeval t;
    gettimeofday(&t, nullptr);
    return t.tv_sec * ((uint64_t)1000000) + t.tv_usec;
}


class Telemetry : public ::testing::Test {
protected:
    const std::shared_ptr<PassthroughTester> link;
    const YAML::Node config;

    static constexpr double RATE_MARGIN = 0.2;

    Telemetry() :
          link(Environment::getInstance()->getPassthroughTester()),
          config(Environment::getInstance()->getConfig()) {
        link->flushAll();
    }

    template<int MSG>
    double measureRate(int n_samples) {
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

    double scaledRate(double rate) {
        double sim_factor = config["Telemetry"]["sim_factor"].as<double>();
        return (rate / sim_factor) + RATE_MARGIN;
    }

};

TEST_F(Telemetry, HaveHeartbeat) {
    double freq = measureRate<HEARTBEAT>(3);
    printf("HEARTBEAT interval %.2f Hz", freq);
    EXPECT_GT(scaledRate(freq), 1.);
}


TEST_F(Telemetry, HaveBatteryStatus) {
    double freq = measureRate<BATTERY_STATUS>(2);
    printf("BATTERY_STATUS interval %.2f Hz", freq);
    EXPECT_GT(scaledRate(freq), 1.);
}

TEST_F(Telemetry, HaveSysStatus) {
    double freq = measureRate<SYS_STATUS>(2);
    printf("SYS_STATUS interval %.2f Hz\n", freq);
    EXPECT_GT(scaledRate(freq), 1.);
}

TEST_F(Telemetry, HaveExtendedSysStatus) {
    double freq = measureRate<EXTENDED_SYS_STATE>(2);
    printf("EXTENDED_SYS_STATE interval %.2f Hz\n", freq);
    EXPECT_GT(scaledRate(freq), 1.);
}

TEST_F(Telemetry, HaveGPSRaw) {
    double freq = measureRate<GPS_RAW_INT>(3);
    printf("GPS_RAW_INT interval %.2f Hz\n", freq);
    EXPECT_GT(scaledRate(freq), 5.);
}

TEST_F(Telemetry, HaveGlobalPosition) {
    double freq = measureRate<GLOBAL_POSITION_INT>(3);
    printf("GLOBAL_POSITION_INT interval %.2f Hz\n", freq);
    EXPECT_GT(scaledRate(freq), 5.);
}

TEST_F(Telemetry, HaveAltitude) {
    double freq = measureRate<ALTITUDE>(3);
    printf("ALTITUDE interval %.2f Hz\n", freq);
    EXPECT_GT(scaledRate(freq), 5.);
}

TEST_F(Telemetry, HaveAttitude) {
    double freq = measureRate<ATTITUDE>(5);
    printf("ATTITUDE interval %.2f Hz\n", freq);
    EXPECT_GT(scaledRate(freq), 15.);
}

TEST_F(Telemetry, HaveEstimatorStatus) {
    double freq = measureRate<ESTIMATOR_STATUS>(2);
    printf("ESTIMATOR_STATUS interval %.2f Hz\n", freq);
    EXPECT_GT(scaledRate(freq), 1.);
}

//TEST_F(Telemetry, HaveAttitudeQuaternion) {
//    double freq = measureRate<ATTITUDE_QUATERNION>(5);
//    printf("ATTITUDE_QUATERNION interval %.2f Hz\n", freq);
//    EXPECT_GT(scaledRate(freq), 0.);
//}

//TEST_F(Telemetry, HaveAttitudeTarget) {
//    double freq = measureRate<ATTITUDE_TARGET>(5);
//    printf("ATTITUDE_TARGET interval %.2f Hz\n", freq);
//    EXPECT_GT(scaledRate(freq), 0.);
//}

//TEST_F(Telemetry, HaveHomePosition) {
//    double freq = measureRate<HOME_POSITION>(2);
//    printf("HOME_POSITION interval %.2f Hz\n", freq);
//    EXPECT_GT(scaledRate(freq), 0.);
//}

//TEST_F(Telemetry, HaveLocalPosition) {
//    double freq = measureRate<LOCAL_POSITION_NED>(5);
//    printf("LOCAL_POSITION_NED interval %.2f Hz\n", freq);
//    EXPECT_GT(scaledRate(freq), 0.);
//}

//TEST_F(Telemetry, HavePositionTarget) {
//    double freq = measureRate<POSITION_TARGET_LOCAL_NED>(5);
//    printf("POSITION_TARGET_LOCAL_NED interval %.2f Hz\n", freq);
//    EXPECT_GT(scaledRate(freq), 0.);
//}

//TEST_F(Telemetry, HaveVFRHUD) {
//    double freq = measureRate<VFR_HUD>(3);
//    printf("VFR_HUD interval %.2f Hz\n", freq);
//    EXPECT_GT(scaledRate(freq), 0.);
//}

