// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <gtest/gtest.h>
#include <spdlog/spdlog-inl.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <console/ConsoleRegistry.h>

namespace Console
{
namespace
{
class ConsoleRegistryTest : public ::testing::Test
{
  public:
    void SetUp() override;
    void TearDown() override;

  private:
};

void ConsoleRegistryTest::SetUp()
{
    spdlog::stdout_color_mt("Test");
}

void ConsoleRegistryTest::TearDown()
{
    spdlog::drop("Test");
}

TEST_F(ConsoleRegistryTest, RegisterCommand)
{
    // "static" command
    static Command<int> sc{"test0", "desc", [&](ArgStack& stack) {
                        EXPECT_EQ(stack.Pop<int>(), 7);
                    }};

    // "static" setting
    static Setting<bool> ss{"test0", "", true, SettingsFlags::kLocked};

    auto r{std::make_unique<ConsoleRegistry>("Test")};
    r->RegisterCommand<bool, bool>("name", "description", [&](ArgStack& stack) {
        EXPECT_TRUE(stack.Pop<bool>());
        EXPECT_FALSE(stack.Pop<bool>());
    });

    ASSERT_TRUE(r->FindCommand("name"));

    r->TryExecuteCommand("/name true false");
    r->TryExecuteCommand("/test0 7");
    r->TryExecuteCommand("/test0 10 10 10");
    r->TryExecuteCommand("/test0 true");

    r->RegisterSetting<bool>("name", "desc", false);
    ASSERT_TRUE(r->FindSetting("name"));
    ASSERT_TRUE(r->FindSetting("test0"));

    // FAIL
    r->TryExecuteCommand("test0 7");
    r->TryExecuteCommand("/test \xe2\x28\xa1");

    r->Update();
}

TEST_F(ConsoleRegistryTest, RegisterSetting)
{
   // ConsoleRegistry r("Test");
   // r.RegisterSetting<bool>("name", "desc", false);

    //ASSERT_TRUE(r.FindSetting("name"));
}
} // namespace
} // namespace Console
