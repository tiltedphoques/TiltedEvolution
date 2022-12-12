// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#ifndef BUILDING_TT_SERVER
#ifdef _WIN32
#define PLUGIN_API extern "C" __declspec(dllexport)
#endif

#ifdef __linux__
#define PLUGIN_API extern "C" __attribute__((visibility("default")))
#endif
#endif

#ifdef BUILDING_TT_SERVER

#ifdef _WIN32
#define SERVER_API extern "C" __declspec(dllexport)
#define SERVER_API_CXX __declspec(dllexport)
#endif

#ifdef __linux__
#define SERVER_API extern "C" __attribute__((visibility("default")))
#define SERVER_API_CXX __attribute__((visibility("default")))
#endif

#else

#ifdef _WIN32
#define SERVER_API extern "C" __declspec(dllimport)
#define SERVER_API_CXX __declspec(dllimport)
#endif

#ifdef __linux__
#define SERVER_API extern "C" __attribute__((visibility("default")))
#define SERVER_API_CXX __attribute__((visibility("default")))
#endif

#endif
