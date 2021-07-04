#pragma once

#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Allocator.hpp>
#include "ClientAdminMessageFactory.h"

static std::function<UniquePtr<ClientAdminMessage>(TiltedPhoques::Buffer::Reader& aReader)>
    s_clientAdminMessageExtractor[kClientAdminOpcodeMax];

namespace details
{
static struct ClientAdminMessageFactoryInit
{
    ClientAdminMessageFactoryInit()
    {
        auto extractor = [](auto& x) {
            using T = typename std::remove_reference_t<decltype(x)>::Type;

            s_clientAdminMessageExtractor[T::Opcode] = [](TiltedPhoques::Buffer::Reader& aReader) {
                auto ptr = TiltedPhoques::MakeUnique<T>();
                ptr->DeserializeRaw(aReader);
                return TiltedPhoques::CastUnique<ClientAdminMessage>(std::move(ptr));
            };

            return false;
        };

        ClientAdminMessageFactory::Visit(extractor);
    }
} s_ClientAdminMessageFactoryInit;
} // namespace details

UniquePtr<ClientAdminMessage> ClientAdminMessageFactory::Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept
{
    uint64_t data;
    aReader.ReadBits(data, sizeof(ClientAdminOpcode) * 8);

    if (data >= kClientAdminOpcodeMax) [[unlikely]]
        return {nullptr};

    const auto opcode = static_cast<ClientAdminOpcode>(data);
    return s_clientAdminMessageExtractor[opcode](aReader);
}
