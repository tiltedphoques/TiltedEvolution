#include <NetImmerse/NiCamera.h>
#include <TiltedOnlinePCH.h>

using TWorldPtToScreenPt3 = bool(float*, const NiRect<float>*, const NiPoint3*, float*, float*, float*, float);
static TWorldPtToScreenPt3* s_WorldPtToScreenPt3;

bool NiCamera::WorldPtToScreenPt3(const NiPoint3& in, NiPoint3& out, float tolerance)
{
    TP_THIS_FUNCTION(TW2S, bool, NiCamera, const NiPoint3*, float*, float*, float*, float);
    POINTER_SKYRIMSE(TW2S, s_w2s, 70639);

    return ThisCall(s_w2s, this, &in, &out.x, &out.y, &out.y, tolerance);
}

bool NiCamera::WorldPtToScreenPt3(float* matrix, const NiRect<float>* port, const NiPoint3* p_in, float* x_out, float* y_out,
                                  float* z_out, float zeroTolerance)
{
    return s_WorldPtToScreenPt3(matrix, port, p_in, x_out, y_out, z_out, zeroTolerance);
}

static TiltedPhoques::Initializer s_Init([]() {
    //POINTER_FALLOUT4(TWorldPtToScreenPt3, s_w2s, 109442);
    POINTER_SKYRIMSE(TWorldPtToScreenPt3, s_w2s, 70640);
    s_WorldPtToScreenPt3 = s_w2s.Get();
});
