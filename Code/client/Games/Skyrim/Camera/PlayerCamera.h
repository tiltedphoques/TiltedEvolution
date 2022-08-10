#pragma once

#include <Camera/TESCamera.h>

struct PlayerCamera : public TESCamera
{
    static PlayerCamera* Get() noexcept;

    bool IsFirstPerson() noexcept;

    bool WorldPtToScreenPt3(const NiPoint3& in, NiPoint3& out, float zeroTolerance = 1e-5f);

    void ForceFirstPerson() noexcept;
    void ForceThirdPerson() noexcept;

    char pad39[0x13C - 0x39];
    float fWorldFOV;            // 13C
    float firstPersonFOV;       // 140
    NiPoint3 pos;               // 144 - ?
    float idleTimer;            // 150 - ?
    float yaw;                  // 154 - ? - in radians
    float pitch;                // 158 - ?
    float fOriginalPitchToZero; // 15C - ?
    bool allowAutoVanityMode;   // 160
    bool bowZoomedIn;           // 161
    bool isWeapSheathed;        // 162 - ?
    bool isProcessed;           // 163 - ?
    std::uint8_t unk164;        // 164
    std::uint8_t unk165;        // 165
    std::uint16_t pad166;       // 166
};

static_assert(offsetof(PlayerCamera, PlayerCamera::pos) == 0x144);
static_assert(sizeof(PlayerCamera) == 0x168);
