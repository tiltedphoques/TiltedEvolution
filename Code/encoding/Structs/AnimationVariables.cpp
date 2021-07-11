#include <Structs/AnimationVariables.h>
#include <TiltedCore/Serialization.hpp>
#include <iostream>

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

void AnimationVariables::GenerateDiff(const AnimationVariables& aPrevious, TiltedPhoques::Buffer::Writer& aWriter) const
{
    uint64_t changes = 0;
    uint32_t idx = 0;

    if(Booleans != aPrevious.Booleans)
    {
        changes |= (1ull << idx);
    }
    ++idx;

    auto integers = aPrevious.Integers;
    if (integers.empty())
        integers.assign(Integers.size(), 0);

    for (auto i = 0u; i < Integers.size(); ++i)
    {
        if (Integers[i] != integers[i])
        {
            changes |= (1ull << idx);
        }
        ++idx;
    }

    auto floats = aPrevious.Floats;
    if (floats.empty())
        floats.assign(Floats.size(), 0.f);

    for (auto i = 0u; i < Floats.size(); ++i)
    {
        if (Floats[i] != floats[i])
        {
            changes |= (1ull << idx);
        }
        ++idx;
    }

    TiltedPhoques::Serialization::WriteVarInt(aWriter, Integers.size());
    TiltedPhoques::Serialization::WriteVarInt(aWriter, Floats.size());

    const auto cDiffBitCount = 1 + Integers.size() + Floats.size();

    aWriter.WriteBits(changes, cDiffBitCount);

    idx = 0;
    if (changes & (1ull << idx))
    {
        aWriter.WriteBits(Booleans, 64);
    }
    ++idx;

    for (const auto value : Integers)
    {
        if (changes & (1ull << idx))
        {
            TiltedPhoques::Serialization::WriteVarInt(aWriter, value & 0xFFFFFFFF);
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

void AnimationVariables::ApplyDiff(TiltedPhoques::Buffer::Reader& aReader)
{
    const auto cIntegersSize = TiltedPhoques::Serialization::ReadVarInt(aReader);
    if (cIntegersSize > 0xFF)
        throw std::runtime_error("Too many integers received !");

    if (Integers.size() != cIntegersSize)
    {
        Integers.assign(cIntegersSize, 0);
    }

    const auto cFloatsSize = TiltedPhoques::Serialization::ReadVarInt(aReader);
    if (cFloatsSize > 0xFF)
        throw std::runtime_error("Too many floats received !");

    if (Floats.size() != cFloatsSize)
    {
        Floats.assign(cFloatsSize, 0.f);
    }

    const auto cDiffBitCount = 1 + Integers.size() + Floats.size();

    uint64_t changes = 0;
    uint32_t idx = 0;

    aReader.ReadBits(changes, cDiffBitCount);

    if (changes & (1ull << idx))
    {
        aReader.ReadBits(Booleans, 64);
    }
    ++idx;

    for (auto& value : Integers)
    {
        if (changes & (1ull << idx))
        {
            value = TiltedPhoques::Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
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
