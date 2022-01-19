#pragma once

#include "Record.h"
#include "Chunks.h"

class REFR : public Record
{
public:
    static constexpr FormEnum kType = FormEnum::REFR;

    Chunks::NAME m_basicObject{};

    void ParseChunks() noexcept;
};

