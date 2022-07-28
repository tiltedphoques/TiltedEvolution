#include <Forms/TESObjectCELL.h>
#include <TESObjectREFR.h>

Vector<TESObjectREFR*> TESObjectCELL::GetRefsByFormTypes(const Vector<FormType>& aFormTypes) noexcept
{
    Vector<TESObjectREFR*> references{};

    if (!objects.data)
        return references;

    for (TESObjectREFR* pObject : objects)
    {
        if (!pObject)
            continue;

        for (FormType formType : aFormTypes)
        {
            if (pObject->baseForm->formType == formType)
                references.push_back(pObject);
        }
    }

    return references;
}
