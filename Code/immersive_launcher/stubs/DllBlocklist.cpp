#include <filesystem>
#include <iostream>
#include <fstream>
#include <regex>
#define SPDLOG_WCHAR_FILENAMES
#include <spdlog/formatter.h>

#include "utils/Error.h"        // For Die()
#include "DllBlocklist.h"
#include "../Launcher.h"        // For gameDir
#include "../TargetConfig.h"    // For PRODUCT_NAME

namespace stubs
{
// clang-format off

    // A list of modules blocked for a variety of reasons, but mainly
    // for causing crashes and incompatibility with ST
    const wchar_t* const kDllBlocklist[] = {
        L"EngineFixes.dll",          // Skyrim Engine Fixes, breaks our hooks
        L"crashhandler64.dll",       // Stream crash handler, breaks heap
        L"fraps64.dll",              // Breaks tilted ui
        L"SpecialK64.dll",           // breaks rendering
        L"ReShade64_SpecialK64.dll", // same reason
        L"NvCamera64.dll",           // broken af nvidia stuff, blacklisted for now, needs fix later
       // L"atiuxp64.dll",
       // L"aticfx64.dll"
    };

// clang-format on

struct DllGreyEntry
{
    const wchar_t* m_dllName;           // The name of the DLL to check.
    const wchar_t* m_configLocation;    // The relative path of the config file (from the game dir)
    const char*    m_configValidator;   // Regex to check if config is sane. Helps distinguish multiple releases of same dll.
    const char*    m_sigRegex;          // If this pattern is found in the config, comments were already added.
    const wchar_t* m_prompt;            // The MessageBox prompt asking for permission to fix.
    const char*    m_sigToInsert;       // This is added at the top of the rewritten config. It can/should say more than just the pattern match signature.
    const char*    m_replacers;         // Regex replacers. Regex & replacement separated by newlines.
};                                      //     You can have more than one replacer, also separated by newlines

// Data drive this. The intent is make sure we can put this in an external file for easy updates
// (although the format would probably be updated to something standard like JSON then, it 
// just isn't worth the additional code while there is only one)
const DllGreyEntry kDllGreyList[] = 
{
    {
        L"EngineFixes.dll", 
        L"Data\\SKSE\\Plugins\\EngineFixes.toml",
        "^VerboseLogging\\s*=",         // Matches EngineFixes Release 6.x series.
        "# SKYRIM TOGETHER REBORN marker for EngineFixes required compatibility settings v2, DO NOT CHANGE THIS LINE",

        L"For EngineFixes to work with Skyrim Together Reborn, some settings are required:\n"
        "\tMemoryManager = false\n"
        "\tScaleformAllocator = false\n"
        "\tMaxStdio = 8192\n\n"

        "OK:\tMakes the changes for you\n"
        "Cancel:\tEngineFixes will not load\n\n"

        "If later you get the (harmless) SrtCrashFix64 popup, manually make this EngineFixes configuration change to suppress it:\n"
        "\tAnimationLoadSignedCrash = false",

        "# SKYRIM TOGETHER REBORN marker for EngineFixes required compatibility settings v2, DO NOT CHANGE THIS LINE\n"
        "# For EngineFixes to work with Skyrim Together Reborn, some settings are required:\n"
        "#    MemoryManager = false\n"
        "#    ScaleformAllocator = false\n"
        "#    MaxStdio = 8192\n"
        "#\n"

        "# If you get a SrtCrashFix64 popup, it is because you've loaded a mod like Animation Limit Crash Fixe SSE\n"
        "# that is doing the same thing as EngineFixes. Manually set\n"
        "#    AnimationLoadSignedCrash = false\n"
        "# to eliminate the annoying popup.\n\n",

        "(^\\s*MemoryManager\\s*=\\s*)(true ?|false)\n$1false\n"
        "(^\\s*ScaleformAllocator\\s*=\\s*)(true ?|false)\n$1false\n"
        "(^\\s*MaxStdio\\s*=\\s*)([0-9]+)\n$018192\n" // Only huge builds need this many files, but make EF match what STR sets.
    },
    {
        L"EngineFixes.dll", 
        L"Data\\SKSE\\Plugins\\EngineFixes.toml",
        "^bVerboseLogging\\s*=",        // Matches EngineFixes Release 7.x series.
        "",                             // No sig regex
        L"",                            // No prompt
        "",                             // No sig to insert
        nullptr                         // We need to check for EF7 vs. EF6, but don't need any changes for EF7.
    }
};

enum GreyListDisposition
{
    kNotGreyList,       // Not on the greylist at all
    kGreyListAccept,    // It is on the greylist with a good config, or the changes were accepted
    kGreyListAbort      // It is on the greylist and the configuration is no good.
};


// This is long, lots of error checking and user interaction, but the comments call out the major steps in the sequence. 
GreyListDisposition IsConfigOK(const std::filesystem::path& aPath, const DllGreyEntry& aEntry)
{
    std::regex signatureRegex; 
    std::stringstream configStream;    

    // Read the entire config into a string buffer.
    // If the config doesn't exist or can't be read, it may be
    // that the mod isn't installed at all. Or we don't have 
    // permissions, which means the mod won't be able to read 
    // the config either and it will be handled there.
    // Returning Accept keeps everthing quiet (not installed is OK),
    // and if config is protected it is the mod's job to handle it 
    std::filesystem::path configFile = aPath / aEntry.m_configLocation;
    std::fstream file(configFile, std::ios::in);
    if (!file.good())
        return kGreyListAccept;
    configStream << file.rdbuf();
    if (configStream.bad() || file.bad())
    {
        auto msg = fmt::format(__FUNCTION__ L": failed to read {}", aEntry.m_configLocation);
        Die(msg.c_str(), true);
        return kGreyListAbort;
    }

    // Rewind config stream and prepare to generate newConfig;
    configStream.clear(); // Clear any EOF flags
    configStream.seekg(0, std::ios::beg);
    std::string newConfig;   

    try
    {   // std::regex constructors can throw exceptions, so must be used inside try/catch
        // subblock to throw away the temps
        {
            const std::regex validatorRegex(aEntry.m_configValidator, std::regex_constants::icase);
            const std::string configStr = configStream.str();
            std::smatch vmatch;
            if (!std::regex_search(configStr.begin(), configStr.end(), vmatch, validatorRegex))
                return kGreyListAbort;
        }

        // This has to be done inside the try/catch, but will be used outside at the end.
        signatureRegex = std::regex(aEntry.m_sigRegex, std::regex_constants::icase);

        // Iterate over each line of the config file
        //     Iterate over each replacer, possibly changing the line
        //     Output upddated line to new config
        if (!aEntry.m_replacers)
            return kGreyListAccept; // Must be good if nothing to change.

        std::string line;
        std::stringstream replacers(aEntry.m_replacers);

        while (configStream.good() && std::getline(configStream, line))
        {
            std::string pattern;
            std::string withThat;
            while (replacers.good() && std::getline(replacers, pattern) && std::getline(replacers, withThat))
            {
                std::regex replaceThis(pattern, std::regex_constants::icase);

                // Stepping through these two debugging lines is very helpful for finding broken expressions.
                std::smatch matches;
                std::regex_search(line, matches, replaceThis);

                line = std::regex_replace(line, replaceThis, withThat);
            }

            newConfig += line + "\n"; // Save the possibly edited line.
            replacers.clear();
            replacers.seekg(0, std::ios::beg);
        }
    }

    catch ([[maybe_unused]] const std::regex_error& e)
    {
        auto msg = fmt::format(__FUNCTION__ L": invalid regular expression in DLL grey list handling");
        Die(msg.c_str(), true);
        return kGreyListAbort;
    }

    // Check: is the newConfig equal to the original? If so, we're done.
    if (_stricmp(newConfig.c_str(), configStream.str().c_str()) == 0)
        return kGreyListAccept;

    // Ask user if they want to fix config, or not load the mod.
    if (MessageBoxW(NULL, aEntry.m_prompt, PRODUCT_NAME L" Requires Configuration Changes", MB_OKCANCEL | MB_ICONWARNING) == IDCANCEL)
        return kGreyListAbort;

    // Have to update the config file.
    // If the newConfig doesn't have the signature in it, insert it to the output.
    // Then save the new configuration
    if (!std::regex_search(newConfig, signatureRegex))
        newConfig = aEntry.m_sigToInsert + newConfig;
    
    // Have to reopen the stream to truncate it to zero, this also check write permissions
    file = std::fstream(configFile, std::ios::in | std::ios::out | std::ios::trunc);

    if (!file.bad())
      file << newConfig;

    if (file.bad())
    {
        auto msg = fmt::format(__FUNCTION__ L": failed to rewrite {}", configFile.c_str());
        Die(msg.c_str(), true);
        return kGreyListAbort;
    }

    return kGreyListAccept;
}


// "GreyList" DLLs are ones that can run if they are configured properly.
// It's only applicable to SKSE-loaded add-ons, as far as I can tell.
// They will be in the Data\SKSE\Plugins directory, so we have to know 
// little things like the gamePath. 
// This code runs so early you may not know gamePath yet, 
// So guard against it not being set up.
enum GreyListDisposition IsDllGreyListBlocked(const std::wstring_view aDllName)
{
    GreyListDisposition retval = kNotGreyList;
    launcher::LaunchContext* LC = launcher::GetLaunchContext(); 

    if (!LC)
        return kNotGreyList;

    std::filesystem::path gamePath = LC->gamePath;
    for (auto& greyListEntry : kDllGreyList)
    {
        if (std::wcscmp(aDllName.data(), greyListEntry.m_dllName) == 0)
        {
            // DLL name matches, read in entire config file.
            // Might be multiple configs for differing versions, 
            // so only exit iteration early if config accepted.
            retval = IsConfigOK(gamePath, greyListEntry);
            if (retval == kGreyListAccept)
                break;
        }
    }
    return retval;
}



bool IsDllBlocked(std::wstring_view dllName)
{
    GreyListDisposition retval = IsDllGreyListBlocked(dllName);

    if (retval != kNotGreyList)
        return retval == kGreyListAbort;

    for (const wchar_t* dllEntry : kDllBlocklist)
    {
        if (std::wcscmp(dllName.data(), dllEntry) == 0)
        {
            return true;
        }
    }

    return false;
}

bool IsSoulsRE(std::wstring_view dllName)
{
    const wchar_t *dllEntry = L"SkyrimSoulsRE.dll";
    if (std::wcscmp(dllName.data(), dllEntry) == 0)
        return true;

    return false;
}
} // namespace stubs
