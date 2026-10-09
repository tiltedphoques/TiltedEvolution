#include <TiltedOnlinePCH.h>

#include <NetImmerse/ImageSpaceModifierInstance.h>

void ImageSpaceModifierInstance::Stop(ImageSpaceModifierInstance* apModifier)
{
    using TStop = void(ImageSpaceModifierInstance*);
    static VersionDbPtr<TStop> stop(2199897);
    stop.Get()(apModifier);
}
