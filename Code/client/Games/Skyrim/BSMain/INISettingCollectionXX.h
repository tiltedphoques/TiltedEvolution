#pragma once

#include "Setting.h"
#include "SettingCollectionList.h"

namespace BSMain
{
class INISettingCollectionXX : public SettingCollectionList<BSMain::Setting>
{

};
static_assert(sizeof(INISettingCollectionXX) == 0x128);
}
