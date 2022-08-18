#pragma once

namespace BSMain
{
class Setting
{
  public:
    enum SETTING_TYPE : int
    {
        ST_BINARY = 0x0,
        ST_CHAR = 0x1,
        ST_UCHAR = 0x2,
        ST_INT = 0x3,
        ST_UINT = 0x4,
        ST_FLOAT = 0x5,
        ST_STRING = 0x6,
        ST_RGB = 0x7,
        ST_RGBA = 0x8,
        ST_NONE = 0x9,
    };

    virtual ~Setting() = default;

    // Is it a member of the user customizeable collection?
    virtual bool IsPrefSetting()
    {
        return false;
    }

    static SETTING_TYPE DataType(const char* apKey);

    const char* GetName() const
    {
        return pKey;
    }

    // To data type
    int asInt() const
    {
        return value.i;
    }

    float AsFloat() const
    {
        return value.f;
    }

    bool asBool() const
    {
        return value.b;
    }

  public:
    union {
        float f;
        int i;
        uint32_t u;
        bool b;
        char c;
        const char* str;
    } value;
    const char* pKey;
};

static_assert(sizeof(Setting) == 0x18);

// Wrapper class so we can register our own settings.
class TiltedSetting final : public Setting
{
  private:
    TiltedSetting(TiltedSetting*& apParent) noexcept : m_pNext(apParent)
    {
        apParent = this;
    }

  public:
    TiltedSetting(const char* apID, int aiValue) noexcept : TiltedSetting(GetRoot())
    {
        Setting::pKey = apID;
        Setting::value.i = aiValue;
    }

    TiltedSetting(const char* apID, float afValue) noexcept : TiltedSetting(GetRoot())
    {
        Setting::pKey = apID;
        Setting::value.f = afValue;
    }

    TiltedSetting(const char* apID, bool abValue) noexcept : TiltedSetting(GetRoot())
    {
        Setting::pKey = apID;
        Setting::value.b = abValue;
    }

    TiltedSetting(const char* apID, const char* apString) noexcept : TiltedSetting(GetRoot())
    {
        Setting::pKey = apID;
        Setting::value.str = apString;
    }

    // TODO: add more constructors..

    inline static TiltedSetting*& GetRoot() noexcept
    {
        static TiltedSetting* s_root{nullptr};
        return s_root;
    }

    template <typename TFunctor>
    inline static size_t VisitAll(const TFunctor aFunctor, TiltedSetting*& apStart = GetRoot(), bool aClear = true)
    {
        size_t total = 0;

        auto* i = apStart;

        if (aClear)
            apStart = nullptr;

        while (i)
        {
            aFunctor(i);
            ++total;

            auto* j = i->m_pNext;

            if (aClear)
                i->m_pNext = nullptr;

            i = j;
        }

        return total;
    }

  private:
    TiltedSetting* m_pNext{nullptr};
};
} // namespace BSMain
