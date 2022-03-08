#include <gtest/gtest.h>
#include "../environment.hpp"

using namespace MavlinkTestingSuite;

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {

  auto pt = Environment::getInstance()->getPassthrough();

  int compid = pt->get_our_compid();
  printf("Our compid = %d\n", compid);

  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
