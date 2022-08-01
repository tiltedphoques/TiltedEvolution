
#include <Games/Skyrim/Interface/Menus/HUDMenuUtils.h>
#include <Games/Skyrim/NetImmerse/NiCamera.h>

namespace HUDMenuUtils
{
static float (*CameraWorldToCam)[4][4] = nullptr;
static const NiRect<float>* CameraPort = nullptr;

bool WorldPtToScreenPt3(const NiPoint3& aWorldPt, NiPoint3& aScreenPt)
{
    return NiCamera::WorldPtToScreenPt3(reinterpret_cast<float*>(CameraWorldToCam), CameraPort, &aWorldPt, &aScreenPt.x,
                                        &aScreenPt.y, &aScreenPt.z, 1e-5f);
}
} // namespace HUDMenuUtils

static TiltedPhoques::Initializer s_Init([]() {
    POINTER_FALLOUT4(float[4][4], s_matrix, 698830);
    POINTER_FALLOUT4(NiRect<float>, s_port, 1482949);

    POINTER_SKYRIMSE(float[4][4], s_matrix, 406126);
    POINTER_SKYRIMSE(NiRect<float>, s_port, 406160);

    HUDMenuUtils::CameraWorldToCam = s_matrix.Get();
    HUDMenuUtils::CameraPort = s_port.Get();
});
