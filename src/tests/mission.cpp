
#include "mission.h"
#include <future>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

namespace tests
{
REGISTER_TEST(Mission);

Mission::Mission(const Context& context) : TestBase(context), _mission(context.system) {}

shared_ptr<dcsdk::MissionItem> Mission::makeMissionItem(double latitude_deg, double longitude_deg,
                                                        float relative_altitude_m)
{
    shared_ptr<dcsdk::MissionItem> new_item = make_shared<dcsdk::MissionItem>();
    new_item->set_position(latitude_deg, longitude_deg);
    new_item->set_relative_altitude(relative_altitude_m);
    return new_item;
}

TestBase::Result Mission::run()
{
    mission_items items = assembleMissionItems();

    uploadMission(items);
    mission_items downloaded_items = downloadMission();

    compareMissions(items, downloaded_items);

    return Result::Success;
}

mission_items Mission::assembleMissionItems()
{
    cout << "Number of waypoints: " << _config.num_waypoints << endl;

    mission_items items{};

    for (int i = 0; i < _config.num_waypoints; ++i) {
        float altitude = 10.f + (float)i;
        double latitude = 47.398170327054473 + (double)i * 1e-5;
        items.push_back(makeMissionItem(latitude, 8.5456490218639658, altitude));
    }

    return std::move(items);
}

void Mission::uploadMission(const mission_items& items)
{
    cout << "Uploading mission..." << endl;
    promise<dcsdk::Mission::Result> prom{};
    auto fut = prom.get_future();

    _mission.upload_mission_async(items, [&prom](dcsdk::Mission::Result result) { prom.set_value(result); });

    // wait until uploaded
    const dcsdk::Mission::Result result = fut.get();
    if (result != dcsdk::Mission::Result::SUCCESS) {
        cout << "Mission upload failed (" << dcsdk::Mission::result_str(result) << "), exiting." << endl;
        // TODO: we need a mechanism to fail.
        // return Result::Failed;
    }
    cout << "Mission uploaded." << endl;
}

mission_items Mission::downloadMission()
{
    cout << "Downloading mission..." << endl;
    promise<pair<dcsdk::Mission::Result, mission_items>> prom{};
    auto fut = prom.get_future();

    _mission.download_mission_async(
        [&prom](dcsdk::Mission::Result result, mission_items items) { prom.set_value(make_pair<>(result, items)); });

    auto value = fut.get();
    const dcsdk::Mission::Result& result = value.first;
    const mission_items& items = value.second;

    // wait until uploaded
    if (result != dcsdk::Mission::Result::SUCCESS) {
        cout << "Mission upload failed (" << dcsdk::Mission::result_str(fut.get().first) << "), exiting." << endl;
        // TODO: we need a mechanism to fail.
        // return Result::Failed;
    }
    cout << "Mission uploaded." << endl;

    return items;
}

void Mission::compareMissions(const mission_items& items_a, const mission_items& items_b)
{
    cout << "Not comparing missions yet" << endl;
    // TODO: Compare mission items but we need something like gtest for this.
    //       Also, it would be nice to use == on a MissionItem.
}

}  // namespace tests
