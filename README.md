## MAVLink Standard Testing Suite

[![Build Status](https://travis-ci.org/Auterion/mavlink-testing-suite.svg?branch=master)](https://travis-ci.org/Auterion/mavlink-testing-suite)

This project aims to provide a testing suite to test standard compliance of
MAVLink-enabled components/systems.
The design intent is described in https://docs.google.com/document/d/1zwUZ-VUmq2pmCuGn1kY6BRS48-GiSXcMO5TUfnTpqmI

- Build the testing suite:
  ```
  (mkdir -p build && cd build && cmake .. && make)
  ```
### Running the Autopilot Tests

- Start the SITL simulation
- Then run the tests in the `build` directory with:
  ```
  (cd build && ./mavlink_testing_suite ../config/autopilot.yaml udp://)
  ```

### Running the Camera-Manager Tests

- Start a camera-manager
- Then run the tests in the `build` directory with:
  ```
  (cd build && ./mavlink_testing_suite ../config/camera-manager.yaml udp://)
  ```

### Fix Code Formatting and Style

Before committing, make sure to run the code formatting and tidying using clang.
```
(cd build && make format; make clang_tidy)
```

## Use custom MAVSDK build

For debugging purposes it can be handy to use a custom version of MAVSDK:

```
cd <wherever>/MAVSDK
make clean
export DRONECODE_SDK_INSTALL_DIR=$(pwd)/install
make ENABLE_MAVLINK_PASSTHROUGH=1 INSTALL_PREFIX=$MAVSDK_INSTALL_DIR default install
cd <wherever>/mavlink-testing-suite
(mkdir -p build && cd build && cmake -DDRONECODE_SDK_INSTALL_DIR=$MAVSDK_INSTALL_DIR .. && make)
```

### Sample Output of the Autopilot Tests
This is the output of a successful test run:
```
[10:56:54|Info ] DronecodeSDK version: 0.9.0 (dronecode_sdk_impl.cpp:24)
[10:56:54|Debug] New: System ID: 0 Comp ID: 0 (dronecode_sdk_impl.cpp:288)
Waiting to discover system...
[10:56:54|Info ] New device on: 127.0.0.1:14580 (udp_connection.cpp:200)
[10:56:54|Debug] Component Autopilot (1) added. (system_impl.cpp:369)
[10:56:54|Debug] Discovered 1 component(s) (UUID: 5283920058631409231) (system_impl.cpp:509)
Discovered system with UUID: 5283920058631409231
[10:56:55|Debug] MAVLink: info: data link #1 lost (system_impl.cpp:280)
[10:56:55|Debug] MAVLink: info: data link #1 regained (system_impl.cpp:280)
Discovered a component with type 1
number of waypoints: 50
Uploading mission...
[10:56:56|Debug] Send mission item 0 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 1 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 2 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 3 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 4 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 5 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 6 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 7 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 8 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 9 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 10 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 11 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 12 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 13 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 14 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 15 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 16 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 17 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 18 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 19 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 20 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 21 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 22 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 23 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 24 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 25 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 26 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 27 (mission_impl.cpp:996)
[10:56:56|Debug] Send mission item 28 (mission_impl.cpp:996)
[10:56:56|Info ] Mission accepted (mission_impl.cpp:163)
Mission uploaded.
MissionUpload test result: Success
Setting param SYS_HITL to 1
Checking param SYS_HITL is 1
Setting param SYS_HITL to 0
Checking param SYS_HITL is 0
ParamChange test result: Success
```

And an unsuccessful run, where handling of `MISSION_COUNT` on the autopilot is
disabled:
```
[10:58:27|Info ] DronecodeSDK version: 0.9.0 (dronecode_sdk_impl.cpp:24)
[10:58:27|Debug] New: System ID: 0 Comp ID: 0 (dronecode_sdk_impl.cpp:288)
Waiting to discover system...
[10:58:27|Info ] New device on: 127.0.0.1:14580 (udp_connection.cpp:200)
[10:58:27|Debug] Component Autopilot (1) added. (system_impl.cpp:369)
[10:58:28|Debug] Discovered 1 component(s) (UUID: 5283920058631409231) (system_impl.cpp:509)
Discovered system with UUID: 5283920058631409231
Discovered a component with type 1
number of waypoints: 50
Uploading mission...
[10:58:31|Warn ] Mission handling timed out while uploading mission (expected MISSION_REQUEST_INT). (mission_impl.cpp:1170)
Mission upload failed (Timeout), exiting.
MissionUpload test result: Failed
```

### Sample Output of the Camera-Manager Tests
This is the output of a successful test run:

```
[12:03:36|Info ] DronecodeSDK version: 0.14.2-34-g215e3bd6 (dronecode_sdk_impl.cpp:25)
Waiting to discover system...
[12:03:36|Debug] New: System ID: 0 Comp ID: 0 (dronecode_sdk_impl.cpp:285)
[12:03:36|Info ] New system on: 127.0.0.1:50124 (udp_connection.cpp:225)
[12:03:36|Debug] Component Camera 1 (100) added. (system_impl.cpp:385)
Discovered a component with type 2
[12:03:36|Debug] Component Camera 2 (101) added. (system_impl.cpp:385)
Discovered a component with type 2
[12:03:36|Debug] Component Camera 3 (102) added. (system_impl.cpp:385)
[12:03:36|Debug] Component Camera 4 (103) added. (system_impl.cpp:385)
[12:03:36|Debug] request camera info (camera_impl.cpp:123)
[12:03:36|Debug] downloading camera definition file from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:924)
[12:03:36|Info ] Downloading camera definition from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:1145)
[12:03:37|Debug] request camera info (camera_impl.cpp:123)
[12:03:37|Warn ] sending again, retries to do: 3  (528). (mavlink_commands.cpp:219)
[12:03:38|Warn ] sending again, retries to do: 2  (528). (mavlink_commands.cpp:219)
[12:03:38|Warn ] sending again, retries to do: 1  (528). (mavlink_commands.cpp:219)
Checking if wb-mode is a possible setting.
Getting option for wb-mode
Getting possible settings for wb-mode
- Manual Mode (0)
Setting wb-mode to 0
Getting option for wb-mode
[12:03:39|Error] Retrying failed (528) (mavlink_commands.cpp:235)
[12:03:39|Debug] downloading camera definition file from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:924)
[12:03:39|Info ] Downloading camera definition from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:1145)
[12:03:39|Warn ] sending again, retries to do: 3  (528). (mavlink_commands.cpp:219)
- Auto (1)
Setting wb-mode to 1
Getting option for wb-mode
[12:03:40|Warn ] sending again, retries to do: 2  (528). (mavlink_commands.cpp:219)
[12:03:40|Warn ] sending again, retries to do: 1  (528). (mavlink_commands.cpp:219)
Resetting setting to initial option.
Setting wb-mode to 1
[12:03:41|Error] Retrying failed (528) (mavlink_commands.cpp:235)
Getting option for wb-mode
Getting possible settings for wb-mode
Setting wb-mode to 0
Resetting setting to initial option.
Setting wb-mode to 1
CameraSettings test result: Success
----
Test summary:
CameraSettings:
name: CameraSettings
camera_id: 2
param_name: wb-mode
param_value: 0
sub_param_name: wb-temp
sub_param_values: 4000
sub_param_value:
  => passed
----
```
