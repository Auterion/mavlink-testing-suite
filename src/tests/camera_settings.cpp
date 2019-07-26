
#include "camera_settings.h"
#include <algorithm>
#include <chrono>
#include <future>
#include <thread>
#include <vector>

using namespace std;
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
    getPossibleSettings();
    getAndSetPossibleOptions();
    setSettingWithSubSetting();
}

void CameraSettings::selectCamera()
{
    _camera.select_camera(_config.camera_id);
    // FIXME: we need to let the camera plugin initialize after this.
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void CameraSettings::getPossibleSettings()
{
    std::cout << "Checking if " << _config.param_name << " is a possible setting." << std::endl;
    std::vector<std::string> settings;
    _camera.get_possible_setting_options(settings);

    const bool found_setting =
        std::any_of(std::begin(settings), std::end(settings),
                    [this](const std::string& setting) { return (setting == _config.param_name); });

    ASSERT_EQ(found_setting, true);
}

void CameraSettings::getAndSetPossibleOptions()
{
    Camera::Option initial_option = getOption(_config.param_name);

    std::cout << "Getting possible settings for " << _config.param_name << std::endl;
    std::vector<Camera::Option> options;
    _camera.get_possible_options(_config.param_name, options);

    for (const auto& option : options) {
        std::cout << "- " << option.option_description << " (" << option.option_id << ")"
                  << std::endl;
        setOption(_config.param_name, option);
        Camera::Option set_option = getOption(_config.param_name);
        EXPECT_EQ(set_option, option);
    }
    std::cout << "Resetting setting to initial option." << std::endl;
    setOption(_config.param_name, initial_option);
}

void CameraSettings::setSettingWithSubSetting()
{
    Camera::Option initial_option = getOption(_config.param_name);

    std::cout << "Getting possible settings for " << _config.param_name << std::endl;
    std::vector<Camera::Option> options;
    _camera.get_possible_options(_config.param_name, options);

    bool option_available = false;
    for (const auto& option : options) {
        if (option.option_id != _config.param_value) {
            continue;
        }
        option_available = true;
        setOption(_config.param_name, option);

        // TODO: add support for Options with range type (e.g. wb-temp 4000..10000).
    }

    EXPECT_EQ(option_available, true);

    std::cout << "Resetting setting to initial option." << std::endl;
    setOption(_config.param_name, initial_option);
}

Camera::Option CameraSettings::getOption(const std::string& setting_id)
{
    std::cout << "Getting option for " << setting_id << std::endl;
    Camera::Option option;
    ASSERT_EQ(_camera.get_option(setting_id, option), Camera::Result::SUCCESS);
    return option;
}

void CameraSettings::setOption(const std::string& setting_id, const Camera::Option& option)
{
    std::cout << "Setting " << setting_id << " to " << option.option_id << std::endl;
    std::promise<Camera::Result> prom;
    std::future<Camera::Result> fut = prom.get_future();
    _camera.set_option_async([&prom](Camera::Result set_result) { prom.set_value(set_result); },
                             setting_id, option);
    auto fut_ret = fut.wait_for(std::chrono::seconds(2));
    EXPECT_EQ(fut_ret, std::future_status::ready);
    if (fut_ret == std::future_status::ready) {
        EXPECT_EQ(fut.get(), Camera::Result::SUCCESS);
    }
}

}  // namespace tests
