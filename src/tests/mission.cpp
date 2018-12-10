
#include "mission.h"
#include <future>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace dronecode_sdk;

namespace tests
{
MissionUpload::MissionUpload(const Context& context) : TestBase(context), _mission(context.system) {}

shared_ptr<MissionItem> MissionUpload::make_mission_item(double latitude_deg, double longitude_deg,
                                                         float relative_altitude_m, float speed_m_s,
                                                         bool is_fly_through, float gimbal_pitch_deg,
                                                         float gimbal_yaw_deg, MissionItem::CameraAction camera_action)
{
	shared_ptr<MissionItem> new_item = make_shared<MissionItem>();
	new_item->set_position(latitude_deg, longitude_deg);
	new_item->set_relative_altitude(relative_altitude_m);
	new_item->set_speed(speed_m_s);
	new_item->set_fly_through(is_fly_through);
	new_item->set_gimbal_pitch_and_yaw(gimbal_pitch_deg, gimbal_yaw_deg);
	new_item->set_camera_action(camera_action);
	return new_item;
}

TestBase::Result MissionUpload::run()
{
	// TODO: use configuration
	cout << "number of waypoints: " << _config.num_waypoints << endl;

	vector<shared_ptr<MissionItem>> mission_items;

	mission_items.push_back(make_mission_item(47.398170327054473, 8.5456490218639658, 10.0f, 5.0f, false, 20.0f, 60.0f,
	                                          MissionItem::CameraAction::NONE));

	mission_items.push_back(make_mission_item(47.398241338125118, 8.5455360114574432, 10.0f, 2.0f, true, 0.0f, -60.0f,
	                                          MissionItem::CameraAction::TAKE_PHOTO));

	mission_items.push_back(make_mission_item(47.398139363821485, 8.5453846156597137, 10.0f, 5.0f, true, -45.0f, 0.0f,
	                                          MissionItem::CameraAction::START_VIDEO));

	mission_items.push_back(make_mission_item(47.398058617228855, 8.5454618036746979, 10.0f, 2.0f, false, -90.0f, 30.0f,
	                                          MissionItem::CameraAction::STOP_VIDEO));

	mission_items.push_back(make_mission_item(47.398100366082858, 8.5456969141960144, 10.0f, 5.0f, false, -45.0f,
	                                          -30.0f, MissionItem::CameraAction::START_PHOTO_INTERVAL));

	mission_items.push_back(make_mission_item(47.398001890458097, 8.5455576181411743, 10.0f, 5.0f, false, 0.0f, 0.0f,
	                                          MissionItem::CameraAction::STOP_PHOTO_INTERVAL));

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
