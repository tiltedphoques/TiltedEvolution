// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <gtest/gtest.h>
#include <spdlog/spdlog-inl.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <console/ConsoleRegistry.h>
#include <console/EnvSettingsProvider.h>

#include <cstdlib>

namespace Console
{
namespace
{
void SetEnv(const char* apName, const char* apValue)
{
#ifdef _WIN32
    _putenv_s(apName, apValue);
#else
    setenv(apName, apValue, 1);
#endif
}

void ClearEnv(const char* apName)
{
#ifdef _WIN32
    _putenv_s(apName, "");
#else
    unsetenv(apName);
#endif
}

class EnvSettingsProviderTest : public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;
};

void EnvSettingsProviderTest::SetUp()
{
    spdlog::stdout_color_mt("Test");
}

void EnvSettingsProviderTest::TearDown()
{
    ClearEnv("UDIFFICULTY");
    ClearEnv("BENABLEPVP");
    ClearEnv("SPASSWORD");
    ClearEnv("FINVALIDNUMBER");
    spdlog::drop("Test");
}

TEST(EnvSettingsProvider, VariableNameStripsSectionAndUppercases)
{
    // The names from the examples in the feature request.
    EXPECT_EQ(ToEnvironmentVariableName("Gameplay:uDifficulty"), "UDIFFICULTY");
    EXPECT_EQ(ToEnvironmentVariableName("ModPolicy:bEnableModCheck"), "BENABLEMODCHECK");
    EXPECT_EQ(ToEnvironmentVariableName("GameServer:sPassword"), "SPASSWORD");
    // Settings without a section keep their full name.
    EXPECT_EQ(ToEnvironmentVariableName("bConsole"), "BCONSOLE");
}

// The settings are function-local statics, the pattern ConsoleRegistryTest
// uses, and the registry is leaked on purpose. Every command/setting
// construction links into the static lists that a new ConsoleRegistry
// drains in BindStaticItems, including the registry's own native commands:
// if this registry were destroyed, the next registry in the process would
// drain dangling entries. Production runs a single registry for the life
// of the process, so this only matters in tests.
TEST_F(EnvSettingsProviderTest, EnvironmentOverridesRegisteredSettings)
{
    static Setting<uint32_t> s_uDifficulty{"Gameplay:uDifficulty", "desc", 4u};
    static Setting<bool> s_bEnablePvp{"Gameplay:bEnablePvp", "desc", false};
    static StringSetting s_sPassword{"GameServer:sPassword", "desc", ""};
    static Setting<float> s_fInvalidNumber{"Test:fInvalidNumber", "desc", 1.5f};

    static ConsoleRegistry* s_pRegistry = new ConsoleRegistry("Test");
    auto* r = s_pRegistry;

    // Reset to the defaults, so the phases below do not depend on test
    // order or repetition.
    s_uDifficulty.data.as_uint32 = 4u;
    s_bEnablePvp.data.as_boolean = false;
    s_sPassword.StoreValue(s_sPassword, "");
    s_fInvalidNumber.data.as_float = 1.5f;
    ClearEnv("UDIFFICULTY");
    ClearEnv("BENABLEPVP");
    ClearEnv("SPASSWORD");
    ClearEnv("FINVALIDNUMBER");

    // Nothing set: every setting keeps its current value.
    LoadSettingsFromEnvironment(*r);
    EXPECT_EQ(r->FindSetting("Gameplay:uDifficulty")->data.as_uint32, 4u);
    EXPECT_FALSE(r->FindSetting("Gameplay:bEnablePvp")->data.as_boolean);

    // Set variables override the current values.
    SetEnv("UDIFFICULTY", "3");
    SetEnv("BENABLEPVP", "true");
    SetEnv("SPASSWORD", "supersecretpassword");

    LoadSettingsFromEnvironment(*r);

    EXPECT_EQ(r->FindSetting("Gameplay:uDifficulty")->data.as_uint32, 3u);
    EXPECT_TRUE(r->FindSetting("Gameplay:bEnablePvp")->data.as_boolean);
    EXPECT_EQ(std::string(r->FindSetting("GameServer:sPassword")->c_str()), "supersecretpassword");

    // A value that does not parse keeps the current setting.
    SetEnv("FINVALIDNUMBER", "not-a-number");
    LoadSettingsFromEnvironment(*r);
    EXPECT_FLOAT_EQ(r->FindSetting("Test:fInvalidNumber")->data.as_float, 1.5f);
}
} // namespace
} // namespace Console
