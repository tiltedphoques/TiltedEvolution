// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include "Pch.h"

namespace Resources
{
struct Manifest001
{
    uint32_t apiSet;                   // < major * 100 + minor
    uint32_t version;                  // < major>.<minor>.<patch>
    TiltedPhoques::String name;        // < name of the resource
    TiltedPhoques::String description; // < description of the resource
    TiltedPhoques::String keywords;    // < keywords for the resource
    TiltedPhoques::String license;     // < license of the resource
    TiltedPhoques::String repository;  // < repository of the resource
    TiltedPhoques::String homepage;    // < homepage of the resource
    TiltedPhoques::String author;      // < author of the resource
    TiltedPhoques::String entryPoint;  // < entry point of the resource
};

// all future manifest versions shall inherit from this.
} // namespace Resources
