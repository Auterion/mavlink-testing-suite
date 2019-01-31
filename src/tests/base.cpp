
#include "base.h"

namespace tests
{
ConfigProvider::ConfigProvider(ConfigNode& config, bool store_config)
    : _config(config), _store_config(store_config)
{
}

void TestBase::loadConfig(ConfigNode& config)
{
    ConfigProvider provider(config, false);
    serialize(provider);
}

void TestBase::storeConfig(ConfigNode& config)
{
    ConfigProvider provider(config, true);
    serialize(provider);
}

TestBase::TestBase(const Context& context) : _context(context) {}

std::string TestBase::extractFilename(const std::string& path)
{
    std::stringstream ss(path);
    std::string token;
    while (std::getline(ss, token, '/'))
        ;
    return token;
}

std::ostream& operator<<(std::ostream& str, const TestBase::Result& result)
{
    std::string result_str;
    switch (result) {
        case TestBase::Result::Success: result_str = "Success"; break;
        case TestBase::Result::Failed: result_str = "Failed"; break;
        case TestBase::Result::Timeout: result_str = "Timeout"; break;
        case TestBase::Result::NotImplemented: result_str = "Not implemented"; break;
    }

    return str << result_str;
}

TestFactory& TestFactory::instance()
{
    static TestFactory instance;
    return instance;
}

void TestFactory::registerTest(ITestRegistrar* registrar, const std::string& name)
{
    _registry[name] = registrar;
}

std::unique_ptr<TestBase> TestFactory::getTest(const std::string& name, Context& context)
{
    auto find = _registry.find(name);
    if (find == _registry.end()) {
        throw std::out_of_range("Bad Test Name");
    }
    return find->second->getTest(context);
}

}  // namespace tests
