#! /bin/bash

BUILD_DIR=build_ci
SDK_INSTALL_DIR="$(pwd)/DronecodeSDK-install"

set -e

# Get the SDK - for now we use the latest develop version
if [ -d DronecodeSDK ]; then
	# If it's already here, make sure it's up-to-date and clean
	pushd DronecodeSDK
	git pull
	make clean
	popd
else
	git clone --recursive https://github.com/Dronecode/DronecodeSDK.git || true
fi
pushd DronecodeSDK
[ ! -d "$SDK_INSTALL_DIR" ] && mkdir -p "$SDK_INSTALL_DIR"
export INSTALL_PREFIX="$SDK_INSTALL_DIR"
make default install
popd

[ ! -d "$BUILD_DIR" ] && mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -DSDK_INSTALL_DIR="${SDK_INSTALL_DIR}"
make

set +e

make clang_tidy
clang_tidy_ret=$?
git diff --exit-code
if [ $? -ne 0 -o $clang_tidy_ret -ne 0 ]; then
	echo "Clang tidy failed. Run 'make clang_tidy'"
	exit -1
fi

make format
git diff --exit-code
if [ $? -ne 0 ]; then
	echo "Formatting issue. Run 'make format' and amend the result"
	exit -1
fi

exit 0

