#pragma once

#include "Record.h"
#include "Chunks.h"

class REFR : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::REFR;

    struct Data
    {
        Data() = default;
        Chunks::NAME m_basicObject;
    };

    Data ParseChunks() noexcept;
};

