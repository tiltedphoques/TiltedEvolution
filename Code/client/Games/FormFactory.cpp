#include <TiltedOnlinePCH.h>

#include <Games/IFormFactory.h>

IFormFactory* IFormFactory::GetForType(const FormType aId) noexcept
{
    if (aId >= FormType::Count)
        return nullptr;

    POINTER_FALLOUT4(IFormFactory*, s_factories, 0x1458D3BF0 - 0x140000000);
    POINTER_SKYRIMSE(IFormFactory*, s_factories, 0x141F5E4A0 - 0x140000000);

    return s_factories.Get()[(uint32_t)aId];
}

