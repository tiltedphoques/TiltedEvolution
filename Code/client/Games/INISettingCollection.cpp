#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

INISettingCollection* INISettingCollection::Get() noexcept
{
    POINTER_FALLOUT4(INISettingCollection*, settingCollection, 0x145EDB528 - 0x140000000);
    POINTER_SKYRIMSE(INISettingCollection*, settingCollection, 0x1430B82D8 - 0x140000000);

    return *settingCollection.Get();
}

Setting* INISettingCollection::GetSetting(const char* acpName) noexcept
{
    Entry* pCurrent = &head;

    while (pCurrent)
    {
        if (_stricmp(acpName, pCurrent->setting->name) == 0)
            return pCurrent->setting;

        pCurrent = pCurrent->next;
    }

    return nullptr;
}
