#pragma once

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>

#include "base.h"

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
        bool param_is_range{false};

        void serialize(ConfigProvider& c)
        {
            c("camera_id", camera_id);
            c("param_name", param_name);
            c("param_is_range", param_is_range);
        }
    };

    explicit CameraSettings(const Context& context);
    ~CameraSettings() override = default;

    void run() override;

protected:
    void serialize(ConfigProvider& c) override { _config.serialize(c); }

private:
    void selectCamera();
    mavsdk::Camera::SettingOptions getPossibleSettingOptions();
    void getAndSetPossibleOptions(const mavsdk::Camera::SettingOptions& setting_options);
    void setAllEnumOptions(const mavsdk::Camera::SettingOptions& setting_options);
    void setSomeRangeOptions(const mavsdk::Camera::SettingOptions& setting_options);

    mavsdk::Camera::Option getOption(const std::string& setting_id);
    void setOption(const std::string& setting_id, const mavsdk::Camera::Option& option);

    mavsdk::Camera _camera;
    Config _config;
};

}  // namespace tests
