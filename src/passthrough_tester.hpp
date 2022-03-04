#pragma once
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <map>
#include <list>

#include <utility>
#include "passthrough_messages.hpp"

class PassthroughTester {
private:
    std::shared_ptr<mavsdk::MavlinkPassthrough> _passthrough;
    std::map<uint32_t, std::list<std::shared_ptr<std::promise<mavlink_message_t>>>> _callbackMap;

    void passthroughIntercept(mavlink_message_t &message) {
        for (auto &prom : _callbackMap[message.msgid]) {
            prom->set_value(message);
        }
    }


public:
    PassthroughTester(std::shared_ptr<mavsdk::MavlinkPassthrough> passthrough) : _passthrough(std::move(passthrough)) {
        _passthrough->intercept_incoming_messages_async([this](mavlink_message_t &message) {
            passthroughIntercept(message);
            return true;
        });
    }

    template<int MSG, typename... Args>
    void send(Args... args) {
        mavlink_message_t msg;
        msg_helper<MSG>::pack(_passthrough->get_our_sysid(), _passthrough->get_our_compid(), &msg, args...);
        _passthrough->send_message(msg);
    }

    template<int MSG>
    typename msg_helper<MSG>::decode_type receive() {
        auto prom = std::make_shared<std::promise<mavlink_message_t>>();
        auto fut = prom->get_future();

        (_callbackMap[msg_helper<MSG>::ID]).push_back(prom);

        if (fut.wait_for(std::chrono::seconds(3)) == std::future_status::timeout) {
            std::cerr << "Timed out\n";
            throw std::runtime_error("Timeout");
        }
        _callbackMap[msg_helper<MSG>::ID].remove(prom);

        typename msg_helper<MSG>::decode_type decoded_data;
        mavlink_message_t msg = fut.get();
        msg_helper<MSG>::unpack(&msg, &decoded_data);
        return decoded_data;
    }

};
