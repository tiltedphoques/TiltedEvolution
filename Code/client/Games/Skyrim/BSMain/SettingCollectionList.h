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

    virtual void Add(T*) override
    {
    }

    virtual void Remove(T*) override
    {
    }
    virtual bool WriteSetting(T*) = 0;
    virtual bool ReadSetting(T*) = 0;
    virtual bool Open(bool) override
    {
        return false;
    }

    virtual bool Close() override
    {
        return false;
    }

    virtual bool ReadSettingsFromProfile() = 0;

    virtual bool WriteSettings() override
    {
        return false;
    }

    virtual bool ReadSettings() override
    {
        return false;
    }

    TList& GetList() const
    {
        return SettingsA;
    }

  private:
    TList SettingsA;
};
static_assert(sizeof(SettingCollectionList<void*>) == 0x128);

} // namespace BSMain
