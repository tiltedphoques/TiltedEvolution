#pragma once

#include "Render_Types2D.h"

namespace Scaleform::Render
{
template <typename T> T TwipsToPixels(T x)
{
    return T((x) / T(20.));
}
template <typename T> T PixelsToTwips(T x)
{
    return T((x)*T(20.));
}

// Specializations to use mult instead of div op
template <> inline float TwipsToPixels(float x)
{
    return x * 0.05f;
}
template <> inline double TwipsToPixels(double x)
{
    return x * 0.05;
}

inline RectF TwipsToPixels(const RectF& x)
{
    return RectF(TwipsToPixels(x.x1), TwipsToPixels(x.y1), TwipsToPixels(x.x2), TwipsToPixels(x.y2));
}

}
