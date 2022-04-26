// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <base/Check.h>
#include <base/simpleini/SimpleIni.h>

#include <console/IniSettingsProvider.h>
#include <console/ConsoleUtils.h>
#include <console/Setting.h>
#include <console/ConsoleRegistry.h>

#include <charconv>
#include <fstream>

#include <TiltedCore/Filesystem.hpp>

namespace Console
{
namespace
{
template <typename T, typename TVal>
static SI_Error SetIniValue(CSimpleIni& ini, const T* a_pSection, const T* a_pKey, const TVal a_nValue,
                            const T* a_pComment = nullptr)
{
    char szValue[64]{};
    std::to_chars(szValue, szValue + sizeof(szValue), a_nValue);

    // convert to output text
    T szOutput[256];
    CSimpleIni::Converter c(ini.IsUnicode());
    c.ConvertFromStore(szValue, std::strlen(szValue) + 1, szOutput, sizeof(szOutput) / sizeof(T));

    return ini.AddEntry(a_pSection, a_pKey, szOutput, a_pComment, false, true);
}

template <typename T, typename TVal>
TVal GetIniValue(CSimpleIni& ini, const T* a_pSection, const T* a_pKey, const TVal a_nDefault, bool& a_pHasMultiple)
{
    const T* pszValue = ini.GetValue(a_pSection, a_pKey, nullptr, &a_pHasMultiple);
    if (!pszValue || !*pszValue)
        return a_nDefault;

    // convert to UTF-8/MBCS which for a numeric value will be the same as ASCII
    char szValue[64]{};
    CSimpleIni::Converter c(ini.IsUnicode());
    if (!c.ConvertToStore(pszValue, szValue, sizeof(szValue)))
    {
        return a_nDefault;
    }

    return ConvertStringValue<TVal>(szValue, a_nDefault);
}

std::pair<std::string, std::string> SplitSection(const SettingBase* setting)
{
    std::string nameProperty(setting->name);
    size_t pos = nameProperty.find_first_of(':');

    std::string section = pos == std::string_view::npos ? "general" : nameProperty.substr(0, pos);
    auto name = &nameProperty[pos + 1];

    return {section, name};
}
} // namespace

void SaveSettingsToIni(ConsoleRegistry& aReg, const std::filesystem::path& aPath)
{
    CSimpleIni ini;

    SI_Error error{SI_Error::SI_OK};
    aReg.ForAllSettings([&](SettingBase* setting) {
        auto items = SplitSection(setting);
        auto& section = items.first;
        auto& name = items.second;

        switch (setting->type)
        {
        case SettingBase::Type::kBoolean:
            error = ini.SetBoolValue(section.c_str(), name.c_str(), setting->data.as_boolean);
            break;
        case SettingBase::Type::kInt:
            error = SetIniValue(ini, section.c_str(), name.c_str(), setting->data.as_int32);
            break;
        case SettingBase::Type::kUInt:
            error = SetIniValue(ini, section.c_str(), name.c_str(), setting->data.as_uint32);
            break;
        case SettingBase::Type::kInt64:
            error = SetIniValue(ini, section.c_str(), name.c_str(), setting->data.as_int64);
            break;
        case SettingBase::Type::kUInt64:
            error = SetIniValue(ini, section.c_str(), name.c_str(), setting->data.as_uint64);
            break;
        case SettingBase::Type::kFloat:
            error = SetIniValue(ini, section.c_str(), name.c_str(), setting->data.as_float);
            break;
        case SettingBase::Type::kString:
            error = ini.SetValue(section.c_str(), name.c_str(), setting->c_str());
            break;
        default:
            BASE_ASSERT(true, "SaveSettingsToIni(): Unknown type index for {}", setting->name);
            break;
        }

        if (error != SI_Error::SI_OK)
        {
            BASE_ASSERT(true, "Failed to write ini setting.");
        }
    });

    std::string buf;
    error = ini.Save(buf, true);
    BASE_ASSERT(error == SI_Error::SI_OK, "Saving the ini failed");

    // TODO(Vince): eventually we shall burn the ini library
    TiltedPhoques::SaveFile(aPath, TiltedPhoques::String(buf));
}

void LoadSettingsFromIni(ConsoleRegistry& aReg, const std::filesystem::path& aPath)
{
    CSimpleIni ini;
    {
        auto buf = TiltedPhoques::LoadFile(aPath);
        BASE_ASSERT(ini.LoadData(buf.c_str()) == SI_Error::SI_OK, "Failed to load ini data");
    }

    aReg.ForAllSettings([&](SettingBase* setting) {
        auto items = SplitSection(setting);
        auto& section = items.first;
        auto& name = items.second;

        bool multiMatch = false;
        switch (setting->type)
        {
        // With scalar types we don't expect the size to change...
        // However, they should all call StoreValue in the future.
        case SettingBase::Type::kBoolean:
            setting->data.as_boolean = ini.GetBoolValue(section.c_str(), name.c_str(), setting->data.as_boolean);
            break;
        case SettingBase::Type::kInt:
            setting->data.as_int32 =
                GetIniValue(ini, section.c_str(), name.c_str(), setting->data.as_int32, multiMatch);
            break;
        case SettingBase::Type::kUInt:
            setting->data.as_uint32 =
                GetIniValue(ini, section.c_str(), name.c_str(), setting->data.as_uint32, multiMatch);
            break;
        case SettingBase::Type::kInt64:
            setting->data.as_int64 =
                GetIniValue(ini, section.c_str(), name.c_str(), setting->data.as_int64, multiMatch);
            break;
        case SettingBase::Type::kUInt64:
            setting->data.as_uint64 =
                GetIniValue(ini, section.c_str(), name.c_str(), setting->data.as_uint64, multiMatch);
            break;
        case SettingBase::Type::kFloat:
            setting->data.as_float =
                GetIniValue(ini, section.c_str(), name.c_str(), setting->data.as_float, multiMatch);
            break;
        // Strings however are a special case, as it has its own allocator.
        case SettingBase::Type::kString: {
            // This is not at all how i want it to be :/
            const char* c = ini.GetValue(section.c_str(), name.c_str(), setting->c_str());
            static_cast<StringSetting*>(setting)->StoreValue(*setting, c);
            break;
        }
        default:
            BASE_ASSERT(true, "LoadSettingsFromIni(): Unknown type index for {}", setting->name);
            break;
        }
    });
}
} // namespace base
