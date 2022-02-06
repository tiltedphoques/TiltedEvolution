// Copyright (C) 2021 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

namespace oobe
{
enum class CompatabilityStatus
{
    kAllGood,
    kDX11Unsupported,
    kOldOS
};

CompatabilityStatus ReportModCompatabilityStatus();
} // namespace oobe
