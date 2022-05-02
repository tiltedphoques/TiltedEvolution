#pragma once

struct BSThread
{
  public:
    virtual ~BSThread() = 0;
    // runner
    virtual uint32_t ThreadProc() = 0;
    // probably release
    virtual bool Release() = 0;

  //private:
    CRITICAL_SECTION lock; // in reality a BSCriticalSection
    void* m_ThreadHandle;
    void* m_ParentHandle;
    uint32_t m_ThreadID;
    uint32_t m_ParentID;
    bool bThreadIsActive;
};
static_assert(sizeof(BSThread) == 0x50, "BSThread size mismatch");
