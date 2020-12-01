#pragma once

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/mission/mission.h>

#include "base.h"
#include "lossy_link.h"

namespace tests
{
/**
 * @class Mission
 * Test Mission Upload
 */
class Mission : public TestBase
{
public:
    struct Config {
        int num_waypoints{10};
        float message_loss{0.0F};

        void serialize(ConfigProvider& c)
        {
            c("num_waypoints", num_waypoints);
            c("message_loss", message_loss);
        }
    };

    explicit Mission(const Context& context);
    ~Mission() override = default;

    void run() override;

protected:
    void serialize(ConfigProvider& c) override { _config.serialize(c); }

private:
    void uploadDownloadCompare();
    void eraseMission();
    mavsdk::Mission::MissionItem makeMissionItem(double latitude_deg, double longitude_deg,
                                                 float relative_altitude_m);

    mavsdk::Mission::MissionPlan assembleMissionPlan();
    void uploadMission(const mavsdk::Mission::MissionPlan& plan);
    const mavsdk::Mission::MissionPlan downloadMission();
    void compareMissions(const mavsdk::Mission::MissionPlan& plan_a,
                         const mavsdk::Mission::MissionPlan& plan_b);

    void dropMessages(float ratio);
    bool shouldDropMissionMessage(const mavlink_message_t& message, float ratio);

    mavsdk::Mission _mission;
    mavsdk::MavlinkPassthrough _mavlink_passthrough;
    Config _config;

    LossyLink _lossy_link_incoming;
    LossyLink _lossy_link_outgoing;
};

}  // namespace tests
