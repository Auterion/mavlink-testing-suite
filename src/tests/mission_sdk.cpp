#include <gtest/gtest.h>
#include "../environment.hpp"
#include <future>
#include <memory>
#include <vector>

using namespace MavlinkTestingSuite;

class MissionSDK : public ::testing::Test {
protected:
    const std::shared_ptr<mavsdk::Mission> mission;
    const YAML::Node config;

    MissionSDK() :
    mission(Environment::getInstance()->getMissionPlugin()),
    config(Environment::getInstance()->getConfig()) {}
};


mavsdk::Mission::MissionItem makeMissionItem(double latitude_deg, double longitude_deg,
                                                      float relative_altitude_m) {
    mavsdk::Mission::MissionItem new_item{};
    new_item.latitude_deg = latitude_deg;
    new_item.longitude_deg = longitude_deg;
    new_item.relative_altitude_m = relative_altitude_m;
    new_item.acceptance_radius_m = 1.;
    return new_item;
}

mavsdk::Mission::MissionPlan assembleMissionPlan() {
    mavsdk::Mission::MissionPlan plan;

    for (int i = 0; i < 15; ++i) {
        float altitude = 10.F + (float)i;
        double latitude = 47.398170327054473 + (double)i * 1e-5;
        plan.mission_items.push_back(makeMissionItem(latitude, 8.5456490218639658, altitude));
    }
    return plan;
}


TEST_F(MissionSDK, UploadDownloadCompare) {
    const auto plan = assembleMissionPlan();

    // -- Upload mission --
    std::promise<mavsdk::Mission::Result> prom{};
    auto fut = prom.get_future();

    mission->upload_mission_async(
        plan, [&prom](mavsdk::Mission::Result result) { prom.set_value(result); });

    // wait until uploaded
    fut.wait_for(std::chrono::seconds(1));
    const mavsdk::Mission::Result result = fut.get();
    ASSERT_EQ(result, mavsdk::Mission::Result::Success);

    // -- Download mission --
    auto dl_result = mission->download_mission();

    // wait until downloaded
    ASSERT_EQ(dl_result.first, mavsdk::Mission::Result::Success);

    const auto downloaded_plan = dl_result.second;

    // -- Compare planned mission with downloaded mission --
    EXPECT_EQ(plan.mission_items.size(), downloaded_plan.mission_items.size());

    for (std::size_t i = 0; i < downloaded_plan.mission_items.size(); ++i) {
        EXPECT_EQ(plan.mission_items[i], downloaded_plan.mission_items[i]);
    }

    // -- Clear mission --
    auto clear_result = mission->clear_mission();
    ASSERT_EQ(clear_result, mavsdk::Mission::Result::Success);
}
