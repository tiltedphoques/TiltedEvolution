#pragma once

struct GFxValue
{
    void* ObjectInterface_0;
    int32 Type_8;
    char _pad_C[4];
    uint64 Value_10;
};

class HUDObject
{
  public:
    virtual ~HUDObject() = default;

    virtual void Update() = 0;
    virtual int ProcessMessage(void* param) = 0;
    virtual void RegisterHUDComponents(void* param) = 0;
    virtual void CleanUp() = 0;

  private:
    void* movieView;
    GFxValue value;
};

static_assert(sizeof(HUDObject) == 0x28);
