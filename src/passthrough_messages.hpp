#pragma once
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include "environment.hpp"

#define MAVLINK_MSG_PACK(MESSAGE_SHORT) mavlink_msg_##MESSAGE_SHORT##_pack
#define MAVLINK_MSG_UNPACK(MESSAGE_SHORT) mavlink_msg_##MESSAGE_SHORT##_decode
#define MAVLINK_MSG_TYPE(MESSAGE_SHORT) mavlink_##MESSAGE_SHORT##_t
#define MAVLINK_MSG_ID(MESSAGE_SHORT_UC) MAVLINK_MSG_ID_##MESSAGE_SHORT_UC

#define USE_MESSAGE(MESSAGE_SHORT, MESSAGE_SHORT_UC) \
    static constexpr int MESSAGE_SHORT_UC = MAVLINK_MSG_ID(MESSAGE_SHORT_UC);                                                 \
    template<>                                                                                \
    struct msg_helper<MESSAGE_SHORT_UC> {                             \
        using decode_type = MAVLINK_MSG_TYPE(MESSAGE_SHORT);                                          \
        static constexpr int ID = MAVLINK_MSG_ID(MESSAGE_SHORT_UC);                                                           \
        static constexpr char NAME[] = #MESSAGE_SHORT_UC;                                             \
        template<typename... Args>                                                                     \
        static void pack(Args... args) {                                                      \
            MAVLINK_MSG_PACK(MESSAGE_SHORT)(args...);                                         \
        }                                                                                     \
        static void unpack(const mavlink_message_t * msg, MAVLINK_MSG_TYPE(MESSAGE_SHORT)* result) {\
            MAVLINK_MSG_UNPACK(MESSAGE_SHORT)(msg, result);                                   \
        }                                                                                     \
    };

template<int MSG>
struct msg_helper {};

/* ----------- LIST ALL MESSAGES TO BE USED IN PASSTHROUGH TESTER BELOW ----------- */

USE_MESSAGE(param_value, PARAM_VALUE)
USE_MESSAGE(param_request_read, PARAM_REQUEST_READ)
USE_MESSAGE(param_set, PARAM_SET)
USE_MESSAGE(param_request_list, PARAM_REQUEST_LIST)

