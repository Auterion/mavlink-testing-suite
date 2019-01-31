#! /bin/bash

BUILD_DIR=build_ci

set -e

[ ! -d "$BUILD_DIR" ] && mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ..
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

