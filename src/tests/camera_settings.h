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
        unsigned camera_id{};
        std::string param_name{};
        std::string param_value{};
        std::string sub_param_name{};
        std::string sub_param_value{};

        void serialize(ConfigProvider& c)
        {
            c("camera_id", camera_id);
            c("param_name", param_name);
            c("param_value", param_value);
            c("sub_param_name", sub_param_name);    // FIXME: not used yet
            c("sub_param_value", sub_param_value);  // FIXME: not used yet
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
    void setSettingWithSubSetting();

    dronecode_sdk::Camera::Option getOption(const std::string& setting_id);
    void setOption(const std::string& setting_id, const dronecode_sdk::Camera::Option& option);

    dronecode_sdk::Camera _camera;
    Config _config;
};

}  // namespace tests
