// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include "Manifest.h"

#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Platform.hpp>

namespace Resources
{
class ResourceCollection
{
public:
    ResourceCollection();
    ~ResourceCollection();

    void CollectResources();

    auto& GetResourceFolderPath() const { return m_resourcePath; }

    bool ValidateDependencyVersions();

    bool LoadManifestData(const std::filesystem::path& aPath);

private:
    std::filesystem::path m_resourcePath;
    TiltedPhoques::Vector<TiltedPhoques::UniquePtr<Resources::Manifest001>> manifests_;
};
} // namespace Resources
