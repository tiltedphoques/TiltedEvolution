// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.

#include "ResourceCollection.h"
#include "SemanticVersion2.h"

#include <base/Check.h>
#include <base/simpleini/SimpleIni.h>

namespace Resources
{
namespace
{
constexpr char kResourceFolderName[] = "resources";
constexpr char kResourceManifestExt[] = ".manifest";

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

Manifest001::DependencyTuple SplitDependencyString(const TiltedPhoques::String& aString)
{
    size_t atPos = aString.find_first_of('@');
    if (atPos == TiltedPhoques::String::npos)
    {
        return {aString, {0, 0, 0}};
    }
    auto name = aString.substr(0, atPos);
    auto versionString = aString.substr(atPos + 1);

    SemanticVersion version(versionString.c_str());
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
    auto readSemVer = [&ini](const char* apName) -> SemanticVersion {
        return ini.GetValue("Manifest", apName, "0.0.0");
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
                    if (dep.second && dep.second != it2->resourceVersion)
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
