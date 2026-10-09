#pragma once

template <class T> struct NiPointer
{
    NiPointer<T>& operator=(const NiPointer& acRhs)
    {
        if (object != acRhs.object)
        {
            if (acRhs.object)
                acRhs.object->IncRef();
            if (object)
                object->DecRef();

            object = acRhs.object;
        }

        return *this;
    }

    NiPointer<T>& operator=(T* aRhs)
    {
        if (object != aRhs)
        {
            if (aRhs)
                aRhs->IncRef();
            if (object)
                object->DecRef();

            object = aRhs;
        }

        return *this;
    }

    [[nodiscard]] explicit constexpr operator bool() const noexcept
    {
        return static_cast<bool>(object);
    }

    T* object;
};
