#pragma once

#include "SettingCollection.h"

namespace BSMain
{
template <class T> 
class SettingCollectionMap : public SettingCollection<T>
{
  public:
    void Add(T*) override {}

  private:
    char pad118[0x140 - 0x118];
};
static_assert(sizeof(SettingCollectionMap<Setting>) == 0x140);
} // namespace BSMain
