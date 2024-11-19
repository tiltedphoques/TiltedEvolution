#pragma once
#include "Message.h"
#include <Structs/GameId.h>

struct RemoveSpellRequest final: ClientMessage
{
    static constexpr ClientOpcode Opcode = kRequestRemoveSpell;
    RemoveSpellRequest() : ClientMessage(Opcode) {}
    virtual ~RemoveSpellRequest() = default;
    void SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept override;
    void DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept override;
    bool operator==(const RemoveSpellRequest& achRhs) const noexcept { return  Opcode == achRhs.Opcode && TargetId == achRhs.TargetId && SpellId == achRhs.SpellId; }

    uint32_t TargetId{};
    GameId SpellId{};
};
