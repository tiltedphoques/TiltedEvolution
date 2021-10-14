// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

namespace oobe
{
enum class Policy
{
    kLax,
    kRecommended,
    kTesting
};

bool TestPlatformViability(Policy);
} // namespace oobe
