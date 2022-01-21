#pragma once

class Group
{
    Group() = delete;

private:
    char m_recordType[4];
    uint32_t m_groupSize;
    char m_label[4];
    int32_t m_groupType;
    uint16_t m_formVersion;
    uint16_t m_versionControl;
    uint32_t m_unk;
};

static_assert(sizeof(Group) == 0x18);

class GroupData
{
public:
    Vector<const void*> m_subGroups;
};

