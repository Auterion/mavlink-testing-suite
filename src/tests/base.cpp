
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
