#if TP_SKYRIMVR

#include <Services/SpellSyncExclusions.h>

#include <base/simpleini/SimpleIni.h>

#include <spdlog/fmt/fmt.h>

#include <array>
#include <filesystem>
#include <string>
#include <vector>

#include <Games/TES.h>

namespace
{
constexpr char kConfigPathName[] = "Data/SkyrimTogetherReborn/config";
constexpr char kSettingsFileName[] = "SpellSyncExclusions.ini";
constexpr char kSectionName[] = "SpellSyncExclusions";
constexpr char kSettingsComment[] =
    "; Plugins whose spells and magic effects are never sent to, or replayed from, other players.\n"
    "; Add one entry per plugin: PluginFileName.esp = 1, and set it to 0 to stop excluding it.\n"
    "; Use this for plugins that bind a spell to a local control surface -- a configuration menu on a\n"
    "; lesser power, a hotkey wheel, a grab gesture -- where running it on another player is wrong.\n"
    "; Vanilla powers and shouts are replayed on purpose, so do not list plugins that add real spells.\n"
    "; Removing every entry disables the exclusion list entirely.";

// Defaults, not a fixed policy. VRIK is the case actually observed: its configuration menu is bound
// to a lesser power, so casting it replayed onto the other player and opened that menu on their
// screen. The other two are listed because they bind spells to local controls in the same way, not
// because each was separately observed. All three are written to the ini on first run so a user can
// change, disable or remove them.
constexpr std::array<const char*, 3> kDefaultPlugins{"vrik.esp", "SpellWheelVR.esp", "higgs_vr.esp"};

std::filesystem::path GetSettingsPath()
{
    return std::filesystem::current_path() / kConfigPathName / kSettingsFileName;
}

bool CreateDefaultSettings(const std::filesystem::path& acPath)
{
    std::error_code error{};
    std::filesystem::create_directories(acPath.parent_path(), error);
    if (error)
        return false;

    CSimpleIni ini(true);
    ini.SetValue(kSectionName, nullptr, nullptr, kSettingsComment);

    for (const char* pPlugin : kDefaultPlugins)
        ini.SetValue(kSectionName, pPlugin, "1");

    return ini.SaveFile(acPath.c_str(), true) == SI_Error::SI_OK;
}

/**
 * @brief The configured plugin names, read once from the ini.
 *
 * Separate from slot resolution below, because the file is available long before the mod list is.
 */
const std::vector<std::string>& GetConfiguredPlugins() noexcept
{
    static const std::vector<std::string> s_plugins = []() -> std::vector<std::string> {
        std::vector<std::string> plugins;

        // An empty list is a valid choice and disables the feature, so a file we could not read must
        // NOT look like one. On any failure the compiled defaults are kept and the failure is logged:
        // silently losing the filter is the worse outcome of the two.
        const auto UseDefaults = [&plugins](const char* acpWhy) {
            plugins.clear();

            for (const char* pPlugin : kDefaultPlugins)
                plugins.emplace_back(pPlugin);

            spdlog::warn("Spell sync exclusions: {}; falling back to the built-in defaults", acpWhy);
        };

        try
        {
            const auto path = GetSettingsPath();

            std::error_code error{};
            const bool cExists = std::filesystem::exists(path, error);

            if (error)
            {
                UseDefaults("could not check for the configuration file");
                return plugins;
            }

            if (!cExists && !CreateDefaultSettings(path))
            {
                UseDefaults("could not write the default configuration file");
                return plugins;
            }

            CSimpleIni ini(true);
            const auto loadResult = ini.LoadFile(path.c_str());
            if (loadResult != SI_Error::SI_OK)
            {
                UseDefaults(fmt::format("failed to read '{}' (error {})", path.string(), static_cast<int>(loadResult)).c_str());
                return plugins;
            }

            CSimpleIni::TNamesDepend keys;
            ini.GetAllKeys(kSectionName, keys);

            for (const auto& key : keys)
            {
                const char* pValue = ini.GetValue(kSectionName, key.pItem);

                // Any entry explicitly turned off is kept in the file but not applied, so a user can
                // disable one without losing the name.
                if (pValue && (pValue[0] == '0' || _stricmp(pValue, "false") == 0))
                    continue;

                if (key.pItem && *key.pItem)
                    plugins.emplace_back(key.pItem);
            }
        }
        catch (...)
        {
            UseDefaults("the configuration file could not be processed");
            return plugins;
        }

        if (plugins.empty())
            spdlog::info("Spell sync exclusions: none configured, spell and effect filtering is off");
        else
            spdlog::info("Spell sync exclusions: {} plugin(s) configured", plugins.size());

        return plugins;
    }();

    return s_plugins;
}

struct ResolvedSlots
{
    // Standard plugins occupy the top byte of a form id; light plugins occupy bits 12-23 under 0xFE.
    std::vector<uint8_t> Standard;
    std::vector<uint16_t> Lite;
    size_t LastModCount = 0;
    bool Complete = false;
};

/**
 * @brief Load-order slots for the configured plugins, resolved against the live mod list.
 *
 * Resolution is NOT frozen after a partial result. A spell can be cast before the mod list is
 * populated. Resolve again until all configured plugins are found or a nonempty mod count is
 * unchanged between attempts. The latter is a heuristic for absent optional plugins, not proof
 * that engine loading has finished. Each attempt walks the mod list per configured name.
 */
size_t CountLoadedMods(const ModManager& acManager) noexcept
{
    size_t count = 0;

    for (const auto* pEntry = &acManager.mods.entry; pEntry && pEntry->data; pEntry = pEntry->next)
        ++count;

    return count;
}

const ResolvedSlots& GetResolvedSlots() noexcept
{
    static ResolvedSlots s_slots;

    if (s_slots.Complete)
        return s_slots;

    const auto& cPlugins = GetConfiguredPlugins();

    if (cPlugins.empty())
    {
        s_slots.Complete = true;
        return s_slots;
    }

    ModManager* pManager = ModManager::Get();
    if (!pManager)
        return s_slots;

    s_slots.Standard.clear();
    s_slots.Lite.clear();

    size_t found = 0;

    for (const auto& cName : cPlugins)
    {
        Mod* pMod = pManager->GetByName(cName.c_str());
        if (!pMod || !pMod->IsLoaded())
            continue;

        ++found;

        // IsLite reads the plugin's own flag, which is the game's answer; the standard id being 0xFE
        // is a consequence of that flag rather than the definition of it.
        if (pMod->IsLite())
            s_slots.Lite.push_back(pMod->liteId);
        else
            s_slots.Standard.push_back(pMod->standardId);
    }

    // A configured plugin that is simply not installed is the normal case -- every default here is a
    // VR mod, and a desktop client has none of them. Waiting for all of them would rescan the mod list
    // on every spell for the whole session. Instead the load order is treated as settled once its
    // length stops changing between attempts. This is a heuristic, not a load-complete event.
    const size_t cModCount = CountLoadedMods(*pManager);
    const bool cSettled = cModCount != 0 && cModCount == s_slots.LastModCount;

    s_slots.LastModCount = cModCount;

    if (found == cPlugins.size() || cSettled)
    {
        s_slots.Complete = true;

        // Per plugin, because a count alone cannot tell two working exclusions from none: a silently
        // unresolved name is exactly the failure this line exists to make visible.
        for (const auto& cName : cPlugins)
        {
            Mod* pMod = pManager->GetByName(cName.c_str());

            if (!pMod || !pMod->IsLoaded())
                spdlog::warn("Spell sync exclusion '{}' is not loaded; nothing from it will be filtered", cName);
            else if (pMod->IsLite())
                spdlog::info("Spell sync exclusion '{}' resolved to light slot {:X}", cName, pMod->liteId);
            else
                spdlog::info("Spell sync exclusion '{}' resolved to standard slot {:X}", cName, pMod->standardId);
        }
    }

    return s_slots;
}
} // namespace

bool SpellSyncExclusions::IsExcluded(const uint32_t aFormId) noexcept
{
    const uint32_t cHi = aFormId >> 24;

    // 0xFF is a form created at runtime and 0 is no form at all. Neither has an originating plugin,
    // and treating either as a match would exclude content that belongs to nobody.
    if (aFormId == 0 || cHi == 0xFFu)
        return false;

    const ResolvedSlots& cSlots = GetResolvedSlots();

    if (cHi == 0xFEu)
    {
        const uint16_t cLiteId = static_cast<uint16_t>((aFormId >> 12) & 0xFFFu);

        for (const uint16_t cSlot : cSlots.Lite)
        {
            if (cSlot == cLiteId)
                return true;
        }

        return false;
    }

    for (const uint8_t cSlot : cSlots.Standard)
    {
        if (cSlot == static_cast<uint8_t>(cHi))
            return true;
    }

    return false;
}

#endif // TP_SKYRIMVR
