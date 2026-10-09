#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

INISettingCollection* INISettingCollection::Get() noexcept
{
    POINTER_GAME(INISettingCollection*, settingCollection, 411155, 2704108);

    return *settingCollection.Get();
}

Setting* INISettingCollection::GetSetting(const char* acpName) noexcept
{
    Entry* pCurrent = &head;

    while (pCurrent)
    {
        if (pCurrent->setting && pCurrent->setting->name && _stricmp(acpName, pCurrent->setting->name) == 0)
            return pCurrent->setting;

        pCurrent = pCurrent->next;
    }

    return nullptr;
}
