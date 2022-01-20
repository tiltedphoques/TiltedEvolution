#include <gtest/gtest.h>
#include <TESFile.h>

#include <Records/NPC.h>

namespace
{
class TESFileTest : public ::testing::Test
{
public:
    void SetUp() override;
    TESFile& GetFile();
    RecordCollection& GetCollection();

private:
    TESFile m_file;
    RecordCollection m_collection;
};

void TESFileTest::SetUp()
{
    m_file.LoadFile("Data\\Skyrim.esm");
    m_file.IndexRecords(m_collection);
}

TESFile& TESFileTest::GetFile()
{
    return m_file;
}

RecordCollection& TESFileTest::GetCollection()
{
    return m_collection;
}

TEST(TESFileTests, IndexRecords)
{
    TESFile skyrimEsm;
    skyrimEsm.LoadFile("Skyrim.esm");
    RecordCollection collection;
    bool result = skyrimEsm.IndexRecords(collection);
    EXPECT_TRUE(result);
}

TEST_F(TESFileTest, LoadNPCCompressedFaendal)
{
    NPC& npc = GetCollection().GetNpcById(0x13480);
    npc.ParseChunks();

    EXPECT_FALSE(npc.m_baseStats.IsFemale());
}
} // namespace
