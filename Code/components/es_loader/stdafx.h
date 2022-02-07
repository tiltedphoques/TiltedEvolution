#pragma once

#ifdef _WIN32
#define DEBUG_BREAK __debugbreak()
#else
#define DEBUG_BREAK
#endif

#include <cstdint>
#include <optional>

#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Buffer.hpp>
#include <TiltedCore/Serialization.hpp>
#include <glm/glm.hpp>

#include <spdlog/spdlog.h>

using namespace TiltedPhoques;
using TiltedPhoques::Serialization;

