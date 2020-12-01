## MAVLink Standard Testing Suite

[![GitHub Actions Status](https://github.com/mavlink/MAVSDK/workflows/Build/badge.svg?branch=main)](https://github.com/mavlink/MAVSDK/actions?query=branch%3Amain)

This project aims to provide a testing suite to test standard compliance of MAVLink-enabled components/systems. The definition of MAVLink as standard consists of all the messages and only the messages of this message set: https://mavlink.io/en/messages/common.html

Build the testing suite, on Linux/macOS:
```
  cmake -Bbuild -S.
  cmake --build build -j8.
```
Or on Windows:
```
cmake -G "Visual Studio 16 2019" -Bbuild -S.
cmake --build build -j8
```

### Running the Autopilot Tests

1. Start the SITL simulation (this is specific to the system-under-test, PX4 SITL is simply provided as an example):
    ```
    make px4_sitl jmavsim
    ```

2. Then run the tests in the `build` directory (this is the same for any tested system):
    ```
    build/mavlink_testing_suite config/autopilot.yaml udp://
    ```

### Running the Camera-Manager Tests

1. Start the camera-manager (this is specific to the system-under-test, DCM is simply provided as an example):
    ```
    ./dcm -c samples/config/ubuntu.conf -g debug
    ```
2. Start the camera-manager http server to serve the camera definition file (this is specific to the system-under-test, DCM is simply provided as an example):
    ```
    python -m SimpleHTTPServer
    ```

3. Then run the tests in the `build` directory (this is the same for any tested system):
    ```
    build/mavlink_testing_suite ../config/camera-manager.yaml udp://:14550
    ```

**Note:**
The `camera_id` in [camera-manager.yaml](config/camera-manager.yaml) might need to be adapted based on the camera that camera-manager exposes and you want to test against.

### Fix Code Formatting and Style

Before committing, make sure to run the code formatting and tidying using clang.
```
cmake --build build --target clang_tidy
cmake --build build --target clang_format
```

## Use custom MAVSDK build

For debugging purposes it can be handy to use a custom version of MAVSDK:

```
cmake -Bbuild -DMAVSDK_INSTALL_DIR=path/to/MAVSDK/install -S. && cmake --build build -j8)
```

### Sample Output of the Autopilot Tests
This is the output of a successful test run:
```
[10:03:01|Info ] MAVSDK version: 0.18.3-56-g730a0ef5 (mavsdk_impl.cpp:25)
Waiting to discover system...
[10:03:01|Debug] New: System ID: 0 Comp ID: 0 (mavsdk_impl.cpp:361)
[10:03:01|Info ] New system on: 127.0.0.1:14580 (udp_connection.cpp:231)
[10:03:01|Debug] Component Autopilot (1) added. (system_impl.cpp:399)
Discovered a component with type 1
[10:03:02|Debug] Discovered 1 component(s) (UUID: 5283920058631409231) (system_impl.cpp:563)
[10:03:02|Debug] MAVLink: critical: Data link lost (system_impl.cpp:306)
[10:03:02|Debug] MAVLink: info: Data link regained (system_impl.cpp:306)
Number of waypoints: 50
Uploading mission...
[10:03:04|Debug] Send mission item 0 (mission_impl.cpp:1132)
[10:03:04|Debug] Send mission item 1 (mission_impl.cpp:1132)
[10:03:04|Debug] Send mission item 2 (mission_impl.cpp:1132)
[10:03:04|Debug] Send mission item 3 (mission_impl.cpp:1132)

...

[10:03:04|Debug] Send mission item 47 (mission_impl.cpp:1132)
[10:03:04|Debug] Send mission item 48 (mission_impl.cpp:1132)
[10:03:04|Debug] Send mission item 49 (mission_impl.cpp:1132)
[10:03:04|Info ] Mission accepted (mission_impl.cpp:166)
Downloading mission...
[10:03:04|Debug] Requested mission item 0 (mission_impl.cpp:1007)
[10:03:04|Debug] Received mission item 0 (mission_impl.cpp:282)
[10:03:04|Debug] Requested mission item 1 (mission_impl.cpp:1007)
[10:03:04|Debug] Received mission item 1 (mission_impl.cpp:282)
[10:03:04|Debug] Requested mission item 2 (mission_impl.cpp:1007)
[10:03:04|Debug] Received mission item 2 (mission_impl.cpp:282)
[10:03:04|Debug] Requested mission item 3 (mission_impl.cpp:1007)

...

[10:03:04|Debug] Assembling Message: 47 (mission_impl.cpp:883)
[10:03:04|Debug] Assembling Message: 48 (mission_impl.cpp:883)
[10:03:04|Debug] Assembling Message: 49 (mission_impl.cpp:883)
Mission test result: Success
----
Number of waypoints: 50
Uploading mission...
[10:03:07|Debug] Dropped outgoing message: 44 (system_impl.cpp:496)
[10:03:08|Warn ] Retrying send mission count... (mission_impl.cpp:1352)
[10:03:08|Debug] Dropped incoming message: 51 (system_impl.cpp:136)
[10:03:08|Warn ] Retrying send mission count... (mission_impl.cpp:1352)
[10:03:08|Debug] Send mission item 0 (mission_impl.cpp:1132)
[10:03:08|Debug] Dropped incoming message: 51 (system_impl.cpp:136)
[10:03:08|Warn ] Retrying send mission count... (mission_impl.cpp:1355)
[10:03:08|Debug] Send mission item 0 (mission_impl.cpp:1132)
[10:03:08|Debug] Dropped incoming message: 51 (system_impl.cpp:136)
[10:03:09|Warn ] Retrying send mission count... (mission_impl.cpp:1355)
[10:03:09|Debug] Send mission item 0 (mission_impl.cpp:1132)
[10:03:09|Debug] Dropped outgoing message: 73 (system_impl.cpp:496)
[10:03:09|Debug] Send mission item 1 (mission_impl.cpp:1132)
[10:03:09|Debug] Send mission item 2 (mission_impl.cpp:1132)
[10:03:09|Debug] Send mission item 3 (mission_impl.cpp:1132)

...

[10:03:30|Debug] Assembling Message: 47 (mission_impl.cpp:883)
[10:03:30|Debug] Assembling Message: 48 (mission_impl.cpp:883)
[10:03:30|Debug] Assembling Message: 49 (mission_impl.cpp:883)
Mission test result: Success
----
Resetting int32 param SYS_HITL to 1
Checking set param SYS_HITL is 1
Setting int32 param SYS_HITL to 0
Checking reset param SYS_HITL is 0
ParamChange test result: Success
----
Resetting float param MPC_XY_CRUISE to 10
Checking set param MPC_XY_CRUISE is 10
Setting float param MPC_XY_CRUISE to 5
Checking reset param MPC_XY_CRUISE is 5
ParamChange test result: Success
----
Test summary:
Mission:
name: Mission
num_waypoints: 50
message_loss: 0
  => passed
----
Mission:
name: Mission
num_waypoints: 50
message_loss: 0.25
  => passed
----
ParamChange:
name: ParamChange
param_name: SYS_HITL
param_type: int32
param_set_value: 1
param_reset_value: 0
  => passed
----
ParamChange:
name: ParamChange
param_name: MPC_XY_CRUISE
param_type: float
param_set_value: 10
param_reset_value: 5
  => passed
----
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
[09:41:23|Info ] MAVSDK version: 0.18.3-56-g730a0ef5 (mavsdk_impl.cpp:25)
Waiting to discover system...
[09:41:23|Debug] New: System ID: 0 Comp ID: 0 (mavsdk_impl.cpp:361)
[09:41:24|Info ] New system on: 127.0.0.1:58873 (udp_connection.cpp:231)
[09:41:24|Debug] Component Camera 1 (100) added. (system_impl.cpp:399)
Discovered a component with type 2
[09:41:24|Debug] Component Camera 2 (101) added. (system_impl.cpp:399)
Discovered a component with type 2
[09:41:24|Debug] Component Camera 3 (102) added. (system_impl.cpp:399)
[09:41:24|Debug] Component Camera 4 (103) added. (system_impl.cpp:399)
[09:41:24|Debug] request camera info (camera_impl.cpp:142)
[09:41:24|Debug] downloading camera definition file from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:931)
[09:41:24|Info ] Downloading camera definition from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:1171)
[09:41:24|Debug] request camera info (camera_impl.cpp:142)
[09:41:24|Warn ] sending again, retries to do: 3  (528). (mavlink_commands.cpp:222)
[09:41:25|Warn ] sending again, retries to do: 2  (528). (mavlink_commands.cpp:222)
[09:41:25|Warn ] sending again, retries to do: 1  (528). (mavlink_commands.cpp:222)
[09:41:26|Error] Retrying failed (528) (mavlink_commands.cpp:239)
[09:41:26|Debug] downloading camera definition file from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:931)
[09:41:26|Info ] Downloading camera definition from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:1171)
[09:41:27|Warn ] sending again, retries to do: 3  (528). (mavlink_commands.cpp:222)
Checking if wb-mode is a possible setting.
Getting option for wb-mode
Getting option for wb-mode
Getting possible settings for wb-mode
- Manual Mode (0)
Setting wb-mode to 0
Getting option for wb-mode
- Auto (1)
Setting wb-mode to 1
[09:41:27|Warn ] sending again, retries to do: 2  (528). (mavlink_commands.cpp:222)
Getting option for wb-mode
Resetting setting to initial option.
Setting wb-mode to 1
CameraSettings test result: Success
----
[09:41:27|Debug] request camera info (camera_impl.cpp:142)
[09:41:28|Warn ] sending again, retries to do: 1  (528). (mavlink_commands.cpp:222)
[09:41:28|Debug] request camera info (camera_impl.cpp:142)
[09:41:28|Error] Retrying failed (528) (mavlink_commands.cpp:239)
[09:41:28|Debug] downloading camera definition file from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:931)
[09:41:28|Info ] Downloading camera definition from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:1171)
[09:41:29|Warn ] sending again, retries to do: 3  (528). (mavlink_commands.cpp:222)
[09:41:29|Warn ] sending again, retries to do: 2  (528). (mavlink_commands.cpp:222)
[09:41:30|Warn ] sending again, retries to do: 1  (528). (mavlink_commands.cpp:222)
Checking if exp-mode is a possible setting.
Getting option for exp-mode
Getting option for exp-mode
Getting possible settings for exp-mode
- Manual Mode (1)
Setting exp-mode to 1
[09:41:30|Error] Retrying failed (528) (mavlink_commands.cpp:239)
Getting option for exp-mode
- Aperture Priority Mode (3)
Setting exp-mode to 3
[09:41:30|Debug] downloading camera definition file from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:931)
[09:41:30|Info ] Downloading camera definition from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:1171)
Getting option for exp-mode
Resetting setting to initial option.
Setting exp-mode to 3
CameraSettings test result: Success
----
[09:41:30|Debug] request camera info (camera_impl.cpp:142)
[09:41:31|Warn ] sending again, retries to do: 3  (528). (mavlink_commands.cpp:222)
[09:41:31|Debug] request camera info (camera_impl.cpp:142)
[09:41:31|Warn ] sending again, retries to do: 2  (528). (mavlink_commands.cpp:222)
[09:41:32|Warn ] sending again, retries to do: 1  (528). (mavlink_commands.cpp:222)
[09:41:32|Error] Retrying failed (528) (mavlink_commands.cpp:239)
[09:41:32|Debug] downloading camera definition file from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:931)
[09:41:32|Info ] Downloading camera definition from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:1171)
[09:41:33|Warn ] sending again, retries to do: 3  (528). (mavlink_commands.cpp:222)
Checking if contrast is a possible setting.
Getting option for contrast
Getting range properties for contrast
Setting contrast to 0
Getting option for contrast
Setting contrast to 255
[09:41:33|Warn ] sending again, retries to do: 2  (528). (mavlink_commands.cpp:222)
Getting option for contrast
Setting contrast to 127
Getting option for contrast
Resetting setting to initial option.
Setting contrast to 32
CameraSettings test result: Success
----
[09:41:34|Debug] request camera info (camera_impl.cpp:142)
[09:41:34|Warn ] sending again, retries to do: 1  (528). (mavlink_commands.cpp:222)
[09:41:34|Debug] request camera info (camera_impl.cpp:142)
[09:41:34|Error] Retrying failed (528) (mavlink_commands.cpp:239)
[09:41:34|Debug] downloading camera definition file from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:931)
[09:41:34|Info ] Downloading camera definition from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:1171)
[09:41:35|Warn ] sending again, retries to do: 3  (528). (mavlink_commands.cpp:222)
[09:41:36|Warn ] sending again, retries to do: 2  (528). (mavlink_commands.cpp:222)
[09:41:36|Warn ] sending again, retries to do: 1  (528). (mavlink_commands.cpp:222)
[09:41:37|Error] Retrying failed (528) (mavlink_commands.cpp:239)
[09:41:37|Debug] downloading camera definition file from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:931)
[09:41:37|Info ] Downloading camera definition from: http://127.0.0.1:8000/camera-def-uvc.xml (camera_impl.cpp:1171)
Checking if brightness is a possible setting.
Getting option for brightness
Getting range properties for brightness
Setting brightness to 0
Getting option for brightness
Setting brightness to 225
Getting option for brightness
Setting brightness to 112
Getting option for brightness
Resetting setting to initial option.
Setting brightness to 128
CameraSettings test result: Success
----
Test summary:
CameraSettings:
name: CameraSettings
camera_id: 2
param_name: wb-mode
param_is_range: false
  => passed
----
CameraSettings:
name: CameraSettings
camera_id: 2
param_name: exp-mode
param_is_range: false
  => passed
----
CameraSettings:
name: CameraSettings
camera_id: 2
param_name: contrast
param_is_range: true
  => passed
----
CameraSettings:
name: CameraSettings
camera_id: 2
param_name: brightness
param_is_range: true
  => passed
----
```

**Note:**

In the test above some errors are logged but can be ignored because there is no autopilot connected:
```
[09:41:35|Warn ] sending again, retries to do: 3  (528). (mavlink_commands.cpp:222)
[09:41:36|Warn ] sending again, retries to do: 2  (528). (mavlink_commands.cpp:222)
[09:41:36|Warn ] sending again, retries to do: 1  (528). (mavlink_commands.cpp:222)
[09:41:37|Error] Retrying failed (528) (mavlink_commands.cpp:239)
```
