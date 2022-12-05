// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "ResourceCollection.h"

#include <base/Check.h>
#include <base/simpleini/SimpleIni.h>

namespace Resources
{
namespace
{
constexpr char kResourceFolderName[] = "resources";
constexpr char kResourceManifestExt[] = ".manifest";

// The function takes a string containing a version number in the format X.X.X (where each X is a single digit) and
// converts it to a 32-bit integer by encoding each digit in 4 bits, with the most significant digit being stored in the
// highest-order 4 bits of the integer. The periods are ignored. For example, the string "1.2.3" would be converted to
// the integer value 0x123.
static uint32_t ParseSemVer(const char* apText)
{
    uint32_t version = 0;
    uint32_t shift = 0;
    while (*apText)
    {
        if (*apText == '.')
        {
            shift += 8;
        }
        else
        {
            version |= (*apText - '0') << shift;
            shift += 4;
        }
        ++apText;
    }
    return version;
}

// For each digit, we check if it is the least significant digit, and if it is not, we appends a period (.) to
// the string. Then, the code appends the character representation of the digit (by adding '0' to the numeric value of
// the digit) to the string.
TiltedPhoques::String SemVerToString(uint32_t aVersion)
{
    TiltedPhoques::String result;
    for (int i = 2; i >= 0; --i)
    {
        if (i < 2)
        {
            result += '.';
        }
        result += '0' + ((aVersion >> (i * 4)) & 0xF);
    }
    return result;
}

// to create a character, simply append '0'
uint8_t GetMinorNumber(uint32_t aVersion)
{
    return (aVersion >> 4) & 0xF;
}

uint8_t GetMajorNumber(uint32_t aVersion)
{
    return (aVersion >> 8) & 0xF;
}

uint8_t GetPatchNumber(uint32_t aVersion)
{
    return aVersion & 0xF;
}

TiltedPhoques::Vector<TiltedPhoques::String> ParseQuotedTokens(const TiltedPhoques::String& aString)
{
    TiltedPhoques::Vector<TiltedPhoques::String> result;
    size_t start = aString.find_first_of('"');
    while (start != std::string::npos)
    {
        size_t end = aString.find_first_of('"', start + 1);
        if (end == std::string::npos)
        {
            break;
        }
        result.push_back(aString.substr(start + 1, end - start - 1));
        start = aString.find_first_of('"', end + 1);
        while (start != std::string::npos && isspace(aString[start]))
        {
            ++start;
        }
    }
    return result;
}

std::pair<TiltedPhoques::String, uint32_t> SplitDependencyString(const TiltedPhoques::String& aString)
{
    size_t atPos = aString.find_first_of('@');
    if (atPos == TiltedPhoques::String::npos)
    {
        return {aString, 0};
    }
    auto name = aString.substr(0, atPos);
    auto versionString = aString.substr(atPos + 1);
    uint32_t version = ParseSemVer(versionString.c_str());
    return {name, version};
}

TiltedPhoques::String UnescapeAndStrip(const std::string_view& aString)
{
    TiltedPhoques::String result;
    for (char c : aString)
    {
        if (c == '\\')
        {
            continue;
        }
        else if (c == '"')
        {
            continue;
        }
        else if (c == '\n')
        {
            continue;
        }
        result += c;
    }
    return result;
}
} // namespace

ResourceCollection::ResourceCollection()
{
    CollectResources();
}

ResourceCollection::~ResourceCollection()
{
}

bool ResourceCollection::LoadManifestData(const std::filesystem::path& aPath)
{
    // example:
    // name = "my-resource"
    // resource_version = 1.0.0
    // apiset = 1.0.0
    // description = "A resource that does stuff"
    // keywords = ["selfmade", "resource"]
    // license = "MIT"
    // repository = ""
    // homepage = ""
    // entrypoint = "test.lua"
    // dependencies = ["dependency-a@1.0.0", "dependency-b@1.0.0"]

    CSimpleIni ini;
    {
        auto buf = TiltedPhoques::LoadFile(aPath);
        BASE_ASSERT(ini.LoadData(buf.c_str()) == SI_Error::SI_OK, "Failed to load ini data");
    }

    // the easy to load data.
    auto manifest = TiltedPhoques::MakeUnique<Resources::Manifest001>();

    // text
    auto readString = [&ini](const char* apName) -> TiltedPhoques::String {
        auto value = ini.GetValue("Manifest", apName, "<unknown>");
        return UnescapeAndStrip(value);
    };
    manifest->name = readString("name");
    manifest->description = readString("description");
    manifest->license = readString("license");
    manifest->repository = readString("repository");
    manifest->homepage = readString("homepage");
    manifest->entryPoint = readString("entrypoint");

    // numerics
    auto readSemVer = [&ini](const char* apName) -> uint32_t {
        return ParseSemVer(ini.GetValue("Manifest", apName, "0.0.0"));
    };
    manifest->apiSet = readSemVer("apiset");
    manifest->resourceVersion = readSemVer("resource_version");

    // lists of strings
    auto readStringList = [&ini](const char* apName) -> TiltedPhoques::Vector<TiltedPhoques::String> {
        return ParseQuotedTokens(ini.GetValue("Manifest", apName, ""));
    };
    for (const auto& dep : readStringList("dependencies"))
    {
        manifest->dependencies.push_back(SplitDependencyString(dep));
    }
    manifests_.push_back(std::move(manifest));

    return true;
}

bool ResourceCollection::ValidateDependencyVersions()
{
    for (auto& it : manifests_)
    {
        for (const auto& dep : it->dependencies)
        {
            for (auto& it2 : manifests_)
            {
                if (it2->name == dep.first)
                {
                    if (dep.second != 0 && dep.second != it2->resourceVersion)
                    {
                        spdlog::error("Dependency {} has version {} but {} is required", dep.first,
                                      SemVerToString(it2->resourceVersion), SemVerToString(dep.second));
                    }

                    // let the loop continue to list all other conflicts immedeatly.
                }
            }
        }
    }

    return true;
}

void ResourceCollection::CollectResources()
{
    auto x = SemVerToString(0x123);
    auto y = SemVerToString(0x321);

    m_resourcePath = std::filesystem::current_path() / kResourceFolderName;

    std::vector<std::filesystem::path> manifestCanidates;
    for (auto const& dir : std::filesystem::recursive_directory_iterator{m_resourcePath})
    {
        if (dir.path().extension() == kResourceManifestExt)
        {
            manifestCanidates.push_back(dir.path());
        }
    }

    for (const auto& path : manifestCanidates)
    {
        LoadManifestData(path);
    }

    ValidateDependencyVersions();
}
} // namespace Resources
