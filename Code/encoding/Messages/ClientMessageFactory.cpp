#pragma once

#include <Messages/ClientMessageFactory.h>
#include <Decoding.h>

static std::function<UniquePtr<ClientMessage>(TiltedPhoques::Buffer::Reader& aReader)> s_clientMessageExtractor[kClientOpcodeMax];

namespace details
{
static struct ClientMessageFactoryInit
{
    ClientMessageFactoryInit()
    {
        auto extractor = [](auto& x)
        {
            using T = typename std::remove_reference_t<decltype(x)>::Type;

            s_clientMessageExtractor[T::Opcode] = [](TiltedPhoques::Buffer::Reader& aReader) -> UniquePtr<ClientMessage>
            {
                auto ptr = TiltedPhoques::MakeUnique<T>();
                ptr->DeserializeRaw(aReader);
                if (Decoding::IsMalformed(aReader)) [[unlikely]]
                    return {nullptr};
                return TiltedPhoques::CastUnique<ClientMessage>(std::move(ptr));
            };

            return false;
        };

        ClientMessageFactory::Visit(extractor);
    }
} s_ClientMessageFactoryInit;
} // namespace details

UniquePtr<ClientMessage> ClientMessageFactory::Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept
{
    uint64_t data;
    aReader.ReadBits(data, sizeof(ClientOpcode) * 8);

    if (data >= kClientOpcodeMax) [[unlikely]]
        return {nullptr};

    const auto opcode = static_cast<ClientOpcode>(data);
    // Calling an empty std::function throws, so opcodes without a registered message are dropped here.
    if (!s_clientMessageExtractor[opcode]) [[unlikely]]
        return {nullptr};

    return s_clientMessageExtractor[opcode](aReader);
}
