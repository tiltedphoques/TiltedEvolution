#pragma once

struct IVirtualMachine;

struct GameVM
{
    static GameVM* Get()
    {
        return (GameVM*)0x1456ED318; // 878371, address from 23
    }

    void Update(float aUpdateBudget);

    virtual void unk0();
    // ...

    uint8_t unk8[0xD8 - 8];
    IVirtualMachine* pVM;
    // ...
};

static_assert(offsetof(GameVM, pVM) == 0xD8);
