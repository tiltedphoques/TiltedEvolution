// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "SemanticVersion2.h"
#include <gtest/gtest.h>

namespace
{
using namespace Resources;

TEST(SemanticVersionTest, DefaultConstructor)
{
    SemanticVersion version;
    EXPECT_EQ(version.GetVersionBits(), 0);
}

TEST(SemanticVersionTest, Uint32Constructor)
{
    SemanticVersion version(0x12345678);
    EXPECT_EQ(version.GetVersionBits(), 0x12345678);
}

TEST(SemanticVersionTest, MajorMinorPatchConstructor)
{
    SemanticVersion version(1, 2, 3);
    EXPECT_EQ(version.GetVersionBits(), 4398050705411);
}

TEST(SemanticVersionTest, StringConstructor)
{
    SemanticVersion version("1.2.3");
    EXPECT_EQ(version.GetVersionBits(), 4398050705411);
}

TEST(SemanticVersionTest, GetMajor)
{
    SemanticVersion version("18.52.86");
    EXPECT_EQ(version.GetMajor(), 18);
}

TEST(SemanticVersionTest, GetMinor)
{
    SemanticVersion version("18.52.86");
    EXPECT_EQ(version.GetMinor(), 52);
}

TEST(SemanticVersionTest, GetPatch)
{
    SemanticVersion version("18.52.86");
    EXPECT_EQ(version.GetPatch(), 86);
}

TEST(SemanticVersionTest, ToString)
{
    SemanticVersion version(79164946251862);
    EXPECT_EQ(version.ToString(), "18.52.86");
}

TEST(SemanticVersionTest, FromString)
{
    SemanticVersion version;
    EXPECT_TRUE(version.FromString("18.52.86"));
    EXPECT_EQ(version.GetVersionBits(), 79164946251862);
}

TEST(SemanticVersionTest, EqualityOperator)
{
    SemanticVersion version1(0x12345678);
    SemanticVersion version2(0x12345678);
    EXPECT_TRUE(version1 == version2);
}

// Test that the > operator returns true when comparing two SemanticVersion objects where the left-hand side has a
// higher version number than the right-hand side.
TEST(SemanticVersionTest, GreaterThan)
{
    SemanticVersion lhs("1.2.3");
    SemanticVersion rhs("1.2.2");
    EXPECT_TRUE(lhs > rhs);

    lhs = SemanticVersion("1.2.2");
    rhs = SemanticVersion("1.1.3");
    EXPECT_TRUE(lhs > rhs);

    lhs = SemanticVersion("1.1.3");
    rhs = SemanticVersion("0.2.3");
    EXPECT_TRUE(lhs > rhs);
}

// Test that the > operator returns false when comparing two SemanticVersion objects where the left-hand side has a
// lower or equal version number than the right-hand side.
TEST(SemanticVersionTest, NotGreaterThan)
{
    SemanticVersion lhs("1.2.2");
    SemanticVersion rhs("1.2.3");
    EXPECT_FALSE(lhs > rhs);

    lhs = SemanticVersion("1.1.3");
    rhs = SemanticVersion("1.2.2");
    EXPECT_FALSE(lhs > rhs);

    lhs = SemanticVersion("0.2.3");
    rhs = SemanticVersion("1.1.3");
    EXPECT_FALSE(lhs > rhs);

    lhs = SemanticVersion("1.2.3");
    rhs = SemanticVersion("1.2.3");
    EXPECT_FALSE(lhs > rhs);
}

// Test that the < operator returns true when comparing two SemanticVersion objects where the left-hand side has a lower
// version number than the right-hand side.
TEST(SemanticVersionTest, LessThan)
{
    SemanticVersion lhs("1.2.2");
    SemanticVersion rhs("1.2.3");
    EXPECT_TRUE(lhs < rhs);

    lhs = SemanticVersion("1.1.3");
    rhs = SemanticVersion("1.2.2");
    EXPECT_TRUE(lhs < rhs);

    lhs = SemanticVersion("0.2.3");
    rhs = SemanticVersion("1.1.3");
    EXPECT_TRUE(lhs < rhs);
}
} // namespace