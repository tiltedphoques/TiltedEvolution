#pragma once

#include "Record.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/CONT
class CONT : Record
{
public:
    static constexpr FormEnum kType = FormEnum::CONT;

    struct Object
    {
        // CNTO
        uint32_t m_formId = 0;
        uint32_t m_count = 0;
    };

    struct Data
    {
        // EDID
        const char* m_editorId = "";
        // FULL
        const char* m_name = "";
        // Objects
        Vector<Object> m_objects;
    };

    Data ParseChunks(Map<Record*, SharedPtr<Buffer>>& aCompressedChunkCache) noexcept;
};
