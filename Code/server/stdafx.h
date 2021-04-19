#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef TP_SKYRIM
#define SKYRIM 1
#define FALLOUT4 0
#else
#define SKYRIM 0
#define FALLOUT4 1
#endif

#include <cstdint>
#include <TiltedCore/Platform.hpp>
#include <TiltedCore/StackAllocator.hpp>
#include <TiltedCore/ScratchAllocator.hpp>
#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Outcome.hpp>
#include <TiltedCore/ViewBuffer.hpp>

#include <any>
#include <mutex>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <codecvt>

#include <Server.hpp>
#include <cxxopts.hpp>

#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <BuildInfo.h>

extern "C"
{
#include <lua.h>
#include <lsqlite3.h>
}

#define SOL_ALL_SAFETIES_ON 1
//#define SOL_USE_CXX_LUA_I_
#include <sol/sol.hpp>

using namespace TiltedPhoques;
using namespace std::chrono_literals;

#undef GetClassName
