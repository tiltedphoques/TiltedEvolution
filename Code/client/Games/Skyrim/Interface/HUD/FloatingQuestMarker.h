#pragma once

#include <Interface/HUD/HUDObject.h>

class FloatingQuestMarker : public HUDObject
{
  public:
    virtual ~FloatingQuestMarker() = 0;

    // in FloatingQuestMarker::Update

  private:
    char pad_28[144];
};

static_assert(sizeof(FloatingQuestMarker) == 0xB8);
