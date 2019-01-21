
#include "mission.h"
#include <future>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace dronecode_sdk;

namespace tests
{
REGISTER_TEST(MissionUpload);

MissionUpload::MissionUpload(const Context& context) : TestBase(context), _mission(context.system) {}

shared_ptr<MissionItem> MissionUpload::makeMissionItem(double latitude_deg, double longitude_deg,
                                                       float relative_altitude_m)
{
    shared_ptr<MissionItem> new_item = make_shared<MissionItem>();
    new_item->set_position(latitude_deg, longitude_deg);
    new_item->set_relative_altitude(relative_altitude_m);
    return new_item;
}

TestBase::Result MissionUpload::run()
{
    cout << "Number of waypoints: " << _config.num_waypoints << endl;

    vector<shared_ptr<MissionItem>> mission_items;

    for (int i = 0; i < _config.num_waypoints; ++i) {
        float altitude = 10.f + (float)i;
        double latitude = 47.398170327054473 + (double)i * 1e-5;
        mission_items.push_back(makeMissionItem(latitude, 8.5456490218639658, altitude));
    }

    cout << "Uploading mission..." << endl;
    auto prom = make_shared<promise<Mission::Result>>();
    auto future_result = prom->get_future();
    _mission.upload_mission_async(mission_items, [prom](Mission::Result result) { prom->set_value(result); });

    // wait until uploaded
    const Mission::Result result = future_result.get();
    if (result != Mission::Result::SUCCESS) {
        cout << "Mission upload failed (" << Mission::result_str(result) << "), exiting." << endl;
        return Result::Failed;
    }
    cout << "Mission uploaded." << endl;

    return Result::Success;
}

}  // namespace tests
