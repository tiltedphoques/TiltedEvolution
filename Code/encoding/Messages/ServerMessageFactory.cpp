#pragma once

#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Allocator.hpp>
#include <Messages/ServerMessageFactory.h>

static std::function<UniquePtr<ServerMessage>(TiltedPhoques::Buffer::Reader& aReader)>
    s_serverMessageExtractor[kServerOpcodeMax];

namespace details
{
struct S
{
    S()
    {
        auto extractor = [](auto& x) {
            using T = typename std::remove_reference_t<decltype(x)>::Type;

            s_serverMessageExtractor[T::Opcode] = [](TiltedPhoques::Buffer::Reader& aReader) {
                auto ptr = TiltedPhoques::MakeUnique<T>();
                ptr->DeserializeRaw(aReader);
                return TiltedPhoques::CastUnique<ServerMessage>(std::move(ptr));
            };

            return false;
        };

        ServerMessageFactory::Visit(extractor);
    }
} s;
} // namespace details

UniquePtr<ServerMessage> ServerMessageFactory::Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept
{
    uint64_t data;
    aReader.ReadBits(data, sizeof(ServerOpcode) * 8);

    if (data >= kServerOpcodeMax) [[unlikely]]
        return {nullptr};

    const auto opcode = static_cast<ServerOpcode>(data);
    return s_serverMessageExtractor[opcode](aReader);
}
