// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <base/Check.h>

#include <console/ConsoleRegistry.h>
#include <console/ConsoleUtils.h>
#include <console/EnvSettingsProvider.h>
#include <console/Setting.h>

#include <cctype>
#include <cstdlib>
#include <string_view>

namespace Console
{
std::string ToEnvironmentVariableName(const char* apSettingName)
{
    std::string_view name(apSettingName);

    // Strip the "Section:" prefix, the key alone names the variable.
    const auto cSplitLoc = name.find_first_of(':');
    if(cSplitLoc != std::string_view::npos)
        name = name.substr(cSplitLoc + 1);

    std::string result;
    result.reserve(name.size());
    for(const char c : name)
        result += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

    return result;
}

namespace
{
bool ParseBoolValue(const char* apValue, bool aCurrent)
{
    // Same spellings the runner already accepts for TILTED_ACCEPT_EULA,
    // plus the explicit false ones. Anything else keeps the current value.
    const std::string_view value(apValue);
    if(value == "true" || value == "True" || value == "TRUE" || value == "1")
        return true;
    if(value == "false" || value == "False" || value == "FALSE" || value == "0")
        return false;
    return aCurrent;
}
} // namespace

void LoadSettingsFromEnvironment(ConsoleRegistry& aReg)
{
    aReg.ForAllSettings(
        [&](SettingBase* setting)
        {
            const std::string variable = ToEnvironmentVariableName(setting->name);
            const char* pValue = std::getenv(variable.c_str());
            if(!pValue)
                return;

            switch(setting->type)
            {
            case SettingBase::Type::kBoolean: setting->data.as_boolean = ParseBoolValue(pValue, setting->data.as_boolean); break;
            case SettingBase::Type::kInt: setting->data.as_int32 = ConvertStringValue(pValue, setting->data.as_int32); break;
            case SettingBase::Type::kUInt: setting->data.as_uint32 = ConvertStringValue(pValue, setting->data.as_uint32); break;
            case SettingBase::Type::kInt64: setting->data.as_int64 = ConvertStringValue(pValue, setting->data.as_int64); break;
            case SettingBase::Type::kUInt64: setting->data.as_uint64 = ConvertStringValue(pValue, setting->data.as_uint64); break;
            case SettingBase::Type::kFloat: setting->data.as_float = ConvertStringValue(pValue, setting->data.as_float); break;
            // Strings however are a special case, as it has its own allocator.
            case SettingBase::Type::kString:
            {
                static_cast<StringSetting*>(setting)->StoreValue(*setting, pValue);
                break;
            }
            default: BASE_ASSERT(true, "LoadSettingsFromEnvironment(): Unknown type index for {}", setting->name); break;
            }
        });
}
} // namespace Console
