#pragma once

#include "base.h"

#include <dronecode_sdk/mission.h>
#include <dronecode_sdk/system.h>

namespace tests
{
/**
 * @class MissionUpload
 * Test Mission Upload
 */
class MissionUpload : public TestBase
{
public:
	struct Config {
		int num_waypoints{10};

		void serialize(ConfigProvider& c) { c("num_waypoints", num_waypoints); }
	};

	explicit MissionUpload(const Context& context);
	~MissionUpload() override = default;

	Result run() override;

protected:
	void serialize(ConfigProvider& c) override { _config.serialize(c); }

private:
	std::shared_ptr<dronecode_sdk::MissionItem> makeMissionItem(double latitude_deg, double longitude_deg,
	                                                            float relative_altitude_m, float speed_m_s,
	                                                            bool is_fly_through, float gimbal_pitch_deg,
	                                                            float gimbal_yaw_deg,
	                                                            dronecode_sdk::MissionItem::CameraAction camera_action);

	dronecode_sdk::Mission _mission;
	Config _config;
};

}  // namespace tests
