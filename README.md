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

