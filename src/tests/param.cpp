
#include "param.h"
#include <future>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace dronecode_sdk;

namespace tests
{
REGISTER_TEST(ParamChange);

ParamChange::ParamChange(const Context& context) : TestBase(context), _params_raw(context.system) {}

TestBase::Result ParamChange::run()
{
	// TODO: Change param and then reset it.

	return Result::NotImplemented;
}

}  // namespace tests
