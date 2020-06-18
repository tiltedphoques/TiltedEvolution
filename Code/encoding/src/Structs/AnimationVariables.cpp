#include <Structs/AnimationVariables.h>
#include <Serialization.hpp>

bool AnimationVariables::operator==(const AnimationVariables& acRhs) const noexcept
{
    return Booleans == acRhs.Booleans &&
        Integers == acRhs.Integers &&
        Floats == acRhs.Floats;
}

bool AnimationVariables::operator!=(const AnimationVariables& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void AnimationVariables::Load(std::istream& aInput)
{
    aInput.read(reinterpret_cast<char*>(&Booleans), sizeof(Booleans));
    aInput.read(reinterpret_cast<char*>(Integers.data()), Integers.size() * sizeof(uint32_t));
    aInput.read(reinterpret_cast<char*>(Floats.data()), Floats.size() * sizeof(float));
}

void AnimationVariables::Save(std::ostream& aOutput) const
{
    aOutput.write(reinterpret_cast<const char*>(&Booleans), sizeof(Booleans));
    aOutput.write(reinterpret_cast<const char*>(Integers.data()), Integers.size() * sizeof(uint32_t));
    aOutput.write(reinterpret_cast<const char*>(Floats.data()), Floats.size() * sizeof(float));
}

static constexpr uint32_t s_diffBitCount = 1 + AnimationData::kIntegerCount + AnimationData::kFloatCount;

void AnimationVariables::GenerateDiff(const AnimationVariables& aPrevious, TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    uint32_t changes = 0;
    uint32_t idx = 0;

    if(Booleans != aPrevious.Booleans)
    {
        changes |= (1ull << idx);
    }
    ++idx;

    for (auto i = 0u; i < Integers.size(); ++i)
    {
        if (Integers[i] != aPrevious.Integers[i])
        {
            changes |= (1ull << idx);
        }
        ++idx;
    }

    for (auto i = 0u; i < Floats.size(); ++i)
    {
        if (Floats[i] != aPrevious.Floats[i])
        {
            changes |= (1ull << idx);
        }
        ++idx;
    }

    aWriter.WriteBits(changes, s_diffBitCount);

    idx = 0;
    if (changes & (1ull << idx))
    {
        aWriter.WriteBits(Booleans, AnimationData::kBooleanCount);
    }
    ++idx;

    for (const auto value : Integers)
    {
        if (changes & (1ull << idx))
        {
            aWriter.WriteBits(value, 32);
        }
        ++idx;
    }

    for (const auto value : Floats)
    {
        if (changes & (1ull << idx))
        {
            aWriter.WriteBits(*reinterpret_cast<const uint32_t*>(&value), 32);
        }
        ++idx;
    }
}

void AnimationVariables::ApplyDiff(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t changes = 0;
    uint32_t idx = 0;

    aReader.ReadBits(changes, s_diffBitCount);

    if (changes & (1ull << idx))
    {
        aReader.ReadBits(Booleans, AnimationData::kBooleanCount);
    }
    ++idx;

    for (auto& value : Integers)
    {
        if (changes & (1ull << idx))
        {
            uint64_t tmp = 0;
            aReader.ReadBits(tmp, 32);
            value = tmp & 0xFFFFFFFF;
        }
        ++idx;
    }

    for (auto& value : Floats)
    {
        if (changes & (1ull << idx))
        {
            uint64_t tmp = 0;
            aReader.ReadBits(tmp, 32);
            uint32_t data = tmp & 0xFFFFFFFF;
            value = *reinterpret_cast<float*>(&data);
        }
        ++idx;
    }
}
