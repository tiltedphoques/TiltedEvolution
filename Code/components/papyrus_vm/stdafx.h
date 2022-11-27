#pragma once

#ifdef _WIN32
#define DEBUG_BREAK __debugbreak()
#else
#define DEBUG_BREAK
#endif

#include <cstdint>

#include <spdlog/spdlog.h>

#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Stl.hpp>
