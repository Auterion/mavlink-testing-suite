#pragma once

#include <dronecode_sdk/dronecode_sdk.h>
#include <yaml-cpp/yaml.h>

#include <map>
#include <memory>
#include <string>

#define UNUSED(x) (void)(x)

namespace tests
{
/**
 * @struct Context
 * Data that is supplied to each test
 */
struct Context {
    dronecode_sdk::System& system;
    // TODO: logging, ...
};

// use YAML as config backend
using ConfigNode = YAML::Node;

/**
 * @class ConfigProvider
 * Stores or retrieves configuration data to/from a ConfigNode (abstracts YAML backend)
 */
class ConfigProvider
{
public:
    ConfigProvider(ConfigNode& config, bool store_config);

    template <typename T>
    void operator()(const std::string& config_name, T& value)
    {
        if (_store_config) {
            _config[config_name] = value;
        } else {
            if (_config[config_name])
                value = _config[config_name].as<T>();
        }
    }

private:
    ConfigNode& _config;
    const bool _store_config;  ///< if true, store config in _config, else load from _config
};

/**
 * @class TestBase
 * Base class for all tests
 */
class TestBase
{
public:
    enum class Result { Success = 0, Failed, Timeout, NotImplemented };

    explicit TestBase(const Context& context);
    virtual ~TestBase() = default;

    void loadConfig(ConfigNode& config);
    void storeConfig(ConfigNode& config);

    /**
     * Run the test and return the result
     */
    virtual Result run() = 0;

protected:
    /**
     * Load/store config
     */
    virtual void serialize(ConfigProvider& c) = 0;

    Context _context;
};

inline const char* toString(TestBase::Result result)
{
    switch (result) {
        case TestBase::Result::Success: return "Success";
        case TestBase::Result::Failed: return "Failed";
        case TestBase::Result::Timeout: return "Timeout";
        case TestBase::Result::NotImplemented: return "Not implemented";
    }
    return "Unknown";
}

/* registering tests helper classes */

/**
 * Base class for TestRegistrar
 * See TestRegistrar below for explanations
 */
class ITestRegistrar
{
public:
    virtual std::unique_ptr<TestBase> getTest(Context& context) = 0;
};

/**
 * This is the factory, the common interface to "tests".
 * Tests registers themselves here and the factory can serve them on demand.
 * It is a Singleton.
 */
class TestFactory
{
public:
    static TestFactory& instance();

    TestFactory(TestFactory const&) = delete;
    void operator=(TestFactory const&) = delete;

    /**
     * Get an instance of a test based on its name.
     * throws out_of_range if test not found */
    std::unique_ptr<TestBase> getTest(const std::string& name, Context& context);

private:
    /** Register a new test */
    void registerTest(ITestRegistrar* registrar, const std::string& name);

    TestFactory() = default;

    std::map<std::string, ITestRegistrar*> _registry; /**< Holds pointers to test registrars */

    template <typename TTest>
    friend class TestRegistrar;
};

/**
 * Helper class that registers a test upon construction.
 */
template <class TTest>
class TestRegistrar : public ITestRegistrar
{
public:
    explicit TestRegistrar(const std::string& classname);
    std::unique_ptr<TestBase> getTest(Context& context) override;

private:
    std::string _classname;
};

template <class TTest>
TestRegistrar<TTest>::TestRegistrar(const std::string& classname) : _classname(classname)
{
    TestFactory& factory = TestFactory::instance();
    factory.registerTest(this, classname);
}

template <class TTest>
std::unique_ptr<TestBase> TestRegistrar<TTest>::getTest(Context& context)
{
    return std::unique_ptr<TTest>(new TTest(context));
}

#define REGISTER_TEST(CLASSNAME)                                       \
    namespace                                                          \
    {                                                                  \
    static TestRegistrar<CLASSNAME> CLASSNAME##_registrar(#CLASSNAME); \
    };

}  // namespace tests
