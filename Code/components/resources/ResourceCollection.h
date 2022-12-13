// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include "Manifest.h"

#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Platform.hpp>

namespace Resources
{
struct ResourceCollection
{
public:
    ResourceCollection();
    ~ResourceCollection();

    void CollectResources();

    auto& GetResourceFolderPath() const { return m_resourcePath; }

    void ResolveDependencies();

    bool LoadManifestData(const std::filesystem::path& aPath);

    const auto& GetManifests() const { return m_manifests; }

    template <typename T> void ForEachManifest(const T& aCallback) const noexcept
    {
        for (const auto& manifest : m_manifests)
        {
            aCallback(*manifest);
        }
    }

private:
    std::filesystem::path m_resourcePath;
    TiltedPhoques::Vector<TiltedPhoques::UniquePtr<Resources::Manifest001>> m_manifests;
};
} // namespace Resources
