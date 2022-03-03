#pragma once
#include <memory>
#include <mavsdk/mavsdk.h>
#include "gtest/gtest.h"
#include <chrono>
#include <future>


class Environment : public ::testing::Environment {
private:
    inline static Environment* _instance;

    const std::string _connection_url;

    static std::shared_ptr<mavsdk::System> getSystem(mavsdk::Mavsdk& mavsdk)
    {
        std::cout << "Waiting to discover system...\n";
        auto prom = std::promise<std::shared_ptr<mavsdk::System>>{};
        auto fut = prom.get_future();

        // We wait for new systems to be discovered, once we find one that has an
        // autopilot, we decide to use it.
        mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
            auto system = mavsdk.systems().back();

            if (system->has_autopilot()) {
                std::cout << "Discovered autopilot\n";

                // Unsubscribe again as we only want to find one system.
                mavsdk.subscribe_on_new_system(nullptr);
                prom.set_value(system);
            }
        });

        // We usually receive heartbeats at 1Hz, therefore we should find a
        // system after around 3 seconds max, surely.
        if (fut.wait_for(std::chrono::seconds(3)) == std::future_status::timeout) {
            std::cerr << "No autopilot found.\n";
            return {};
        }

        // Get discovered system now.
        return fut.get();
    }

    Environment(const std::string &connection_url) : _connection_url(connection_url) {
    }

public:


    static bool isCreated() {
        return _instance != nullptr;
    }

    static Environment* getInstance() {
        return _instance;
    }

    static void create(const std::string &connection_url) {
        if (!isCreated()) {
            _instance = new Environment(connection_url);
        }
    }

    void SetUp() override {
        mavsdk::Mavsdk mavsdk;
        mavsdk::ConnectionResult connection_result = mavsdk.add_any_connection(_connection_url);

        if (connection_result != mavsdk::ConnectionResult::Success) {
            std::cerr << "Connection failed: " << connection_result << '\n';
            throw std::runtime_error("Connection failed");
        }

        auto system = getSystem(mavsdk);
        if (!system) {
            throw std::runtime_error("No system found");
        }
    }

    void TearDown() override {
    }

    ~Environment() override {
    }
};

