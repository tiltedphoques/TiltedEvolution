#pragma once

#define NOMINMAX

#ifdef TP_SKYRIM
#define SKYRIM 1
#define FALLOUT4 0
#else
#define SKYRIM 0
#define FALLOUT4 1
#endif

#include <Platform.hpp>
#include <cstdint>

#include <StackAllocator.hpp>
#include <ScratchAllocator.hpp>
#include <Stl.hpp>
#include <Outcome.hpp>
#include <Vector3.hpp>
#include <ViewBuffer.hpp>

#include <any>
#include <mutex>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <codecvt>

#include <Server.hpp>
#include <cxxopts.hpp>

#include <spdlog/spdlog.h>

#include <client_server.pb.h>

#include <entt/entt.hpp>

extern "C"
{
#include <lua.h>
#include <lsqlite3.h>
}

#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>

using namespace TiltedPhoques;

using namespace std::chrono_literals;

#undef GetClassName

