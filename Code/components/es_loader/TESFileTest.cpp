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

private:
    TESFile m_file;
};

void TESFileTest::SetUp()
{
    m_file.LoadFile("Data\\Skyrim.esm");
    m_file.IndexRecords();
}

TESFile& TESFileTest::GetFile()
{
    return m_file;
}

TEST(TESFileTests, IndexRecords)
{
    TESFile skyrimEsm;
    skyrimEsm.LoadFile("Skyrim.esm");
    bool result = skyrimEsm.IndexRecords();
    EXPECT_TRUE(result);
}

TEST_F(TESFileTest, LoadNPCCompressedFaendal)
{
    NPC& npc = GetFile().GetNpcById(0x13480);
    npc.ParseChunks();

    EXPECT_FALSE(npc.m_baseStats.IsFemale());
}
} // namespace
