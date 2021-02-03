#pragma once

#define NOMINMAX

#include <Platform.hpp>

#if defined(TP_SKYRIM) && TP_PLATFORM_64
#define TP_SKYRIM64 1
#define TP_SKYRIM32 0
#define TP_FALLOUT4 0
#elif defined(TP_SKYRIM) && TP_PLATFORM_32
#define TP_SKYRIM64 0
#define TP_SKYRIM32 1
#define TP_FALLOUT4 0
#elif defined(TP_FALLOUT) && TP_PLATFORM_64
#define TP_SKYRIM64 0
#define TP_SKYRIM32 0
#define TP_FALLOUT4 1
#else
#error No game matches these build options
#endif


#include <windows.h>
#include <intrin.h>

#include <cstdint>

#include <StackAllocator.hpp>
#include <ScratchAllocator.hpp>
#include <Stl.hpp>
#include <Outcome.hpp>
#include <AutoPtr.hpp>
#include <App.hpp>
#include <FunctionHook.hpp>
#include <Entry.hpp>
#include <Debug.hpp>
#include <Buffer.hpp>
#include <ViewBuffer.hpp>
#include <ThisCall.hpp>
#include <TaskQueue.hpp>
#include <Math.hpp>
#include <Vector3.hpp>
#include <Initializer.hpp>
#include <Filesystem.hpp>

#include <entt/entt.hpp>

#include <spdlog/spdlog.h>

#include <any>
#include <mutex>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <Utils.h>

#include <Games/Primitives.h>

extern "C"
{
#include <lua.h>
#include <lsqlite3.h>
}

#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>

using TiltedPhoques::AutoPtr;
using TiltedPhoques::Set;
using TiltedPhoques::List;
using TiltedPhoques::Map;
using TiltedPhoques::Vector;
using TiltedPhoques::String;
using TiltedPhoques::UniquePtr;

using TiltedPhoques::Vector3;

using TiltedPhoques::Allocator;
using TiltedPhoques::ScratchAllocator;
using TiltedPhoques::ScopedAllocator;
using TiltedPhoques::StackAllocator;
using TiltedPhoques::ThisCall;
using TiltedPhoques::Buffer;
using TiltedPhoques::Outcome;
using TiltedPhoques::App;

using namespace std::chrono_literals;

#include <RTTI.h>

