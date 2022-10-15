
#include <cryptopp/crc.h>
#include "BSCore/BSCRC32.h"

namespace creation
{
void BSCRC32::GenerateCRC(uint32_t& aiCRC, uint32_t auiData, uint32_t auiDefaultvalue)
{
#if defined(DEBUG)
    // CryptoPP cant handle a default value.
    if (auiDefaultvalue != 0)
        __debugbreak();
#endif

    CryptoPP::CRC32 crc;
    crc.Update(reinterpret_cast<CryptoPP::byte*>(&auiData), sizeof(uint32_t));
    crc.TruncatedFinal(reinterpret_cast<CryptoPP::byte*>(&aiCRC), sizeof(uint32_t));
}
} // namespace creation

