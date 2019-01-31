
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

void ParamChange::run()
{
    cout << "Setting param " << _config.name << " to " << _config.set_value << endl;
    ParamsRaw::Result set_result1 = _params_raw.set_param_int(_config.name, _config.set_value);
    EXPECT_EQ(set_result1, ParamsRaw::Result::SUCCESS);

    cout << "Checking param " << _config.name << " is " << _config.set_value << endl;
    const std::pair<ParamsRaw::Result, int> get_result1 = _params_raw.get_param_int(_config.name);

    EXPECT_EQ(get_result1.first, ParamsRaw::Result::SUCCESS);
    EXPECT_EQ(get_result1.second, _config.set_value);

    cout << "Setting param " << _config.name << " to " << _config.reset_value << endl;
    ParamsRaw::Result set_result2 = _params_raw.set_param_int(_config.name, _config.reset_value);
    EXPECT_EQ(set_result2, ParamsRaw::Result::SUCCESS);

    cout << "Checking param " << _config.name << " is " << _config.reset_value << endl;
    const std::pair<ParamsRaw::Result, int> get_result2 = _params_raw.get_param_int(_config.name);

    EXPECT_EQ(get_result2.first, ParamsRaw::Result::SUCCESS);
    EXPECT_EQ(get_result2.second, _config.reset_value);
}

}  // namespace tests
