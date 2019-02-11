
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
    : TestBase(context), _mission(context.system), _mavlink_passthrough(context.system)
{
}

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
    std::vector<std::shared_ptr<dcsdk::MissionItem>> items = assembleMissionItems();

    uploadMission(items);
    std::vector<std::shared_ptr<dcsdk::MissionItem>> downloaded_items = downloadMission();

    compareMissions(items, downloaded_items);
}

std::vector<std::shared_ptr<dcsdk::MissionItem>> Mission::assembleMissionItems()
{
    cout << "Number of waypoints: " << _config.num_waypoints << endl;

    std::vector<std::shared_ptr<dcsdk::MissionItem>> items{};

    for (int i = 0; i < _config.num_waypoints; ++i) {
        float altitude = 10.f + (float)i;
        double latitude = 47.398170327054473 + (double)i * 1e-5;
        items.push_back(makeMissionItem(latitude, 8.5456490218639658, altitude));
    }

    return items;
}

void Mission::uploadMission(const std::vector<std::shared_ptr<dcsdk::MissionItem>>& items)
{
    cout << "Uploading mission..." << endl;
    promise<dcsdk::Mission::Result> prom{};
    auto fut = prom.get_future();

    _mission.upload_mission_async(
        items, [&prom](dcsdk::Mission::Result result) { prom.set_value(result); });

    // wait until uploaded
    const dcsdk::Mission::Result result = fut.get();
    ASSERT_EQ(result, dcsdk::Mission::Result::SUCCESS);
}

std::vector<std::shared_ptr<dcsdk::MissionItem>> Mission::downloadMission()
{
    cout << "Downloading mission..." << endl;
    promise<pair<dcsdk::Mission::Result, std::vector<std::shared_ptr<dcsdk::MissionItem>>>> prom{};
    auto fut = prom.get_future();

    _mission.download_mission_async(
        [&prom](dcsdk::Mission::Result result,
                std::vector<std::shared_ptr<dcsdk::MissionItem>> items) {
            prom.set_value(make_pair(result, items));
        });

    auto value = fut.get();
    const dcsdk::Mission::Result& result = value.first;
    const std::vector<std::shared_ptr<dcsdk::MissionItem>>& items = value.second;

    // wait until uploaded
    ASSERT_EQ(result, dcsdk::Mission::Result::SUCCESS);

    return items;
}

void Mission::compareMissions(const std::vector<std::shared_ptr<dcsdk::MissionItem>>& items_a,
                              const std::vector<std::shared_ptr<dcsdk::MissionItem>>& items_b)
{
    EXPECT_EQ(items_a.size(), items_b.size());

    if (items_a.size() != items_b.size()) {
        return;
    }

    for (unsigned i = 0; i < items_a.size(); ++i) {
        EXPECT_EQ(*(items_a[i]), *(items_b[i]));
    }
}

}  // namespace tests
