#pragma once

#if defined(_WIN32)
#include <windows.h>
using ThreadHandle = HANDLE;
#else
#include <pthread.h>
using ThreadHandle = pthread_t;
#endif

namespace Base
{
// set to thread handle
bool SetThreadName(const ThreadHandle& acHandle, const char* apThreadName);

// call this on current thread
bool SetCurrentThreadName(const char* apThreadName);
} // namespace Base
