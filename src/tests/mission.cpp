
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

void Mission::run()
{
    mission_items items = assembleMissionItems();

    uploadMission(items);
    mission_items downloaded_items = downloadMission();

    compareMissions(items, downloaded_items);
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

    return items;
}

void Mission::uploadMission(const mission_items& items)
{
    cout << "Uploading mission..." << endl;
    promise<dcsdk::Mission::Result> prom{};
    auto fut = prom.get_future();

    _mission.upload_mission_async(
        items, [&prom](dcsdk::Mission::Result result) { prom.set_value(result); });

    // wait until uploaded
    const dcsdk::Mission::Result result = fut.get();
    EXPECT_EQ(result, dcsdk::Mission::Result::SUCCESS);
}

mission_items Mission::downloadMission()
{
    cout << "Downloading mission..." << endl;
    promise<pair<dcsdk::Mission::Result, mission_items>> prom{};
    auto fut = prom.get_future();

    _mission.download_mission_async([&prom](dcsdk::Mission::Result result, mission_items items) {
        prom.set_value(make_pair(result, items));
    });

    auto value = fut.get();
    const dcsdk::Mission::Result& result = value.first;
    const mission_items& items = value.second;

    // wait until uploaded
    EXPECT_EQ(result, dcsdk::Mission::Result::SUCCESS);

    return items;
}

void Mission::compareMissions(const mission_items& items_a, const mission_items& items_b)
{
    EXPECT_EQ(items_a.size(), items_b.size());

    for (unsigned i = 0; i < items_a.size(); ++i) {
        EXPECT_EQ(*(items_a[i]), *(items_b[i]));
    }
}

}  // namespace tests
