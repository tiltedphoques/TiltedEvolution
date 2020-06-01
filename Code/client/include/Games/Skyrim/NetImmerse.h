#pragma once

#if TP_SKYRIM

#include <Games/Primitives.h>

struct BSFixedString;

template <class T>
struct NiPointer
{
    NiPointer<T>& operator=(const NiPointer& acRhs)
    {
        if (object != acRhs.object)
        {
            if (acRhs.object) acRhs.object->IncRef();
            if (object) object->DecRef();

            object = acRhs.object;
        }

        return *this;
    }

    NiPointer<T>& operator=(T* aRhs)
    {
        if (object != aRhs)
        {
            if (aRhs) aRhs->IncRef();
            if (object) object->DecRef();

            object = aRhs;
        }

        return *this;
    }

    T* object;
};


struct NiRTTI
{

};

struct NiTriBasedGeom;

struct NiObject : NiRefObject
{
    virtual ~NiObject();

    virtual NiRTTI* GetRTTI();

    virtual void sub_3();
    virtual void sub_4();
    virtual void sub_5();
    virtual void sub_6();
#if TP_PLATFORM_64
    virtual void sub_7bis();
#endif
    virtual void sub_7();
    virtual NiTriBasedGeom* CastToNiTriBasedGeom();
    virtual void sub_9();
    virtual void sub_A();
    virtual void sub_B();
    virtual void sub_C();
    virtual void sub_D();
    virtual void sub_E();
    virtual void sub_F();
    virtual void sub_10();
    virtual void sub_11();
    virtual void sub_12();
    virtual void sub_13();
    virtual void sub_14();
    virtual void sub_15();
    virtual void sub_16();
    virtual void sub_17();
    virtual void sub_18();
    virtual void sub_19();
    virtual void sub_1A();
    virtual void sub_1B();
    virtual void sub_1C();
    virtual void sub_1D();
    virtual void sub_1E();
    virtual void sub_1F();
    virtual void sub_20();
    virtual void sub_21();
#if TP_PLATFORM_64
    virtual void sub_64_23();
    virtual void sub_64_24();
    virtual void sub_64_25();
#endif
};

struct NiObjectNET : NiObject
{
    virtual ~NiObjectNET();
};

struct NiProperty : NiObjectNET
{
    virtual ~NiProperty();
};

struct NiAVObject : NiObjectNET
{
    virtual ~NiAVObject();

    virtual void sub_22();
    virtual void sub_23();
    virtual void sub_24();
    virtual void sub_25();
#if TP_PLATFORM_32
    virtual void sub_26();
#endif

    virtual NiAVObject* GetByName(BSFixedString& aName);
};

struct NiGeometry : NiAVObject
{
    virtual ~NiGeometry();

    NiPointer<NiProperty> unkProperty1;

#ifdef _WIN64
    NiPointer<NiProperty> unkProperty2;
    uintptr_t unkB0;
#endif

    NiPointer<NiProperty> effect;
    uintptr_t unkB8;
};

struct NiTriBasedGeom : NiGeometry
{
    virtual ~NiTriBasedGeom();
};

struct NiNode : NiAVObject
{
    virtual ~NiNode();
};

struct BSFaceGenNiNode : NiNode
{
    virtual ~BSFaceGenNiNode();
};


#endif
