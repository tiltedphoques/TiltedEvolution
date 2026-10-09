#include <TiltedOnlinePCH.h>

#include <Offsets.h>

#include <BuildInfo.h>
#include <array>
#include <spdlog/spdlog.h>

namespace fo4
{
#include <OffsetsData.h>

bool ValidateImage(const std::filesystem::path& acExePath) noexcept
{
    std::array<uint8_t, 4096> headers{};
    std::ifstream file(acExePath, std::ios::binary);
    file.read(reinterpret_cast<char*>(headers.data()), headers.size());
    if (!file)
        return false;
    const auto* pDos = reinterpret_cast<const IMAGE_DOS_HEADER*>(headers.data());
    if (pDos->e_magic != IMAGE_DOS_SIGNATURE || pDos->e_lfanew <= 0 ||
        pDos->e_lfanew > headers.size() - sizeof(IMAGE_NT_HEADERS64))
        return false;
    const auto* pNt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(headers.data() + pDos->e_lfanew);
    return pNt->Signature == IMAGE_NT_SIGNATURE && pNt->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 &&
           pNt->FileHeader.TimeDateStamp == kImageTimestamp && pNt->OptionalHeader.SizeOfImage == kImageSize;
}

} // namespace fo4
