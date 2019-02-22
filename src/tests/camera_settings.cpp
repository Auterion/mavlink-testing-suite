
#include "camera_settings.h"
#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;
using namespace dronecode_sdk;

namespace tests
{
REGISTER_TEST(CameraSettings);

CameraSettings::CameraSettings(const Context& context) : TestBase(context), _camera(context.system)
{
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

    EXPECT_EQ(found_setting, true);
}

void CameraSettings::getAndSetPossibleOptions()
{
    std::cout << "Getting possible settings for " << _config.param_name << std::endl;
    std::vector<Camera::Option> options;
    _camera.get_possible_options(_config.param_name, options);

    for (const auto& option : options) {
        std::cout << "- " << option.option_description << " (" << option.option_id << ")"
                  << std::endl;

        // TODO: Set option.
    }
}

void CameraSettings::run()
{
    selectCamera();
    getPossibleSettings();
    getAndSetPossibleOptions();
}

}  // namespace tests
