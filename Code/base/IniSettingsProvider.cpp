// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <base/Check.h>
#include <base/IniSettingsProvider.h>
#include <base/Setting.h>
#include <base/simpleini/SimpleIni.h>

#include <fstream>
#include <charconv>

#include <TiltedCore/Filesystem.hpp>

namespace base
{
namespace
{
static void ShittyFileWrite(const std::filesystem::path& path, const std::string& data)
{
    // TODO: Get rid of this, its horrible.
    std::ofstream myfile(path.c_str());
    myfile << data.c_str();
    myfile.close();
}

template <typename T, typename TVal>
static SI_Error SetIniValue(CSimpleIni& ini, const T* a_pSection, const T* a_pKey, TVal a_nValue,
                            const T* a_pComment = nullptr)
{
    if (!a_pSection || !a_pKey)
        return SI_FAIL;

    // TODO: switch to to_chars
    auto buf = fmt::format("{}", a_nValue);

    // convert to output text
    T szOutput[256];
    CSimpleIni::Converter c(ini.IsUnicode());
    c.ConvertFromStore(buf.c_str(), buf.length() + 1, szOutput, sizeof(szOutput) / sizeof(T));

    // actually add it
    return ini.AddEntry(a_pSection, a_pKey, szOutput, a_pComment, false, true);
}

template <typename T, typename TVal>
TVal GetIniValue(CSimpleIni& ini, const T* a_pSection, const T* a_pKey, const TVal a_nDefault, bool& a_pHasMultiple)
{
    // return the default if we don't have a value
    const T* pszValue = ini.GetValue(a_pSection, a_pKey, nullptr, &a_pHasMultiple);
    if (!pszValue || !*pszValue)
        return a_nDefault;

    // convert to UTF-8/MBCS which for a numeric value will be the same as ASCII
    char szValue[64] = {0};
    CSimpleIni::Converter c(ini.IsUnicode());
    if (!c.ConvertToStore(pszValue, szValue, sizeof(szValue)))
    {
        return a_nDefault;
    }

    TVal nValue = a_nDefault;
    auto [ptr, ec] = std::from_chars(szValue, szValue + std::strlen(szValue), nValue);
    if (ec == std::errc())
        // TODO; REPORT ERR
        return nValue;

    return a_nDefault;
}

std::pair<std::string, const char*> SplitSection(const SettingBase *setting)
{
    std::string nameProperty(setting->name);
    size_t pos = nameProperty.find_first_of(':');

    std::string section = pos == std::string_view::npos ? "general" : nameProperty.substr(0, pos);
    auto name = &nameProperty[pos + 1];

    return {section, name};
}
}

void SaveSettingsToIni(const std::filesystem::path& path)
{
    CSimpleIni ini;

    SI_Error error{SI_Error::SI_OK};

    SettingBase::VisitAll([&](SettingBase* setting) {
        auto items = SplitSection(setting);
        auto& section = items.first;
        auto name = items.second;

        switch (setting->type)
        {
        case SettingBase::Type::kBoolean:
            error = ini.SetBoolValue(section.c_str(), name, setting->data.as_boolean);
            break;
        case SettingBase::Type::kInt:
            error = SetIniValue(ini, section.c_str(), name, setting->data.as_int32);
            break;
        case SettingBase::Type::kUInt:
            error = SetIniValue(ini, section.c_str(), name, setting->data.as_uint32);
            break;
        case SettingBase::Type::kInt64:
            error = SetIniValue(ini, section.c_str(), name, setting->data.as_int64);
            break;
        case SettingBase::Type::kUInt64:
            error = SetIniValue(ini, section.c_str(), name, setting->data.as_uint64);
            break;
        case SettingBase::Type::kFloat:
            error = SetIniValue(ini, section.c_str(), name, setting->data.as_float);
            break;
        case SettingBase::Type::kString:
            error = ini.SetValue(section.c_str(), name, setting->c_str());
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
    ini.Save(buf, true);

    ShittyFileWrite(path, buf);
}

void LoadSettingsFromIni(const std::filesystem::path& path)
{
    CSimpleIni ini;
    {
        auto buf = TiltedPhoques::LoadFile(path);
        ini.LoadData(buf.c_str());
    }

    SettingBase::VisitAll([&](SettingBase* setting) {
        auto items = SplitSection(setting);
        auto& section = items.first;
        auto name = items.second;

        bool multiMatch = false;
        switch (setting->type)
        {
        case SettingBase::Type::kBoolean:
            setting->data.as_boolean = ini.GetBoolValue(section.c_str(), name, setting->data.as_boolean);
            break;
        case SettingBase::Type::kInt:
            setting->data.as_int32 = GetIniValue(ini, section.c_str(), name, setting->data.as_int32, multiMatch);
            break;
        case SettingBase::Type::kUInt:
            setting->data.as_uint32 = GetIniValue(ini, section.c_str(), name, setting->data.as_uint32, multiMatch);
            break;
        case SettingBase::Type::kInt64:
            setting->data.as_int64 = GetIniValue(ini, section.c_str(), name, setting->data.as_int64, multiMatch);
            break;
        case SettingBase::Type::kUInt64:
            setting->data.as_uint64 = GetIniValue(ini, section.c_str(), name, setting->data.as_uint64, multiMatch);
            break;
        case SettingBase::Type::kFloat:
            setting->data.as_float = GetIniValue(ini, section.c_str(), name, setting->data.as_float, multiMatch);
            break;
        case SettingBase::Type::kString:
            // TODO: string_storage
            //setting->data.as_string = GetIniValue(ini, section.c_str(), name, setting->data.as_float, multiMatch);
            break;
        default:
            BASE_ASSERT(true, "LoadSettingsFromIni(): Unknown type index for {}", setting->name);
            break;
        }
    });
}
} // namespace base
