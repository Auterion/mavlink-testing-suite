#pragma once

#include "base.h"
#include "lossy_link.h"

#include <dronecode_sdk/dronecode_sdk.h>
#include <dronecode_sdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <dronecode_sdk/plugins/mission/mission.h>

namespace dcsdk = dronecode_sdk;

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
        float message_loss{0.0f};

        void serialize(ConfigProvider& c) {
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
    std::shared_ptr<dcsdk::MissionItem> makeMissionItem(double latitude_deg, double longitude_deg,
                                                        float relative_altitude_m);

    std::vector<std::shared_ptr<dcsdk::MissionItem>> assembleMissionItems();
    void uploadMission(const std::vector<std::shared_ptr<dcsdk::MissionItem>>& items);
    std::vector<std::shared_ptr<dcsdk::MissionItem>> downloadMission();
    void compareMissions(const std::vector<std::shared_ptr<dcsdk::MissionItem>>& items_a,
                         const std::vector<std::shared_ptr<dcsdk::MissionItem>>& items_b);

    void dropMessages(float ratio);

    dcsdk::Mission _mission;
    dcsdk::MavlinkPassthrough _mavlink_passthrough;
    Config _config;

    LossyLink _lossy_link{};
};

}  // namespace tests
