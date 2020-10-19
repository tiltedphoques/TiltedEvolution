#pragma once

struct CharacterContext;

struct hkbCharacter
{
    virtual ~hkbCharacter() {};

    uint8_t pad8[0xA0 - 0x8];
    CharacterContext* characterContext;
    uint8_t pad88[0xA0 - 0x88];
};
