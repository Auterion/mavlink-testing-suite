#pragma once

#include "base.h"

#include <dronecode_sdk/dronecode_sdk.h>
#include <dronecode_sdk/plugins/param_raw/params_raw.h>

namespace tests
{
/**
 * @class ParamChange
 * Test setting parameter.
 */
class ParamChange : public TestBase
{
public:
    struct Config {
        std::string name{};
        int set_value{0};
        int reset_value{0};

        void serialize(ConfigProvider& c)
        {
            c("param_name", name);
            c("param_set_value", set_value);
            c("param_reset_value", reset_value);
        }
    };

    explicit ParamChange(const Context& context);
    ~ParamChange() override = default;

    void run() override;

protected:
    void serialize(ConfigProvider& c) override { _config.serialize(c); }

private:
    dronecode_sdk::ParamsRaw _params_raw;
    Config _config;
};

}  // namespace tests
