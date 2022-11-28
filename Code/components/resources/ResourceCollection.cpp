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
    CSimpleIni ini;
    {
        auto buf = TiltedPhoques::LoadFile(aPath);
        BASE_ASSERT(ini.LoadData(buf.c_str()) == SI_Error::SI_OK, "Failed to load ini data");
    }

    double resource_version = ini.GetDoubleValue("Resource", "apiset", 1.0);
}

void ResourceCollection::CollectResources()
{
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
}
} // namespace Resources
