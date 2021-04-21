#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <TiltedCore/Platform.hpp>

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

// TiltedCore
#include <TiltedCore/StackAllocator.hpp>
#include <TiltedCore/ScratchAllocator.hpp>
#include <TiltedCore/Filesystem.hpp>
#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Outcome.hpp>
#include <TiltedCore/ViewBuffer.hpp>
#include <TiltedCore/Math.hpp>
#include <TiltedCore/TaskQueue.hpp>
#include <TiltedCore/Buffer.hpp>
#include <TiltedCore/Initializer.hpp>

// TiltedReverse
#include <AutoPtr.hpp>
#include <App.hpp>
#include <FunctionHook.hpp>
#include <Entry.hpp>
#include <Debug.hpp>
#include <ThisCall.hpp>

#define SPDLOG_WCHAR_FILENAMES

#include <entt/entt.hpp>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <any>
#include <mutex>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <Utils.h>
#include <BuildInfo.h>
#include <Games/Primitives.h>


extern "C"
{
#include <lua.h>
#include <lsqlite3.h>
}

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

using TiltedPhoques::AutoPtr;
using TiltedPhoques::Set;
using TiltedPhoques::List;
using TiltedPhoques::Map;
using TiltedPhoques::Vector;
using TiltedPhoques::String;
using TiltedPhoques::UniquePtr;
using TiltedPhoques::Allocator;
using TiltedPhoques::ScratchAllocator;
using TiltedPhoques::ScopedAllocator;
using TiltedPhoques::StackAllocator;
using TiltedPhoques::ThisCall;
using TiltedPhoques::Buffer;
using TiltedPhoques::Outcome;
using TiltedPhoques::App;

using namespace std::chrono_literals;

#include "Components.h"

#include <RTTI.h>

