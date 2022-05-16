
#include "ThreadUtils.h"

namespace
{
// as defined in sentry-native/src/sentry_sync.h
#ifdef _WIN32
#include <Windows.h>
using sentry_threadid_t = HANDLE;

sentry_threadid_t GetCurrentThreadHandle()
{
    return ::GetCurrentThread();
}
#else
#include <pthread.h>
using sentry_threadid_t = pthread_t;

sentry_threadid_t GetCurrentThreadHandle()
{
    return ::pthread_self();
}
#endif

} // namespace

// we use the sentry impl here, as it covers all bases, e.g linux & windows support, plus additional windows 10+
// features
extern "C"
{
    int sentry__thread_setname(sentry_threadid_t aThreadHandle, const char* apThreadName);
}

namespace Base
{
bool SetThreadName(void* apThreadHandle, const char* apThreadName)
{
    return sentry__thread_setname(reinterpret_cast<sentry_threadid_t>(apThreadHandle), apThreadName) == 0;
}

bool SetCurrentThreadName(const char* apThreadName)
{
    return sentry__thread_setname(GetCurrentThreadHandle(), apThreadName) == 0;
}

} // namespace Base
