/**
 * Steamless Steam DRM Remover - SteamStub_Variant3.h
 * (c) 2015 atom0s [atom0s@live.com]
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/
 */
#pragma once
#include <cstdint>

namespace steam
{
struct SteamStubHeaderV31
{
    uint32_t XorKey;               // The base xor key, if defined, to unpack the file with.
    uint32_t Signature;            // The signature to ensure the xor decoding was successful.
    uintptr_t ImageBase;           // The base of the image that was protected.
    uintptr_t AddressOfEntryPoint; // The entry point that is set from the DRM.
    uint32_t BindSectionOffset;    // The starting offset to the .bind section data. RVA(AddressOfEntryPoint -
    uint32_t Unknown0000;          // [Cyanic: This field is most likely the .bind code size.] public
    uintptr_t OriginalEntryPoint;  // The original entry point of the binary before it was protected.
    uint32_t Unknown0001;          // [Cyanic: This field is most likely an offset to a string table.]
    uint32_t PayloadSize;
    uint32_t DRMPDllOffset; // The offset to the SteamDrmp.dll file.
    uint32_t DRMPDllSize; // The size of the SteamDrmp.dll file. uint32_tSteamAppId; // The Steam application id of this
    uint32_t Flags;       // The DRM flags used while protecting this program.
    uint32_t BindSectionVirtualSize;     // The .bind section virtual size.
    uint32_t Unknown0002;                // [Cyanic: This field is most likely a hash of some sort.]
    uintptr_t CodeSectionVirtualAddress; // The code section virtual address.
    uintptr_t CodeSectionRawSize;        // The code section raw size.

    uint8_t AES_Key[32];
    uint8_t AES_IV[16];
    uint8_t CodeSectionStolenData[16]; // The first 16 bytes of the code section stolen.
    uint32_t EncryptionKeys[4];        // Encryption keys used to decrypt the SteamDrmp.dll file.
    uint32_t Unknown0003[8];           // Unknown unused data.

    uintptr_t GetModuleHandleA_Rva; // The rva to GetModuleHandleA.
    uintptr_t GetModuleHandleW_Rva; // The rva to GetModuleHandleW.
    uintptr_t LoadLibraryA_Rva;     // The rva to LoadLibraryA.
    uintptr_t LoadLibraryW_Rva;     // The rva to LoadLibraryW.
    uintptr_t GetProcAddress_Rva;   // The rva to GetProcAddress.
};

static_assert(sizeof(SteamStubHeaderV31) == 240);
}
