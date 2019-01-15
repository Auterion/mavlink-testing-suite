## MAVLink Standard Testing Suite

This project aims to provide a testing suite to test standard compliance of
MAVLink-enabled components/systems.
The design intent is described in https://docs.google.com/document/d/1zwUZ-VUmq2pmCuGn1kY6BRS48-GiSXcMO5TUfnTpqmI

Note: the implementation here is very preliminary.

### Build Instructions
- install the Dronecode SDK (from source):
  ```
  git clone --recursive https://github.com/Dronecode/DronecodeSDK.git
  cd DronecodeSDK
  mkdir install
  export INSTALL_PREFIX=$(pwd)/install
  make default install
  ```
- build the testing suite:
  ```
  mkdir build && cd build
  cmake .. -DSDK_INSTALL_DIR=${INSTALL_PREFIX}
  make
  ```
### Running the Test(s)
- Start the SITL simulation
- Then run the tests in the `build` directory with:
  ```
  ./mavlink_testing_suite udp://
  ```

### Sample Output
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
