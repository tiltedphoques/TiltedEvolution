#pragma once

struct MenuControls
{
    static MenuControls* GetInstance();

    void SetToggle(bool);

private:
    char pad_0000[128]; // 0x0000
    bool isProcessing;  // 0x0080
    bool isBeastForm;   // 0x0081
    bool isRemapMode;   // 0x0082
    bool canBeOpened;   // 0x0083
    char pad_0084[12];  // 0x0084
};                      // Size: 0x0090
static_assert(sizeof(MenuControls) == 0x90);
