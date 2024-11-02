#include <TiltedOnlinePCH.h>

#include <Games/IFormFactory.h>

IFormFactory* IFormFactory::GetForType(const FormType aId) noexcept
{
    if (aId >= FormType::Count)
        return nullptr;

    POINTER_SKYRIMSE(IFormFactory*, s_factories, 400508);

    return s_factories.Get()[(uint32_t)aId];
}
