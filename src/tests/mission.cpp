
#include "mission.h"
#include <future>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

namespace tests
{
REGISTER_TEST(Mission);

Mission::Mission(const Context& context)
    : TestBase(context),
      _mission(context.system),
      _mavlink_passthrough(context.system),
      _lossy_link_incoming(0),
      _lossy_link_outgoing(1)
{
}

mavsdk::Mission::MissionItem Mission::makeMissionItem(double latitude_deg, double longitude_deg,
                                                      float relative_altitude_m)
{
    mavsdk::Mission::MissionItem new_item{};
    new_item.latitude_deg = latitude_deg;
    new_item.longitude_deg = longitude_deg;
    new_item.relative_altitude_m = relative_altitude_m;
    return new_item;
}

void Mission::run()
{
    // FIXME: we need to let the mission plugin initialize first.
    std::this_thread::sleep_for(std::chrono::seconds(3));

    dropMessages(_config.message_loss);
    uploadDownloadCompare();
    eraseMission();
}

void Mission::uploadDownloadCompare()
{
    const auto plan = assembleMissionPlan();

    uploadMission(plan);
    const auto downloaded_plan = downloadMission();

    compareMissions(plan, downloaded_plan);
}

void Mission::eraseMission()
{
    cout << "Clearing mission..." << endl;
    auto result = _mission.clear_mission();
    ASSERT_EQ(result, mavsdk::Mission::Result::Success);
}

mavsdk::Mission::MissionPlan Mission::assembleMissionPlan()
{
    cout << "Number of waypoints: " << _config.num_waypoints << endl;

    mavsdk::Mission::MissionPlan plan;

    for (int i = 0; i < _config.num_waypoints; ++i) {
        float altitude = 10.F + (float)i;
        double latitude = 47.398170327054473 + (double)i * 1e-5;
        plan.mission_items.push_back(makeMissionItem(latitude, 8.5456490218639658, altitude));
    }

    return plan;
}

void Mission::uploadMission(const mavsdk::Mission::MissionPlan& plan)
{
    cout << "Uploading mission..." << endl;
    promise<mavsdk::Mission::Result> prom{};
    auto fut = prom.get_future();

    _mission.upload_mission_async(
        plan, [&prom](mavsdk::Mission::Result result) { prom.set_value(result); });

    // wait until uploaded
    const mavsdk::Mission::Result result = fut.get();
    ASSERT_EQ(result, mavsdk::Mission::Result::Success);
}

const mavsdk::Mission::MissionPlan Mission::downloadMission()
{
    cout << "Downloading mission..." << endl;

    auto result = _mission.download_mission();

    // wait until uploaded
    ASSERT_EQ(result.first, mavsdk::Mission::Result::Success);

    return result.second;
}

void Mission::compareMissions(const mavsdk::Mission::MissionPlan& plan_a,
                              const mavsdk::Mission::MissionPlan& plan_b)
{
    EXPECT_EQ(plan_a.mission_items.size(), plan_b.mission_items.size());

    if (plan_a.mission_items.size() != plan_b.mission_items.size()) {
        return;
    }

    for (std::size_t i = 0; i < plan_a.mission_items.size();
         ++i) {
        EXPECT_EQ(plan_a.mission_items[i], plan_b.mission_items[i]);
    }
}

void Mission::dropMessages(const float ratio)
{
    _mavlink_passthrough.intercept_incoming_messages_async(
        [this, ratio](mavlink_message_t& message) {
            unused(message);
            return shouldDropMissionMessage(message, ratio);
        });

    _mavlink_passthrough.intercept_outgoing_messages_async(
        [this, ratio](mavlink_message_t& message) {
            unused(message);
            return shouldDropMissionMessage(message, ratio);
        });
}

bool Mission::shouldDropMissionMessage(const mavlink_message_t& message, const float ratio)
{
    bool should_keep = true;
    if (message.msgid == MAVLINK_MSG_ID_MISSION_ITEM_INT ||
        message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_INT ||
        message.msgid == MAVLINK_MSG_ID_MISSION_COUNT ||
        message.msgid == MAVLINK_MSG_ID_MISSION_REQUEST_LIST /* ||
        message.msgid == MAVLINK_MSG_ID_MISSION_ACK*/) {
        // TODO: we need to check if MISSION_ACK can be dropped.
        should_keep = !_lossy_link_incoming.drop(ratio);
    }
    return should_keep;
}

}  // namespace tests
