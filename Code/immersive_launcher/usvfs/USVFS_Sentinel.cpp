
#include <Windows.h>
#include "usvfs/USVFS_Sentinel.h"

namespace usvfs
{
bool IsUSVFSActive()
{
    return GetModuleHandleW(L"usvfs_x64.dll");
}

USVFSSentinel::USVFSSentinel()
{
    m_usvfsHandle = GetModuleHandleW(L"usvfs_x64.dll");

    if (m_usvfsHandle)
    {
    
    }
}

} // namespace usvfs
