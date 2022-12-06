// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <gtest/gtest.h>
#include "SemanticVersion.h"

namespace
{
TEST(SemanticVersionTest, DefaultConstructor)
{
    Resources::SemanticVersion version;
    EXPECT_EQ(version.GetVersionBits(), 0);
}

TEST(SemanticVersionTest, Uint32Constructor)
{
    Resources::SemanticVersion version(0x12345678);
    EXPECT_EQ(version.GetVersionBits(), 0x12345678);
}

TEST(SemanticVersionTest, MajorMinorPatchConstructor)
{
    Resources::SemanticVersion version(1, 2, 3);
    EXPECT_EQ(version.GetVersionBits(), 0x01020300);
}

TEST(SemanticVersionTest, StringConstructor)
{
    Resources::SemanticVersion version("1.2.3");
    EXPECT_EQ(version.GetVersionBits(), 0x01020300);
}

TEST(SemanticVersionTest, GetMajor)
{
    Resources::SemanticVersion version(0x12345678);
    EXPECT_EQ(version.GetMajor(), 0x12);
}

TEST(SemanticVersionTest, GetMinor)
{
    Resources::SemanticVersion version(0x12345678);
    EXPECT_EQ(version.GetMinor(), 0x34);
}

TEST(SemanticVersionTest, GetPatch)
{
    Resources::SemanticVersion version(0x12345678);
    EXPECT_EQ(version.GetPatch(), 0x56);
}

TEST(SemanticVersionTest, ToString)
{
    Resources::SemanticVersion version(0x12345678);
    EXPECT_EQ(version.ToString(), "18.52.86");
}

TEST(SemanticVersionTest, FromString)
{
    Resources::SemanticVersion version;
    EXPECT_TRUE(version.FromString("18.52.86"));
    EXPECT_EQ(version.GetVersionBits(), 0x12345678);
}

TEST(SemanticVersionTest, EqualityOperator)
{
    Resources::SemanticVersion version1(0x12345678);
    Resources::SemanticVersion version2(0x12345678);
    EXPECT_TRUE(version1 == version2);
}
}