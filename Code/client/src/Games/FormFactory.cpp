#include <Games/IFormFactory.h>

#include <Games/Fallout4/Forms/TESForm.h>
#include <Games/Skyrim/Forms/TESForm.h>

IFormFactory* IFormFactory::GetForType(const uint32_t aId) noexcept
{
    if (aId >= Count)
        return nullptr;

    POINTER_FALLOUT4(IFormFactory*, s_factories, 0x1458D3BF0 - 0x140000000);
    POINTER_SKYRIMSE(IFormFactory*, s_factories, 0x141EC3CE0 - 0x140000000);

    return s_factories.Get()[aId];
}

