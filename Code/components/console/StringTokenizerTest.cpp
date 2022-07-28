// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <gtest/gtest.h>
#include <console/StringTokenizer.h>

namespace Console
{
namespace
{
TEST(StringTokenizer, Count)
{
    {
        StringTokenizer t("A Quick brown fox jumps over the fence");
        EXPECT_EQ(t.CountTokens(), 8);
    }

    {
        StringTokenizer t("A_Quick  brown fox_jumps over  the fence");
        EXPECT_EQ(t.CountTokens(), 6);
    }

    {
        StringTokenizer t("A Quick brown\nfox jumps\rover the fence");
        EXPECT_EQ(t.CountTokens(), 8);
    }
}

TEST(StringTokenizer, Tokinize)
{
    {
        StringTokenizer t("A Quick brown fox jumps over the fence");

        std::vector<TiltedPhoques::String> vec;
        while (t.HasMore())
            t.GetNext(vec.emplace_back());

        EXPECT_EQ(vec.size(), 8);
    }

    {
        StringTokenizer t("A_Quick  brown fox_jumps over  the fence");

        std::vector<TiltedPhoques::String> vec;
        while (t.HasMore())
            t.GetNext(vec.emplace_back());

        EXPECT_EQ(vec.size(), 6);
    }

    {
        StringTokenizer t("A Quick brown\nfox jumps\rover the fence");

        std::vector<TiltedPhoques::String> vec;
        while (t.HasMore())
            t.GetNext(vec.emplace_back());

        EXPECT_EQ(vec.size(), 8);
    }
}
} // namespace
}
