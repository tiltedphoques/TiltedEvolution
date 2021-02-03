#pragma once

#if defined(TP_SKYRIM) && TP_PLATFORM_64
#define POINTER_SKYRIMSE(className, variableName, ...) static AutoPtr<className> variableName(__VA_ARGS__)
#else
#define POINTER_SKYRIMSE(className, variableName, ...) ;
#endif

#if defined(TP_SKYRIM) && TP_PLATFORM_32
#define POINTER_TESV(className, variableName, ...) static AutoPtr<className> variableName(__VA_ARGS__)
#else
#define POINTER_TESV(className, variableName, ...) ;
#endif

#if defined(TP_FALLOUT) && TP_PLATFORM_64
#define POINTER_FALLOUT4(className, variableName, ...) static AutoPtr<className> variableName(__VA_ARGS__)
#else
#define POINTER_FALLOUT4(className, variableName, ...) ;
#endif

