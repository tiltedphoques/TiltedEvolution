#include "FormExtensions.h"

ActorExtension* Actor::GetExtension() noexcept
{
    if (AsExActor())
    {
        return static_cast<ActorExtension*>(AsExActor());
    }

    if (AsExPlayerCharacter())
    {
        return static_cast<ActorExtension*>(AsExPlayerCharacter());
    }

    return nullptr;
}

ExActor* Actor::AsExActor() noexcept
{
    if (cFormType == Type && formID != 0x14)
        return static_cast<ExActor*>(this);

    return nullptr;
}

ExPlayerCharacter* Actor::AsExPlayerCharacter() noexcept
{
    if (formID == 0x14)
        return static_cast<ExPlayerCharacter*>(this);

    return nullptr;
}
