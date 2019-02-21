
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

#include <dronecode_sdk/dronecode_sdk.h>

#include "tests/base.h"

using namespace dronecode_sdk;
using namespace std::this_thread;
using namespace std::chrono;

#define ERROR_CONSOLE_TEXT "\033[31m"      // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m"  // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m"      // Restore normal console colour

void componentDiscovered(ComponentType component_type)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Discovered a component with type "
              << unsigned(component_type) << std::endl;
}

int main(int argc, char** argv)
{
    ConnectionResult connection_result;

    DronecodeSDK dc;

    bool discovered_system = false;
    if (argc != 3) {
        std::cout << "Must specify a config file and a connection" << std::endl;
        return 1;
    }

    const std::string config_file_path = argv[1];
    const std::string connection_url = argv[2];

    // connection_url format:
    // For TCP : tcp://[server_host][:server_port]
    // For UDP : udp://[bind_host][:bind_port]
    // For Serial : serial:///path/to/serial/dev[:baudrate]
    connection_result = dc.add_any_connection(connection_url);

    if (connection_result != ConnectionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT
                  << "Connection failed: " << connection_result_str(connection_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    std::cout << "Waiting to discover system..." << std::endl;
    dc.register_on_discover([&discovered_system](uint64_t uuid) {
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        discovered_system = true;
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a system after around 2
    // seconds.
    sleep_for(seconds(2));

    if (!discovered_system) {
        std::cout << ERROR_CONSOLE_TEXT << "No system found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    // TODO: connect & identify system based on sysid
    System& system = dc.system();
    system.register_component_discovered_callback(componentDiscovered);

    // load config
    YAML::Node config = YAML::LoadFile(config_file_path);
    YAML::Node tests_node = config["tests"];

    std::vector<bool> fails(tests_node.size(), false);

    // Run the test(s)
    tests::Context context{.system = system};
    unsigned i = 0;
    for (auto test_node : tests_node) {
        std::string test_name = test_node["name"].as<std::string>();
        fails[i] = false;
        std::unique_ptr<tests::TestBase> test =
            tests::TestFactory::instance().getTest(test_name, context);

        test->loadConfig(test_node);

        try {
            test->run();
        } catch (tests::TestBase::TestAborted& e) {
            std::cout << test_name << " aborted early (" << e.what() << ")" << std::endl;
            fails[i] = true;
        }

        if (!fails[i]) {
            tests::TestBase::Result result = test->getResult();
            if (result != tests::TestBase::Result::Success) {
                fails[i] = true;
            }
            std::cout << test_name << " test result: " << result << std::endl;
        }

        // store the actually used config (which includes the default values) back
        // in the YAML config node
        test->storeConfig(test_node);
        std::cout << "----" << std::endl;
        ++i;
    }

    std::cout << "Test summary:" << std::endl;

    i = 0;
    for (auto test_node : tests_node) {
        std::cout << test_node["name"].as<std::string>() << ":" << std::endl;
        for (auto config : test_node) {
            std::cout << config.first.as<std::string>() << ": " << config.second.as<std::string>()
                      << std::endl;
        }

        std::cout << "  => " << (fails[i] ? std::string("failed") : std::string("passed"))
                  << std::endl;
        std::cout << "----" << std::endl;
        ++i;
    }

    return (std::any_of(fails.cbegin(), fails.cend(),
                        [](std::vector<bool>::const_reference item) { return item; })
                ? -1
                : 0);
}
