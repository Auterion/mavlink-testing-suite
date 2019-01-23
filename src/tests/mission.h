#pragma once

#include "base.h"

#include <dronecode_sdk/dronecode_sdk.h>
#include <dronecode_sdk/plugins/mission/mission.h>

namespace dcsdk = dronecode_sdk;
using mission_items = std::vector<std::shared_ptr<dcsdk::MissionItem>>;

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

        void serialize(ConfigProvider& c) { c("num_waypoints", num_waypoints); }
    };

    explicit Mission(const Context& context);
    ~Mission() override = default;

    Result run() override;

protected:
    void serialize(ConfigProvider& c) override { _config.serialize(c); }

private:
    std::shared_ptr<dcsdk::MissionItem> makeMissionItem(double latitude_deg, double longitude_deg,
                                                        float relative_altitude_m);

    mission_items assembleMissionItems();
    void uploadMission(const mission_items& items);
    mission_items downloadMission();
    void compareMissions(const mission_items& items_a, const mission_items& items_b);

    dcsdk::Mission _mission;
    Config _config;
};

}  // namespace tests
