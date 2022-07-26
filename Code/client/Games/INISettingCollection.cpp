#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

INISettingCollection* INISettingCollection::Get() noexcept
{
    POINTER_FALLOUT4(INISettingCollection*, settingCollection, 791184);
    POINTER_SKYRIMSE(INISettingCollection*, settingCollection, 411155);

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
