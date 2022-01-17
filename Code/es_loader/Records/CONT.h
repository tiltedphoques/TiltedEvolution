#pragma once

#include "Record.h"
#include "Chunks.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/CONT
class CONT : Record
{
public:
    static constexpr FormEnum kType = FormEnum::CONT;

    struct Data
    {
        // EDID
        String m_editorId = "";
        // FULL
        String m_name = "";
        // Objects
        Vector<Chunks::CNTO> m_objects;
    };

    Data ParseChunks() noexcept;
};
