#include <Forms/TESObjectCELL.h>
#include <TESObjectREFR.h>

void TESObjectCELL::GetRefsByFormTypes(Vector<TESObjectREFR*>& aRefs, const Vector<FormType>& aFormTypes) noexcept
{
    if (!refData.refArray)
        return;

    for (uint32_t i = 0; i < refData.capacity; i++)
    {
        auto ref = refData.refArray[i].Get();
        if (!ref)
            continue;

        for (auto formType : aFormTypes)
        {
            if (ref->baseForm->formType == formType)
                aRefs.push_back(ref);
        }
    }
}
