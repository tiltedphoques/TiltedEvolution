#pragma once

namespace Scaleform
{
class String
{
  public:
    // just to make sure..
    struct DataDesc
    {
        size_t Size;
        volatile int RefCount;
        char Data[1];
    };

    enum HeapType
    {
        HT_Global = 0,
        HT_Local = 1,
        HT_Dynamic = 2,
        HT_Mask = 3
    };

    union {
        DataDesc* pData;
        size_t HeapTypeBits;
    };

    using DataDescUnion = union {
        DataDesc* pData;
        size_t HeapTypeBits;
    };
};

// Temporary hack.
struct StringLH : Scaleform::String
{
};
} // namespace Scaleform
