
#include "SteamCeg.h"

#include <algorithm>
#include <assert.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>

namespace steam
{
using namespace CryptoPP;

static uint32_t SteamXor(uint8_t* data, uint32_t size, uint32_t key)
{
    // Decode the data..
    for (size_t x = 4; x < size; x += 4)
    {
        auto val = *(uint32_t*)(data + x);
        *(uint32_t*)(data + x) = val ^ key;
        key = val;
    }

    return key;
}

// Just don't touch it, its evil
// ...sorry, it left me no choice.
uintptr_t CrackCEGInPlace(const CEGLocationInfo& acInfo)
{
    SteamStubHeaderV31 stub{};
    std::memcpy(&stub, acInfo.start - sizeof(SteamStubHeaderV31), sizeof(SteamStubHeaderV31));
    SteamXor(reinterpret_cast<uint8_t*>(&stub), sizeof(SteamStubHeaderV31), stub.XorKey);
    assert(stub.Signature == 0xC0DEC0DF);

    // Newer SteamStub v3.1 builds may leave the code section unencrypted.
    if (stub.CodeSectionVirtualAddress == 0 || stub.CodeSectionRawSize == 0)
    {
        return stub.OriginalEntryPoint;
    }

    // decrypt IV in place
    ECB_Mode<AES>::Decryption ecbDec(stub.AES_Key, sizeof(stub.AES_Key));
    ecbDec.ProcessData(stub.AES_IV, stub.AES_IV, sizeof(stub.AES_IV));

    constexpr auto kCodeSize = sizeof(SteamStubHeaderV31::CodeSectionStolenData);

    uint8_t* pText = acInfo.textSeg.start.as<uint8_t*>();
    std::memmove(pText + kCodeSize, pText, acInfo.textSeg.size - kCodeSize);
    std::memcpy(pText, stub.CodeSectionStolenData, kCodeSize);

    CBC_Mode<AES>::Decryption cbcDec(stub.AES_Key, sizeof(stub.AES_Key), stub.AES_IV);
    cbcDec.ProcessData(pText, pText, acInfo.textSeg.size);

    return stub.OriginalEntryPoint;
}
} // namespace steam
