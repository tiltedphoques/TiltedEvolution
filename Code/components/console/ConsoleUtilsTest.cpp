// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <gtest/gtest.h>
#include <console/ConsoleUtils.h>

namespace Console
{
namespace
{
TEST(ConsoleUtils, TestUTF8)
{
    EXPECT_TRUE(CheckIsValidUTF8("A Quick brown fox jumps over the fence"));
    EXPECT_FALSE(CheckIsValidUTF8("\xe2\x28\xa1"));
}

TEST(ConsoleUtils, TestNumeric)
{
    EXPECT_TRUE(IsNumber("10"));
    EXPECT_TRUE(IsNumber("10.00"));

    // German way is not supported yet.
    //EXPECT_TRUE(IsNumber("10,00"));

    EXPECT_FALSE(IsNumber("ten"));
    EXPECT_FALSE(IsNumber("ten100"));
    EXPECT_FALSE(IsNumber("ten10,0"));
    EXPECT_FALSE(IsNumber("ten10.0"));
    EXPECT_FALSE(IsNumber("ten 10.0 10"));
}
} // namespace
} // namespace Console
