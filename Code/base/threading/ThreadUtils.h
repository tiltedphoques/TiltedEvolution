#pragma once

namespace Base
{
// set to thread handle
bool SetThreadName(void* apThreadHandle, const char* apThreadName);

// call this on current thread
bool SetCurrentThreadName(const char* apThreadName);
} // namespace Base
