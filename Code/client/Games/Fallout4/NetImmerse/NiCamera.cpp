#include <TiltedOnlinePCH.h>
#include <NetImmerse/NiCamera.h>

using TWorldPtToScreenPt3 = bool(float*, const NiRect<float>*, const NiPoint3*, float*, float*, float*, float);

bool NiCamera::WorldPtToScreenPt3(const NiPoint3& in, NiPoint3& out, float tolerance)
{
    return WorldPtToScreenPt3(&worldToCam[0][0], &port, &in, &out.x, &out.y, &out.z, tolerance);
}

bool NiCamera::WorldPtToScreenPt3(float* matrix, const NiRect<float>* port, const NiPoint3* p_in, float* x_out, float* y_out, float* z_out, float zeroTolerance)
{
    static VersionDbPtr<TWorldPtToScreenPt3> s_w2s(2270344);
    return s_w2s.Get()(matrix, port, p_in, x_out, y_out, z_out, zeroTolerance);
}
