#pragma once

#include "Setting.h"
#include "SettingCollectionMap.h"

namespace BSMain
{
class GameSettingCollection final : public SettingCollectionMap<BSMain::Setting>
{
  public:

};
static_assert(sizeof(GameSettingCollection) == 0x140);
}
