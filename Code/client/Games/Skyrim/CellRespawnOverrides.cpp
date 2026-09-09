#include <Games/Skyrim/CellRespawnOverrides.h>

#include <base/simpleini/SimpleIni.h>

#include <array>
#include <cstdio>
#include <filesystem>
#include <string_view>

#include <spdlog/fmt/fmt.h>

#include <Games/Primitives.h>

namespace
{
constexpr char kConfigPathName[] = "Data/SkyrimTogetherReborn/config";
constexpr char kSettingsFileName[] = "CellRespawnOverrides.ini";
constexpr char kSectionName[] = "CellRespawnOverrides";
constexpr char kSettingsComment[] =
    "; Local respawn-position overrides for problematic cells.\n"
    "; Add one entry per cell: CellEditorId = x, y, z\n"
    "; Use the cell editor ID from sources like UESP. Stand on safe ground and get the coordinates in-game with\n"
    "; player.getpos x, player.getpos y, and player.getpos z. Decimal coordinates are supported.";

struct RespawnOverride
{
    std::string_view CellEditorId;
    NiPoint3 Position;
};

const std::array kDefaultOverrides{
    RespawnOverride{"HaltedStreamCamp01", glm::vec3(-515.f, -501.f, 603.f)},
    RespawnOverride{"Korvanjund01", glm::vec3(-287.f, -5619.f, 608.f)},
    RespawnOverride{"Korvanjund02", glm::vec3(1201.f, -636.f, -400.f)},
    RespawnOverride{"QASmoke", glm::vec3(363.f, 2035.f, 7152.f)},
    RespawnOverride{"Saarthal02", glm::vec3(888.f, -269.f, -4.f)},
    RespawnOverride{"ShimmermistCave02", glm::vec3(5009.f, 57.f, -1319.f)},
    RespawnOverride{"StillbornCave01", glm::vec3(1473.f, -1044.f, -45.f)},
    RespawnOverride{"SwindlersDen01", glm::vec3(1470.f, -1651.f, 1969.f)},
};

bool ReadPosition(const char* apValue, NiPoint3& aOutPosition) noexcept
{
    if (!apValue)
        return false;

    float x{};
    float y{};
    float z{};
    char trailingCharacter{};
    if (sscanf_s(apValue, " %f , %f , %f %c", &x, &y, &z, &trailingCharacter, 1) != 3)
        return false;

    aOutPosition = glm::vec3(x, y, z);
    return true;
}

std::filesystem::path GetSettingsPath()
{
    return std::filesystem::current_path() / kConfigPathName / kSettingsFileName;
}

void CreateDefaultSettings(const std::filesystem::path& acPath)
{
    std::error_code error{};
    std::filesystem::create_directories(acPath.parent_path(), error);
    if (error)
        return;

    CSimpleIni ini(true);
    ini.SetValue(kSectionName, nullptr, nullptr, kSettingsComment);

    for (const auto& entry : kDefaultOverrides)
    {
        const auto value = fmt::format("{}, {}, {}", entry.Position.x, entry.Position.y, entry.Position.z);
        ini.SetValue(kSectionName, entry.CellEditorId.data(), value.c_str());
    }

    ini.SaveFile(acPath.c_str(), true);
}

struct RespawnSettings
{
    RespawnSettings() noexcept
    {
        try
        {
            const auto path = GetSettingsPath();

            std::error_code error{};
            if (!std::filesystem::exists(path, error))
                CreateDefaultSettings(path);

            if (error)
                return;

            const auto loadResult = Ini.LoadFile(path.c_str());
            if (loadResult != SI_Error::SI_OK)
                spdlog::warn("Failed to load cell respawn overrides from '{}' (error {})", path.string(), static_cast<int>(loadResult));
        }
        catch (...)
        {
        }
    }

    CSimpleIni Ini{true};
};

const CSimpleIni& GetSettings() noexcept
{
    static const RespawnSettings s_settings;
    return s_settings.Ini;
}

bool GetConfiguredRespawnPos(const char* apCellEditorId, NiPoint3& aOutPosition) noexcept
{
    const char* pValue = GetSettings().GetValue(kSectionName, apCellEditorId);
    if (!pValue)
        return false;

    if (!ReadPosition(pValue, aOutPosition))
    {
        spdlog::warn("Invalid respawn override for cell '{}': '{}'", apCellEditorId, pValue);
        return false;
    }

    return true;
}
} // namespace

bool CellRespawnOverrides::GetRespawnPos(const char* apCellEditorId, NiPoint3& aOutPos) noexcept
{
    if (!apCellEditorId || !*apCellEditorId)
        return false;

    if (GetConfiguredRespawnPos(apCellEditorId, aOutPos))
        return true;

    for (const auto& entry : kDefaultOverrides)
    {
        if (entry.CellEditorId == apCellEditorId)
        {
            aOutPos = entry.Position;
            return true;
        }
    }

    return false;
}

bool CellRespawnOverrides::GetRespawnPos(TESObjectCELL* apCell, NiPoint3& aOutPos) noexcept
{
    if (!apCell)
        return false;

    return GetRespawnPos(apCell->GetFormEditorID(), aOutPos);
}
