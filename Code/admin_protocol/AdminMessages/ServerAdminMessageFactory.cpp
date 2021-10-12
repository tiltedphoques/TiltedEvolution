#pragma once

#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Allocator.hpp>
#include "ServerAdminMessageFactory.h"

static std::function<UniquePtr<ServerAdminMessage>(TiltedPhoques::Buffer::Reader& aReader)>
    s_serverAdminMessageExtractor[kServerAdminOpcodeMax];

namespace details
{
static struct ServerAdminMessageFactoryInit
{
    ServerAdminMessageFactoryInit()
    {
        auto extractor = [](auto& x) {
            using T = typename std::remove_reference_t<decltype(x)>::Type;

            s_serverAdminMessageExtractor[T::Opcode] = [](TiltedPhoques::Buffer::Reader& aReader) {
                auto ptr = TiltedPhoques::MakeUnique<T>();
                ptr->DeserializeRaw(aReader);
                return TiltedPhoques::CastUnique<ServerAdminMessage>(std::move(ptr));
            };

            return false;
        };

        ServerAdminMessageFactory::Visit(extractor);
    }
} s_ServerAdminMessageFactoryInit;
} // namespace details

UniquePtr<ServerAdminMessage> ServerAdminMessageFactory::Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept
{
    uint64_t data;
    aReader.ReadBits(data, sizeof(ServerAdminOpcode) * 8);

    if (data >= kServerAdminOpcodeMax) [[unlikely]]
        return {nullptr};

    const auto opcode = static_cast<ServerAdminOpcode>(data);
    return s_serverAdminMessageExtractor[opcode](aReader);
}
