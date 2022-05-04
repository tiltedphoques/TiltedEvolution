#pragma once

namespace usvfs
{
// if our process was force fed usvfs
bool IsUSVFSActive();

class USVFSSentinel
{
  public:
    USVFSSentinel();

    bool IsActive()
    {
        return m_usvfsHandle;
    }

  private:
    HMODULE m_usvfsHandle;
};
} // namespace usvfs
