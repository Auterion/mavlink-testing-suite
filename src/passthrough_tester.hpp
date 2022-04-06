#pragma once
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <map>
#include <list>
#include <mutex>

#include <utility>
#include "passthrough_messages.hpp"

namespace MavlinkTestingSuite {

class TimeoutError : public std::runtime_error {
public:
    TimeoutError(const std::string &msg) : std::runtime_error(msg) {}
};

class PassthroughTester {
private:
    std::shared_ptr<mavsdk::MavlinkPassthrough> _passthrough;
    std::map<uint64_t, std::list<std::shared_ptr<std::promise<mavlink_message_t>>>> _promise_map;
    std::map<uint64_t, std::list<mavlink_message_t>> _message_queue_map;
    std::mutex _map_mutex;

    void passthroughIntercept(mavlink_message_t &message) {
        std::scoped_lock lock(_map_mutex);
        uint64_t hash = recMessageHash(message.msgid, message.sysid, message.compid);
        if ((_promise_map[hash]).empty()) {
            (_message_queue_map[hash]).push_back(message);
        } else {
            for (auto &prom : _promise_map[hash]) {
                prom->set_value(message);
            }
            (_promise_map[hash]).clear();
        }
    }

    static uint64_t recMessageHash(uint32_t message_id, uint8_t sys_id, uint8_t comp_id) {
        return (static_cast<uint64_t>(message_id) << 16u) |
               (static_cast<uint64_t>(sys_id) << 8u) |
               (static_cast<uint64_t>(comp_id));
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
    typename msg_helper<MSG>::decode_type receive(uint8_t src_sysid, uint8_t src_compid, uint32_t timeout_ms) {
        uint64_t hash = recMessageHash(msg_helper<MSG>::ID, src_sysid, src_compid);
        mavlink_message_t msg;
        {
            std::unique_lock lock(_map_mutex);

            if ((_message_queue_map[hash]).empty()) {
                auto prom = std::make_shared<std::promise<mavlink_message_t>>();
                auto fut = prom->get_future();
                (_promise_map[hash]).push_back(prom);
                lock.unlock();
                if (fut.wait_for(std::chrono::milliseconds(timeout_ms)) == std::future_status::timeout) {
                    lock.lock();
                    (_promise_map[hash]).clear();
                    lock.unlock();
                    throw TimeoutError("Message receive timeout for message " + std::string(msg_helper<MSG>::NAME));
                }
                msg = fut.get();
            } else {
                msg = (_message_queue_map[hash]).front();
                (_message_queue_map[hash]).pop_front();
            }
        }

        typename msg_helper<MSG>::decode_type decoded_data;        
        msg_helper<MSG>::unpack(&msg, &decoded_data);
        return decoded_data;
    }

    template<int MSG>
    typename msg_helper<MSG>::decode_type receive(uint8_t src_sysid, uint8_t src_compid) {
        return receive<MSG>(src_sysid, src_compid, 100);
    }

    template<int MSG>
    void flush(uint8_t src_sysid, uint8_t src_compid) {
        uint64_t hash = recMessageHash(msg_helper<MSG>::ID, src_sysid, src_compid);
        std::scoped_lock lock{_map_mutex};
        (_promise_map[hash]).clear();
        (_message_queue_map[hash]).clear();
    }

    void flushAll() {
        std::scoped_lock lock{_map_mutex};
        _promise_map.clear();
        _message_queue_map.clear();
    }

    ~PassthroughTester() {
        _passthrough->intercept_incoming_messages_async(nullptr);
    }

};
};
