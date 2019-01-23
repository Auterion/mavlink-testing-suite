
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

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
    DronecodeSDK dc;
    std::string connection_url;
    ConnectionResult connection_result;

    bool discovered_system = false;
    if (argc == 2) {
        connection_url = argv[1];
        // connection_url format:
        // For TCP : tcp://[server_host][:server_port]
        // For UDP : udp://[bind_host][:bind_port]
        // For Serial : serial:///path/to/serial/dev[:baudrate]
        connection_result = dc.add_any_connection(connection_url);
    } else {
        std::cout << "Must specify a connection" << std::endl;
        return 1;
    }

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
    YAML::Node config = YAML::LoadFile("../config/autopilot.yaml");
    YAML::Node tests_node = config["tests"];

    // Run the test(s)
    tests::Context context{.system = system};
    bool failed = false;
    for (auto test_node : tests_node) {
        std::string test_name = test_node["name"].as<std::string>();
        std::unique_ptr<tests::TestBase> test =
            tests::TestFactory::instance().getTest(test_name, context);

        test->loadConfig(test_node);

        tests::TestBase::Result result = test->run();

        std::cout << test_name << " test result: " << toString(result) << std::endl;

        // store the actually used config (which includes the default values) back
        // in the YAML config node
        test->storeConfig(test_node);

        if (result != tests::TestBase::Result::Success)
            failed = true;
    }

    return failed ? -1 : 0;
}
