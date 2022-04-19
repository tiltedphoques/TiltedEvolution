#pragma once

#include <Forms/TESForm.h>

struct IFormFactory
{
    virtual ~IFormFactory();
    virtual TESForm* Create();
    virtual void sub_2();
    virtual void sub_3();
    virtual void sub_4();
    virtual void sub_5();
    virtual void sub_6();

    static IFormFactory* GetForType(const FormType aId) noexcept;

    template<class T>
    static T* Create()
    {
        return static_cast<T*>(GetForType(T::Type)->Create());
    }
};

