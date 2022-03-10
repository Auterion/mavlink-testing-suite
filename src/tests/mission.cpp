#include <gtest/gtest.h>
#include "../environment.hpp"
#include "../passthrough_messages.hpp"

using namespace MavlinkTestingSuite;

struct int_coord_t {
    int32_t latitude;
    int32_t longitude;
    float altitude;
};


class Mission : public ::testing::Test {
protected:
    const std::shared_ptr<PassthroughTester> link;
    const YAML::Node config;

    Mission() :
          link(Environment::getInstance()->getPassthroughTester()),
          config(Environment::getInstance()->getConfig()) {
        link->flushAll();
    }


    // Dummy coordinate generator
    int_coord_t missionCoordGen(int seq) {
        float altitude = 10.F + (float)seq;
        double latitude = config["Mission"]["home_lat"].as<double>() + (double)seq * 1e-5;
        double longitude = config["Mission"]["home_lon"].as<double>();
        return {
            static_cast<int32_t>(latitude * 1e7),
            static_cast<int32_t>(longitude * 1e7),
            altitude
        };
    }

    int_coord_t fenceCoordGen(int seq) {
        if (seq > 3 || seq <0) {
            throw std::runtime_error("sequence out of bounds");
        }
        static constexpr int lat_weights[] = {-1, -1, 1, 1};
        static constexpr int lon_weights[] = {-1, 1, 1, -1};

        double latitude = config["Mission"]["home_lat"].as<double>() + lat_weights[seq] * 1e-5;
        double longitude = config["Mission"]["home_lon"].as<double>() + lon_weights[seq] * 1e-5;
        return {
            static_cast<int32_t>(latitude * 1e7),
            static_cast<int32_t>(longitude * 1e7),
            0.0f
        };
    }


    void uploadMission(int N_ITEMS=10) {

        link->send<MISSION_COUNT>(1, 1, N_ITEMS, MAV_MISSION_TYPE_MISSION);
        auto req = link->receive<MISSION_REQUEST_INT>();
        EXPECT_EQ(req.seq, 0);

        for (int i=0; i<N_ITEMS; i++) {
            auto c = missionCoordGen(i);
            link->send<MISSION_ITEM_INT>(1, 1, i, MAV_FRAME_GLOBAL_INT, MAV_CMD_NAV_WAYPOINT, 0, 1,
                                         0.f, 1.f, 0.f, NAN,
                                         c.latitude, c.longitude, c.altitude, MAV_MISSION_TYPE_MISSION);
            if (i<N_ITEMS -1) {
                req = link->receive<MISSION_REQUEST_INT>();
                EXPECT_EQ(req.seq, i+1);
            } else {
                auto ack = link->receive<MISSION_ACK>();
                EXPECT_EQ(ack.type, MAV_MISSION_ACCEPTED) << "Mission not accepted" << std::endl;
            }
        }
    }

    void downloadMission(int N_ITEMS=10) {
        link->send<MISSION_REQUEST_LIST>(1, 1, MAV_MISSION_TYPE_MISSION);
        auto cnt = link->receive<MISSION_COUNT>();

        EXPECT_EQ(cnt.count, N_ITEMS) << "Received wrong mission count" << std::endl;
        EXPECT_EQ(cnt.mission_type, MAV_MISSION_TYPE_MISSION) << "Received count for wrong mission type" << std::endl;
        for (int i=0; i<N_ITEMS; i++) {
            link->send<MISSION_REQUEST_INT>(1, 1, i, MAV_MISSION_TYPE_MISSION);
            auto item = link->receive<MISSION_ITEM_INT>();
            checkMissionItem(item, i);
        }
        link->send<MISSION_ACK>(1, 1, MAV_MISSION_ACCEPTED, MAV_MISSION_TYPE_MISSION);
    }


    void checkMissionItem(const mavlink_mission_item_int_t &item, int seq) {
        EXPECT_EQ(item.command, MAV_CMD_NAV_WAYPOINT);
        EXPECT_EQ(item.x, missionCoordGen(seq).latitude);
        EXPECT_EQ(item.y, missionCoordGen(seq).longitude);
        EXPECT_EQ(item.z, missionCoordGen(seq).altitude);
        EXPECT_EQ(item.mission_type, MAV_MISSION_TYPE_MISSION);
        EXPECT_EQ(item.seq, seq);
    }

    void clearAll() {
        link->send<MISSION_CLEAR_ALL>(1, 1, MAV_MISSION_TYPE_ALL);
        auto ack = link->receive<MISSION_ACK>();
        EXPECT_EQ(ack.type, MAV_MISSION_ACCEPTED);
    }
};


TEST_F(Mission, Upload) {
    clearAll();
    uploadMission();
    clearAll();
}

TEST_F(Mission, UploadAndDownload) {
    const int N_ITEMS = 10;

    uploadMission(N_ITEMS);
    downloadMission(N_ITEMS);
    clearAll();
}

TEST_F(Mission, SetCurrentItem) {
    uploadMission();
    link->send<MISSION_SET_CURRENT>(1, 1, 2);
    // drop all queued MISSION_CURRENT messages to be sure to get the latest only
    link->flush<MISSION_CURRENT>();
    auto curr = link->receive<MISSION_CURRENT>();
    EXPECT_EQ(curr.seq, 2);

    // try out of range
    link->send<MISSION_SET_CURRENT>(1, 1, 101);
    curr = link->receive<MISSION_CURRENT>();
    link->flush<MISSION_CURRENT>();
    EXPECT_EQ(curr.seq, 2);
    clearAll();
}


TEST_F(Mission, UploadGeofence) {
    link->send<MISSION_COUNT>(1, 1, 4, MAV_MISSION_TYPE_FENCE);
    auto req = link->receive<MISSION_REQUEST_INT>();
    EXPECT_EQ(req.seq, 0);

    for (int i=0; i<4; i++) {
        auto c = fenceCoordGen(i);
        link->send<MISSION_ITEM_INT>(1, 1, i, MAV_FRAME_GLOBAL_INT, MAV_CMD_NAV_FENCE_POLYGON_VERTEX_INCLUSION, 0, 0,
                                     4.f, 1.f, NAN, NAN,
                                     c.latitude, c.longitude, c.altitude, MAV_MISSION_TYPE_FENCE);
        if (i<3) {
            req = link->receive<MISSION_REQUEST_INT>();
            EXPECT_EQ(req.seq, i+1);
        } else {
            auto ack = link->receive<MISSION_ACK>();
            EXPECT_EQ(ack.type, MAV_MISSION_ACCEPTED) << "Mission not accepted" << std::endl;
        }
    }

    clearAll();
}

