#include <TiltedOnlinePCH.h>

#include <Interface/Menus/HUDMenuUtils.h>

void HUDMenuUtils::WorldPtToScreenPt3(const NiPoint3& aWorldPt, NiPoint3& aScreenPt)
{
    using TProject = void(const NiPoint3&, NiPoint3&);
    static VersionDbPtr<TProject> project(2222464);
    project.Get()(aWorldPt, aScreenPt);
}
