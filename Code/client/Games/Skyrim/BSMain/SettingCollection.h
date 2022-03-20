#pragma once

namespace BSMain
{
template <class T> class SettingCollection
{
  public:
    virtual ~SettingCollection() = default;

    virtual void Add(T*) = 0;
    virtual void Remove(T*) = 0;
    virtual bool WriteSetting(T*) = 0;
    virtual bool ReadSetting(T*) = 0;
    virtual bool Open(bool) = 0;
    virtual bool Close() = 0;
    virtual bool ReadSettingsFromProfile() = 0;
    virtual bool WriteSettings() = 0;
    virtual bool ReadSettings() = 0;

  private:
    char pSettingFile[260];
    void* pHandle;
};
static_assert(sizeof(SettingCollection<void*>) == 0x118);

} // namespace BSMain
