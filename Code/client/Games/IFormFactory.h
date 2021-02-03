#pragma once

struct TESForm;
struct IFormFactory
{
    virtual ~IFormFactory();
    virtual TESForm* Create();
    virtual void sub_2();
    virtual void sub_3();
    virtual void sub_4();
    virtual void sub_5();
    virtual void sub_6();

    static IFormFactory* GetForType(uint32_t aId) noexcept;

    template<class T>
    static T* Create()
    {
        return static_cast<T*>(GetForType(T::Type)->Create());
    }
};

