
#include "param.h"
#include <future>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace dronecode_sdk;

namespace tests
{
REGISTER_TEST(ParamChange);

ParamChange::ParamChange(const Context& context) : TestBase(context), _params_raw(context.system) {}

TestBase::Result ParamChange::run()
{
    cout << "Setting param " << _config.name << " to " << _config.set_value << endl;
    ParamsRaw::Result set_result1 = _params_raw.set_param_int(_config.name, _config.set_value);
    if (set_result1 != ParamsRaw::Result::SUCCESS) {
        return Result::Failed;
    }

    cout << "Checking param " << _config.name << " is " << _config.set_value << endl;
    const std::pair<ParamsRaw::Result, int> get_result1 = _params_raw.get_param_int(_config.name);

    if (get_result1.first != ParamsRaw::Result::SUCCESS) {
        return Result::Failed;
    }

    if (get_result1.second != _config.set_value) {
        return Result::Failed;
    }

    cout << "Setting param " << _config.name << " to " << _config.reset_value << endl;
    ParamsRaw::Result set_result2 = _params_raw.set_param_int(_config.name, _config.reset_value);
    if (set_result2 != ParamsRaw::Result::SUCCESS) {
        return Result::Failed;
    }

    cout << "Checking param " << _config.name << " is " << _config.reset_value << endl;
    const std::pair<ParamsRaw::Result, int> get_result2 = _params_raw.get_param_int(_config.name);

    if (get_result2.first != ParamsRaw::Result::SUCCESS) {
        return Result::Failed;
    }

    if (get_result2.second != _config.reset_value) {
        return Result::Failed;
    }

    return Result::Success;
}

}  // namespace tests
