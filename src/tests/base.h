#pragma once

#include <mavsdk/mavsdk.h>
#include <yaml-cpp/yaml.h>
#include "streamable.h"

#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

template <typename T>
inline void unused(T t)
{
    (void)(t);
}

namespace tests
{
/**
 * @struct Context
 * Data that is supplied to each test
 */
struct Context {
    mavsdk::System& system;
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

    class TestAborted : public std::exception
    {
    public:
        explicit TestAborted(std::string reason) : _reason(std::move(reason)) {}
        const char* what() const noexcept override { return _reason.c_str(); }

    protected:
        const std::string _reason;
    };

    explicit TestBase(const Context& context);
    virtual ~TestBase() = default;

    void loadConfig(ConfigNode& config);
    void storeConfig(ConfigNode& config);

    /**
     * Run the test.
     */
    virtual void run() = 0;

    /**
     * Get the result of the test just run.
     */
    Result getResult() { return _result; };

    template <typename T>
    void expectEq(const T& a, const T& b, const std::string& a_str, const std::string& b_str,
                  const std::string& file, int line)
    {
        if (a == b) {
            return;
        }

        std::cout << "Expect at " << extractFilename(file) << ":" << line << " failed" << std::endl;
        std::cout << a_str << " != " << b_str << std::endl;

        auto maybe_streamable = MaybeStreamable<T>();

        if (maybe_streamable.isStreamable()) {
            std::cout << maybe_streamable.print(a) << " != " << maybe_streamable.print(b)
                      << std::endl;
        }

        _result = Result::Failed;
    }

    template <typename T>
    void assertEq(const T& a, const T& b, const std::string& a_str, const std::string& b_str,
                  const std::string& file, int line)
    {
        if (a == b) {
            return;
        }

        std::cout << "Assert at " << extractFilename(file) << ":" << line << " failed" << std::endl;
        std::cout << a_str << " != " << b_str << std::endl;

        auto maybe_streamable = MaybeStreamable<T>();

        if (maybe_streamable.isStreamable()) {
            std::cout << maybe_streamable.print(a) << " != " << maybe_streamable.print(b)
                      << std::endl;
        }

        throw TestAborted("assertEq failed");
    }

#define EXPECT_EQ(a_, b_) expectEq((a_), (b_), #a_, #b_, __FILE__, __LINE__)
#define ASSERT_EQ(a_, b_) assertEq((a_), (b_), #a_, #b_, __FILE__, __LINE__)
#define EXPECT_TRUE(a_) EXPECT_EQ((a_), true)
#define ASSERT_TRUE(a_) ASSERT_EQ((a_), true)
#define EXPECT_FALSE(a_) EXPECT_EQ((a_), false)
#define ASSERT_FALSE(a_) ASSERT_EQ((a_), false)

protected:
    /**
     * Load/store config
     */
    virtual void serialize(ConfigProvider& c) = 0;

    Context _context;
    Result _result{Result::Success};

private:
    std::string extractFilename(const std::string& path);
    static constexpr char separator()
    {
#ifdef _WIN32
        return '\\';
#else
        return '/';
#endif
    }
};

std::ostream& operator<<(std::ostream& str, const TestBase::Result& result);

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
