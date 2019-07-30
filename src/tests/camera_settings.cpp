
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
    getPossibleSettingOptions();
    getAndSetPossibleOptions();
}

void CameraSettings::selectCamera()
{
    _camera.select_camera(_config.camera_id);
    // FIXME: we need to let the camera plugin initialize after this.
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void CameraSettings::getPossibleSettingOptions()
{
    std::cout << "Checking if " << _config.param_name << " is a possible setting." << std::endl;
    std::vector<std::string> settings;
    _camera.get_possible_setting_options(settings);

    const bool found_setting =
        std::any_of(std::begin(settings), std::end(settings),
                    [this](const std::string& setting) { return (setting == _config.param_name); });

    ASSERT_TRUE(found_setting);
}

void CameraSettings::getAndSetPossibleOptions()
{
    Camera::Option initial_option = getOption(_config.param_name);

    bool is_range = _camera.is_setting_range(_config.param_name);
    ASSERT_EQ(_config.param_is_range, is_range);

    if (is_range) {
        setSomeRangeOptions();
    } else {
        setAllEnumOptions();
    }

    std::cout << "Resetting setting to initial option." << std::endl;
    setOption(_config.param_name, initial_option);
}

void CameraSettings::setAllEnumOptions()
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
}

void CameraSettings::setSomeRangeOptions()
{
    std::cout << "Getting range properties for " << _config.param_name << std::endl;
    std::vector<Camera::Option> options;
    _camera.get_possible_options(_config.param_name, options);

    ASSERT_TRUE(options.size() >= 2);
    ASSERT_TRUE(options.size() <= 3);

    // FIXME: dealing with this interface is not great and needs improvement.

    const auto& minimum = std::atoi(options[0].option_id.c_str());
    {
        Camera::Option new_option;
        new_option.option_id = std::to_string(minimum);
        setOption(_config.param_name, new_option);
        Camera::Option set_option = getOption(_config.param_name);
        EXPECT_EQ(set_option, new_option);
    }

    const auto& maximum = std::atoi(options[1].option_id.c_str());
    {
        Camera::Option new_option;
        new_option.option_id = std::to_string(maximum);
        setOption(_config.param_name, new_option);
        Camera::Option set_option = getOption(_config.param_name);
        EXPECT_EQ(set_option, new_option);
    }

    EXPECT_TRUE(maximum > minimum);

    if (options.size() == 2) {
        // We are only given min and max, no interval so any increment should work.
        const auto& center = (maximum - minimum) / 2 + minimum;
        Camera::Option new_option;
        new_option.option_id = std::to_string(center);
        setOption(_config.param_name, new_option);
        Camera::Option set_option = getOption(_config.param_name);
        EXPECT_EQ(set_option.option_id, std::to_string(center));

    } else {
        const auto& interval = std::atoi(options[2].option_id.c_str());

        EXPECT_EQ((maximum - minimum) % interval, 0);

        // We use the interval to get something close to the center.
        const auto& center = ((maximum - minimum) / 2) / interval * interval;
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
