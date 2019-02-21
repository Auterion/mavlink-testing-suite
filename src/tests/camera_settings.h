#pragma once

#include "base.h"

#include <dronecode_sdk/dronecode_sdk.h>
#include <dronecode_sdk/plugins/camera/camera.h>

namespace tests
{
/**
 * @class CameraSettings
 * Test setting and getting camera parameters.
 */
class CameraSettings : public TestBase
{
public:
    struct Config {
        std::string param_name{};
        unsigned camera_id{};

        void serialize(ConfigProvider& c)
        {
            c("param_name", param_name);
            c("camera_id", camera_id);
        }
    };

    explicit CameraSettings(const Context& context);
    ~CameraSettings() override = default;

    void run() override;

protected:
    void serialize(ConfigProvider& c) override { _config.serialize(c); }

private:
    void selectCamera();
    void getPossibleSettings();
    void getAndSetPossibleOptions();

    dronecode_sdk::Camera _camera;
    Config _config;
};

}  // namespace tests
