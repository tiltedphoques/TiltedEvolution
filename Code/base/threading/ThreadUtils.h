#pragma once

namespace base
{
// set to thread handle
bool SetThreadName(void* apThreadHandle, const char* apThreadName);

// call this on current thread
bool SetCurrentThreadName(const char* apThreadName);
} // namespace base
