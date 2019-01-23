#pragma once

#include "base.h"

#include <dronecode_sdk/dronecode_sdk.h>
#include <dronecode_sdk/plugins/mission/mission.h>

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
    std::shared_ptr<dronecode_sdk::MissionItem> makeMissionItem(double latitude_deg, double longitude_deg,
                                                                float relative_altitude_m);

    dronecode_sdk::Mission _mission;
    Config _config;
};

}  // namespace tests
