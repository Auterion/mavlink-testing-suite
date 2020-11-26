
#include "camera_settings.h"
#include <algorithm>
#include <chrono>
#include <future>
#include <thread>
#include <vector>

using namespace mavsdk;

namespace tests
{
REGISTER_TEST(CameraSettings);

CameraSettings::CameraSettings(const Context& context) : TestBase(context), _camera(context.system)
{
}

void CameraSettings::run()
{
    selectCamera();
    const auto setting_options = getPossibleSettingOptions();
    getAndSetPossibleOptions(setting_options);
}

void CameraSettings::selectCamera()
{
    // FIXME: looks like this interface is gone.
    //_camera.select_camera(_config.camera_id);
    // FIXME: we need to let the camera plugin initialize after this.
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

mavsdk::Camera::SettingOptions CameraSettings::getPossibleSettingOptions()
{
    std::cout << "Checking if " << _config.param_name << " is a possible setting." << std::endl;
    auto setting_options = _camera.possible_setting_options();

    const auto found =
        std::find_if(std::begin(setting_options), std::end(setting_options),
                    [this](const Camera::SettingOptions& setting_options) { return (setting_options.setting_id == _config.param_name); });

    ASSERT_TRUE(found != setting_options.end());

    return *found;
}

void CameraSettings::getAndSetPossibleOptions(const mavsdk::Camera::SettingOptions& setting_options)
{
    mavsdk::Camera::Setting setting;
    setting.setting_id = _config.param_name;

    const auto initial_settings = _camera.get_setting(setting);

    ASSERT_EQ(initial_settings.first, mavsdk::Camera::Result::Success);

    ASSERT_EQ(_config.param_is_range, initial_settings.second.is_range);

    if (initial_settings.second.is_range) {
        setSomeRangeOptions(setting_options);
    } else {
        setAllEnumOptions(setting_options);
    }

    std::cout << "Resetting setting to initial option." << std::endl;
    setOption(_config.param_name, initial_settings.second.option);
}

void CameraSettings::setAllEnumOptions(const mavsdk::Camera::SettingOptions& setting_options)
{
    for (const auto& option : setting_options.options) {
        std::cout << "- " << option.option_description << " (" << option.option_id << ")"
                  << std::endl;
        setOption(_config.param_name, option);
        Camera::Option set_option = getOption(_config.param_name);
        EXPECT_EQ(set_option, option);
    }
}

void CameraSettings::setSomeRangeOptions(const mavsdk::Camera::SettingOptions& setting_options)
{
    ASSERT_TRUE(setting_options.options.size() >= 2);
    ASSERT_TRUE(setting_options.options.size() <= 3);

    // FIXME: dealing with this interface is not great and needs improvement.

    const auto minimum = std::atoi(setting_options.options[0].option_id.c_str());
    {
        Camera::Option new_option;
        new_option.option_id = std::to_string(minimum);
        setOption(_config.param_name, new_option);
        Camera::Option set_option = getOption(_config.param_name);
        EXPECT_EQ(set_option, new_option);
    }

    const auto maximum = std::atoi(setting_options.options[1].option_id.c_str());
    {
        Camera::Option new_option;
        new_option.option_id = std::to_string(maximum);
        setOption(_config.param_name, new_option);
        Camera::Option set_option = getOption(_config.param_name);
        EXPECT_EQ(set_option, new_option);
    }

    EXPECT_TRUE(maximum > minimum);

    if (setting_options.options.size() == 2) {
        // We are only given min and max, no interval so any increment should work.
        const auto center = (maximum - minimum) / 2 + minimum;
        Camera::Option new_option;
        new_option.option_id = std::to_string(center);
        setOption(_config.param_name, new_option);
        Camera::Option set_option = getOption(_config.param_name);
        EXPECT_EQ(set_option.option_id, std::to_string(center));

    } else {
        const auto interval = std::atoi(setting_options.options[2].option_id.c_str());

        EXPECT_EQ((maximum - minimum) % interval, 0);

        // We use the interval to get something close to the center.
        const auto center = ((maximum - minimum) / 2) / interval * interval;
        Camera::Option new_option;
        new_option.option_id = std::to_string(center);
        setOption(_config.param_name, new_option);
        Camera::Option set_option = getOption(_config.param_name);
        EXPECT_EQ(set_option.option_id, std::to_string(center));
    }
}

Camera::Option CameraSettings::getOption(const std::string& setting_id)
{
    std::cout << "Getting option for " << setting_id << std::endl;

    Camera::Setting setting;
    setting.setting_id = setting_id;
    const auto result = _camera.get_setting(setting);
    ASSERT_EQ(result.first, Camera::Result::Success);
    return result.second.option;
}

void CameraSettings::setOption(const std::string& setting_id, const Camera::Option& option)
{
    std::cout << "Setting " << setting_id << " to " << option.option_id << std::endl;
    Camera::Setting setting;
    setting.setting_id = setting_id;
    setting.option = option;
    auto result = _camera.set_setting(setting);
    EXPECT_EQ(result, Camera::Result::Success);
}

}  // namespace tests
