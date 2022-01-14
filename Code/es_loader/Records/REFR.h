#pragma once

#include "Record.h"

class REFR : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::REFR;

    struct Data
    {
        uint32_t m_baseId;
    };

    Data ParseChunks(Map<Record*, SharedPtr<Buffer>>& aCompressedChunkCache) noexcept;
};

