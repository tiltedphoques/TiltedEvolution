#include <Structs/ActionEvent.h>
#include <TiltedCore/Serialization.hpp>
#include <sstream>
#include <TiltedCore/StackAllocator.hpp>

using TiltedPhoques::Serialization;

enum DifferentialFlags
{
    kActionId           = 1 << 0,
    kTargetId           = 1 << 1,
    kIdleId             = 1 << 2,
    kState              = 1 << 3,
    kType               = 1 << 4,
    kEventName          = 1 << 5,
    kTargetEventName    = 1 << 6,
    kVariables          = 1 << 7
};

bool ActionEvent::operator==(const ActionEvent& acRhs) const noexcept
{
    return Tick == acRhs.Tick && 
        // ActorId == acRhs.ActorId && // ActorId is a local field as it is filled up latter by the game client
        ActionId == acRhs.ActionId &&
        State1 == acRhs.State1 &&
        State2 == acRhs.State2 &&
        Type == acRhs.Type &&
        TargetId == acRhs.TargetId && 
        IdleId == acRhs.IdleId && 
        EventName == acRhs.EventName && 
        TargetEventName == acRhs.TargetEventName &&
        Variables == acRhs.Variables;
}

bool ActionEvent::operator!=(const ActionEvent& acRhs) const noexcept
{
    return !operator==(acRhs);
}

void ActionEvent::GenerateDifferential(const ActionEvent& aPrevious, TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    uint8_t flags = 0;

    if (ActionId != aPrevious.ActionId)
        flags |= kActionId;

    if (TargetId != aPrevious.TargetId)
        flags |= kTargetId;

    if (IdleId != aPrevious.IdleId)
        flags |= kIdleId;

    if (State1 != aPrevious.State1 || State2 != aPrevious.State2)
        flags |= kState;

    if (Type != aPrevious.Type)
        flags |= kType;

    if (EventName != aPrevious.EventName)
        flags |= kEventName;

    if (TargetEventName != aPrevious.TargetEventName)
        flags |= kTargetEventName;

    if (Variables != aPrevious.Variables)
        flags |= kVariables;

    // Missing variables
    aWriter.WriteBits(flags, 8);

    {
        const auto tickDiff = Tick - aPrevious.Tick;
        Serialization::WriteVarInt(aWriter, tickDiff);
    }

    if(flags & kActionId)
    {
        Serialization::WriteVarInt(aWriter, ActionId);
    }

    if (flags & kTargetId)
    {
        Serialization::WriteVarInt(aWriter, TargetId);
    }

    if (flags & kIdleId)
    {
        Serialization::WriteVarInt(aWriter, IdleId);
    }

    if (flags & kState)
    {
        aWriter.WriteBits(State1, 32);
        aWriter.WriteBits(State2, 32);
    }

    if (flags & kType)
    {
        Serialization::WriteVarInt(aWriter, Type);
    }

    if (flags & kEventName)
    {
        EventName.Serialize(aWriter);
    }

    if (flags & kTargetEventName)
    {
        TargetEventName.Serialize(aWriter);
    }

    if (flags & kVariables)
    {
        Variables.GenerateDiff(aPrevious.Variables, aWriter);
    }
}

void ActionEvent::ApplyDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t flags = 0;

    aReader.ReadBits(flags, 8);

    {
        const auto tickDiff = Serialization::ReadVarInt(aReader);
        Tick += tickDiff;
    }

    if (flags & kActionId)
    {
        ActionId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    }

    if (flags & kTargetId)
    {
        TargetId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    }

    if (flags & kIdleId)
    {
        IdleId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    }

    if (flags & kState)
    {
        uint64_t tmp = 0;
        aReader.ReadBits(tmp, 32);
        State1 = tmp & 0xFFFFFFFF;
        aReader.ReadBits(tmp, 32);
        State2 = tmp & 0xFFFFFFFF;
    }

    if (flags & kType)
    {
        Type = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    }

    if (flags & kEventName)
    {
        EventName.Deserialize(aReader);
    }

    if (flags & kTargetEventName)
    {
        TargetEventName.Deserialize(aReader);
    }

    if (flags & kVariables)
    {
        Variables.ApplyDiff(aReader);
    }
}
