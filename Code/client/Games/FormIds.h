#pragma once

// Base game forms the client refers to directly. Zero means the game has no
// equivalent form, which TESForm::GetById resolves to null.
namespace FormIds
{
#if defined(TP_FALLOUT4)
inline constexpr uint32_t KillMoveGlobal = 0x100F19;
inline constexpr uint32_t WorldEncountersEnabledGlobal = 0;
inline constexpr uint32_t WerewolfBeastRace = 0;
inline constexpr uint32_t VampireLordRace = 0;
inline constexpr uint32_t NoResetEncounterZone = 0;
inline constexpr uint32_t MapWeather = 0;
inline constexpr uint32_t ActionInstantInitializeGraphToBaseState = 0x5704C;
#else
inline constexpr uint32_t KillMoveGlobal = 0x100F19;
inline constexpr uint32_t WorldEncountersEnabledGlobal = 0xB8EC1;
inline constexpr uint32_t WerewolfBeastRace = 0xCDD84;
inline constexpr uint32_t VampireLordRace = 0x200283A;
inline constexpr uint32_t NoResetEncounterZone = 0xF90B1;
inline constexpr uint32_t MapWeather = 0xA6858;
inline constexpr uint32_t ActionInstantInitializeGraphToBaseState = 0;
#endif
} // namespace FormIds
