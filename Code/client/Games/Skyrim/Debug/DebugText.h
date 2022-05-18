#pragma once

struct NiColorA
{
    float red;   // 00
    float green; // 04
    float blue;  // 08
    float alpha; // 0C
};

class IDebugText
{
  public:
    enum DEBUGTEXT_LOCATION
    {
        DTL_LEFT = 0x0,
        DTL_RIGHT = 0x1,
        DTL_CENTER = 0x2,
        DTL_COUNT = 0x3,
    };

    enum DEBUGTEXT_JUSTIFY
    {
        DTJ_LEFT = 0x1,
        DTJ_CENTER = 0x2,
        DTJ_RIGHT = 0x3,
        DTJ_COUNT = 0x4,
    };

    virtual ~IDebugText() = 0;
    virtual void Reset() = 0;
    virtual void SkipLine(DEBUGTEXT_LOCATION, int) = 0;
    virtual int PrintX(int, DEBUGTEXT_LOCATION, DEBUGTEXT_JUSTIFY, const char*, ...) = 0;
    virtual int PrintX(int, const char*, ...) = 0;
    virtual int VAPrintX(int aiX, DEBUGTEXT_LOCATION, DEBUGTEXT_JUSTIFY, const char*, char*, const NiColorA*) = 0;
    virtual int PrintLine(DEBUGTEXT_LOCATION, DEBUGTEXT_JUSTIFY, const char*, ...) = 0;
    virtual int VAPrintLine(DEBUGTEXT_LOCATION, DEBUGTEXT_JUSTIFY, const char*, char*, const NiColorA*) = 0;
    virtual int PrintXC(int, const NiColorA*, DEBUGTEXT_LOCATION, DEBUGTEXT_JUSTIFY, const char*, ...) = 0;
    virtual int PrintXC(int, const NiColorA*, const char*, ...) = 0;
    virtual int PrintLineC(const NiColorA*, DEBUGTEXT_LOCATION, DEBUGTEXT_JUSTIFY, const char*, ...) = 0;
    virtual int GetCurrentLine(DEBUGTEXT_LOCATION) = 0;
    virtual void SetCurrentLine(DEBUGTEXT_LOCATION, int) = 0;
};

class DebugText : public IDebugText
{
  public:
    virtual ~DebugText() = default;

    static DebugText* Instance(bool abCreate);

    // clear the entire screen of any debug text
    void DeleteAllDebugText();

    void Print(const char* apText, float afX, float afY, int aiJustify, int aiLine_num, float afDie, int aiFont,
               const NiColorA* apColor);

  private:
    char pad0[0x60 - 4 - 12 - 12 - 8];
    int iFontHeight;
    int iHeight[3];
    int iXLocation[3];
};

static_assert(sizeof(DebugText) == 0x60);
