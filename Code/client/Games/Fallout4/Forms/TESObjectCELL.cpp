#include <Forms/TESObjectCELL.h>
#include <TESObjectREFR.h>

void TESObjectCELL::GetRefsByFormTypes(Vector<TESObjectREFR*>& aRefs, const Vector<FormType>& aFormTypes) noexcept
{
    if (!objects.data)
        return;

    for (auto object : objects)
    {
        if (!object)
            continue;

        for (auto formType : aFormTypes)
        {
            if (object->baseForm->formType == formType)
                aRefs.push_back(object);
        }
    }
}
