#include <TiltedOnlinePCH.h>

#include <Components/TESSpellList.h>
#include <Games/Memory.h>

void TESSpellList::Initialize()
{
    if (lists != nullptr)
        return;

    lists = Memory::New<Lists>();
}

namespace
{
template <class T> uint32_t Compact(T** apEntries, uint32_t& aCount) noexcept
{
    if (!apEntries)
        return 0;

    uint32_t kept = 0;
    for (uint32_t i = 0; i < aCount; ++i)
    {
        if (apEntries[i])
            apEntries[kept++] = apEntries[i];
    }

    const uint32_t removed = aCount - kept;
    aCount = kept;
    return removed;
}
} // namespace

uint32_t TESSpellList::RemoveNullEntries() noexcept
{
    if (!lists)
        return 0;

    // Compacting in place is safe: the arrays keep their allocation, and the next load reallocates when the count differs
    return Compact(lists->spells, lists->spellCount) + Compact(lists->unk4, lists->unk4Count) + Compact(lists->shouts, lists->shoultCount);
}
