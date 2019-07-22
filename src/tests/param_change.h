#pragma once

#include "base.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/param/param.h>

namespace tests
{
/**
 * @class ParamChange
 * Test setting and getting parameter.
 */
class ParamChange : public TestBase
{
public:
    struct Config {
        std::string name{};
        std::string type{};
        float set_value{0.0f};
        float reset_value{0.0f};

        void serialize(ConfigProvider& c)
        {
            c("param_name", name);
            c("param_type", type);
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
    void setParam();
    void verifySetParam();
    void resetParam();
    void verifyResetParam();
    bool isInt32();
    bool isFloat();

    mavsdk::Param _param;
    Config _config;
};

}  // namespace tests
