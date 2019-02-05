
#include "param.h"
#include <cmath>
#include <cstdint>
#include <future>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace dronecode_sdk;

namespace tests
{
REGISTER_TEST(ParamChange);

ParamChange::ParamChange(const Context& context) : TestBase(context), _params_raw(context.system) {}

void ParamChange::setParam()
{
    if (isInt32()) {
        int32_t set_value = long(std::round(_config.set_value));
        cout << "Resetting int32 param " << _config.name << " to " << set_value << endl;
        ParamsRaw::Result set_result = _params_raw.set_param_int(_config.name, set_value);
        EXPECT_EQ(set_result, ParamsRaw::Result::SUCCESS);

    } else if (isFloat()) {
        cout << "Resetting float param " << _config.name << " to " << _config.set_value << endl;
        ParamsRaw::Result set_result = _params_raw.set_param_float(_config.name, _config.set_value);
        EXPECT_EQ(set_result, ParamsRaw::Result::SUCCESS);

    } else {
        throw std::invalid_argument("Unknown param type");
    }
}

void ParamChange::resetParam()
{
    if (isInt32()) {
        int32_t reset_value = long(std::round(_config.reset_value));
        cout << "Setting int32 param " << _config.name << " to " << reset_value << endl;
        ParamsRaw::Result reset_result = _params_raw.set_param_int(_config.name, reset_value);
        EXPECT_EQ(reset_result, ParamsRaw::Result::SUCCESS);

    } else if (isFloat()) {
        cout << "Setting float param " << _config.name << " to " << _config.reset_value << endl;
        ParamsRaw::Result set_result =
            _params_raw.set_param_float(_config.name, _config.reset_value);
        EXPECT_EQ(set_result, ParamsRaw::Result::SUCCESS);

    } else {
        throw std::invalid_argument("Unknown param type");
    }
}

void ParamChange::verifySetParam()
{
    if (isInt32()) {
        int32_t set_value = long(std::round(_config.set_value));
        cout << "Checking set param " << _config.name << " is " << set_value << endl;
        const std::pair<ParamsRaw::Result, int> get_result =
            _params_raw.get_param_int(_config.name);

        EXPECT_EQ(get_result.first, ParamsRaw::Result::SUCCESS);
        EXPECT_EQ(get_result.second, set_value);

    } else if (isFloat()) {
        cout << "Checking set param " << _config.name << " is " << _config.set_value << endl;
        const std::pair<ParamsRaw::Result, float> get_result =
            _params_raw.get_param_float(_config.name);

        EXPECT_EQ(get_result.first, ParamsRaw::Result::SUCCESS);
        EXPECT_EQ(get_result.second, _config.set_value);

    } else {
        throw std::invalid_argument("Unknown param type");
    }
}

void ParamChange::verifyResetParam()
{
    if (isInt32()) {
        int32_t reset_value = long(std::round(_config.reset_value));
        cout << "Checking reset param " << _config.name << " is " << reset_value << endl;
        const std::pair<ParamsRaw::Result, int> get_result =
            _params_raw.get_param_int(_config.name);

        EXPECT_EQ(get_result.first, ParamsRaw::Result::SUCCESS);
        EXPECT_EQ(get_result.second, reset_value);

    } else if (isFloat()) {
        cout << "Checking reset param " << _config.name << " is " << _config.reset_value << endl;
        const std::pair<ParamsRaw::Result, float> get_result =
            _params_raw.get_param_float(_config.name);

        EXPECT_EQ(get_result.first, ParamsRaw::Result::SUCCESS);
        EXPECT_EQ(get_result.second, _config.reset_value);

    } else {
        throw std::invalid_argument("Unknown param type");
    }
}

void ParamChange::run()
{
    setParam();
    verifySetParam();
    resetParam();
    verifyResetParam();
}

bool ParamChange::isInt32()
{
    return _config.type == "int32";
}

bool ParamChange::isFloat()
{
    return _config.type == "float";
}

}  // namespace tests
