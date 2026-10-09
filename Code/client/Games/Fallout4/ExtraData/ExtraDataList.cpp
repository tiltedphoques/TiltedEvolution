#include <TiltedOnlinePCH.h>

#include <ExtraData/ExtraDataList.h>
#include <Games/Memory.h>

ExtraDataList* ExtraDataList::New() noexcept
{
    auto* pList = Memory::Allocate<ExtraDataList>();
    if (pList)
    {
        using TCtor = void(ExtraDataList*);
        static VersionDbPtr<TCtor> ctor(2190088);
        ctor.Get()(pList);
    }
    return pList;
}

float ExtraDataList::GetHealthPercent() const noexcept
{
    using TGetHealth = float(const ExtraDataList*);
    static VersionDbPtr<TGetHealth> getHealth(2190226);
    return getHealth.Get()(this);
}

void ExtraDataList::SetHealthPercent(float aHealth) noexcept
{
    using TSetHealth = void(ExtraDataList*, float);
    static VersionDbPtr<TSetHealth> setHealth(2190124);
    setHealth.Get()(this, aHealth);
}
