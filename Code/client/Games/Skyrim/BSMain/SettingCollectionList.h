#pragma once

#include "BSMain/SettingCollection.h"
#include <Games/Primitives.h>

namespace BSMain
{
template <class T> class SettingCollectionList : public SettingCollection<T>
{
  public:
    virtual ~SettingCollectionList() = default;

    using TList = GameList<T*>;

    TList& GetList() const
    {
        return SettingsA;
    }

  private:
    TList SettingsA;
};
static_assert(sizeof(SettingCollectionList<void*>) == 0x128);

} // namespace BSMain
