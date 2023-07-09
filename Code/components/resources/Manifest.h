// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include "Pch.h"
#include "SemanticVersion2.h"

#include <filesystem>

namespace Resources
{
namespace fs = std::filesystem;

struct Manifest001
{
    struct DependencyTuple
    {
        TiltedPhoques::String Name;
        SemanticVersion Version;
    };

    SemanticVersion ApiSet;                              // < version of the manifest spec
    SemanticVersion ResourceVersion;                     // < user defined version of the resource
    TiltedPhoques::String Name;                          // < name of the resource
    TiltedPhoques::String Description;                   // < description of the resource
    TiltedPhoques::String Keywords;                      // < keywords for the resource
    TiltedPhoques::String License;                       // < license of the resource
    TiltedPhoques::String Repository;                    // < repository of the resource
    TiltedPhoques::String Homepage;                      // < homepage of the resource
    TiltedPhoques::String Author;                        // < author of the resource
    TiltedPhoques::String EntryPoint;                    // < entry point of the resource
    TiltedPhoques::Vector<DependencyTuple> Dependencies; // < dependencies of the resource
    fs::path FolderName;
    bool IsTombstone = false; // < if true, this manifest is a tombstone and should be ignored, this is only relevant
                              // during dependency resolving & loading
};

// all future manifest versions shall inherit from this.
} // namespace Resources
