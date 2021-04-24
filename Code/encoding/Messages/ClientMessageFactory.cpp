#pragma once

#include <Messages/ClientMessageFactory.h>

static std::function<UniquePtr<ClientMessage>(TiltedPhoques::Buffer::Reader& aReader)>
    s_clientMessageExtractor[kClientOpcodeMax];

namespace details
{
    struct S
    {
        S()
        {
            auto extractor = [](auto& x) {
                using T = typename std::remove_reference_t<decltype(x)>::Type;

                s_clientMessageExtractor[T::Opcode] = [](TiltedPhoques::Buffer::Reader& aReader) {
                    auto ptr = TiltedPhoques::MakeUnique<T>();
                    ptr->DeserializeRaw(aReader);
                    return TiltedPhoques::CastUnique<ClientMessage>(std::move(ptr));
                };

                return false;
            };

            ClientMessageFactory::Visit(extractor);
        }
    } s;
}


UniquePtr<ClientMessage> ClientMessageFactory::Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept
{
    uint64_t data;
    aReader.ReadBits(data, sizeof(ClientOpcode) * 8);

    if (data >= kClientOpcodeMax) [[unlikely]]
        return {nullptr};

    const auto opcode = static_cast<ClientOpcode>(data);
    return s_clientMessageExtractor[opcode](aReader);
}
