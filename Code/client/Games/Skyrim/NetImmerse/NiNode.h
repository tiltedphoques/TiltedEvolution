#pragma once

#include <NetImmerse/NiAVObject.h>
#include <NetImmerse/NiTObjectArray.h>

struct NiNode : NiAVObject
{
    virtual ~NiNode();

      // in reality:
    // NiTObjectArray<NiPointer<NiAVObject>> children;
    //  but we are lazy:
    NiTObjectArray<NiAVObject*> children;
};
