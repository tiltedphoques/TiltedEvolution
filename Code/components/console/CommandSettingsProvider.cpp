// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include <base/Check.h>

#include <console/CommandSettingsProvider.h>
#include <console/ConsoleUtils.h>
#include <console/Setting.h>
#include <console/ConsoleRegistry.h>

namespace Console
{

void LoadSettingsFromCommand(ConsoleRegistry& aReg, int argc, char** argv)
{
    Map<std::string, std::string> args;
    for (auto i = 1; i < argc; ++i)
    {
        std::string value = argv[i];
        value = value.substr(value.find_first_not_of('-'));
        const auto cSplitLoc = value.find_first_of('=');
        if (cSplitLoc != std::string::npos)
        {
            std::string name = value.substr(0, cSplitLoc);
            std::string arg = value.substr(cSplitLoc + 1);
            args[name] = arg;
        }
        else
            args[value] = "true";
    }


     aReg.ForAllSettings([&](SettingBase* setting) {
         const auto pName = setting->name;
         if (!args.contains(pName))
            return;

         switch (setting->type)
         {
         // With scalar types we don't expect the size to change...
         // However, they should all call StoreValue in the future.
         case SettingBase::Type::kBoolean:
            setting->data.as_boolean = args[pName] == "TRUE" || args[pName] == "true";
            break;
         case SettingBase::Type::kInt:
            setting->data.as_int32 = std::stol(args[pName]);
            break;
         case SettingBase::Type::kUInt:
            setting->data.as_uint32 = std::stoul(args[pName]);
            break;
         case SettingBase::Type::kInt64:
            setting->data.as_int64 = std::stoll(args[pName]);
            break;
         case SettingBase::Type::kUInt64:
            setting->data.as_uint64 = std::stoull(args[pName]);
            break;
         case SettingBase::Type::kFloat:
            setting->data.as_float = std::stof(args[pName]);
            break;
         // Strings however are a special case, as it has its own allocator.
         case SettingBase::Type::kString: {
            // This is not at all how i want it to be :/
            static_cast<StringSetting*>(setting)->StoreValue(*setting, args[pName].c_str());
            break;
         }
         default:
            BASE_ASSERT(true, "LoadSettingsFromCommand(): Unknown type index for {}", setting->name);
            break;
        }
    });
}
} // namespace base
