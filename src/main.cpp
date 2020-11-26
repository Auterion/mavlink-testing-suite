
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

#include <mavsdk/mavsdk.h>

#include "tests/base.h"

using namespace mavsdk;

auto constexpr error_console_text = "\033[31m";  // Turn text on console red
auto constexpr normal_console_text = "\033[0m";  // Restore normal console colour

int main(int argc, char** argv)
{
    ConnectionResult connection_result;

    Mavsdk mavsdk;

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
    connection_result = mavsdk.add_any_connection(connection_url);

    if (connection_result != ConnectionResult::Success) {
        std::cout << error_console_text
                  << "Connection failed: " << connection_result
                  << normal_console_text << std::endl;
        return 1;
    }

    std::cout << "Waiting to discover system..." << std::endl;
    auto prom = std::make_shared<std::promise<void>>();
    std::future<void> fut = prom->get_future();

    mavsdk.subscribe_on_new_system([prom, &mavsdk]() {
        std::cout << normal_console_text << "Discovered a system\n";
        try {
            // We only need to receive this once, now we can unsubscribe again.
            mavsdk.subscribe_on_new_system(nullptr);
            prom->set_value();
        } catch (const std::future_error& e) {
            // Ignore if prom is set multiple times which can happen even though we're
            // unsubscribing straightaway if the callback triggers two times in quick
            // succession.
        }
    });

    auto system = mavsdk.systems()[0];

    if (fut.wait_for(std::chrono::seconds(2)) == std::future_status::timeout) {
        std::cout << error_console_text << "No MAVLink component found" << normal_console_text
                  << std::endl;
        return 1;
    }

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
