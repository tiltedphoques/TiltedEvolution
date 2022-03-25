#pragma once

namespace Scaleform::Render
{
template <typename T> struct Size
{
    T Width;
    T Height;
};

using SizeF = Size<float>;
using SizeD = Size<double>;

template <class T> struct RectData
{
    T x1, y1, x2, y2;
};

template <typename T> struct Rect : RectData<T>
{
    inline Rect()
    {
        SetRect(0, 0, 0, 0);
    }

    inline explicit Rect(T val)
    {
        SetRect(val, val);
    }

    inline Rect(const Rect<T>& rc)
    {
        SetRect(rc);
    }

    inline Rect(const Size<T>& sz)
    {
        SetRect(sz);
    }

    inline Rect(T w, T h)
    {
        SetRect(w, h);
    }

    inline Rect(T left, T top, T right, T bottom)
    {
        SetRect(left, top, right, bottom);
    }

    inline void SetRect(T l, T t, T r, T b)
    {
        this->x1 = l;
        this->y1 = t;
        this->x2 = r;
        this->y2 = b;
    }
};

using RectF = Rect<float>;
using RectD = Rect<double>;
} // namespace Scaleform::Render
