#pragma once

#include <unordered_map>
#include <string>

#include <imgui.h>

#include <Events/UpdateEvent.h>

struct World;
struct PlayerCharacter;
struct Actor;
struct NiPoint3;
struct ImguiService;

/**
 * @brief Draws 3D-aware name tags above other players' heads.
 *
 * Plain name text (a crown marks the party leader), scaled with distance.
 * Which tags show, and how, is set by the "Player UI filters" settings in the
 * overlay (see SetSettings).
 */
struct NameTagService
{
    // Matches the order of PlayerUiFilter in the UI.
    enum class Filter : uint8_t
    {
        kOn = 0,
        kParty = 1,
        kOff = 2,
    };

    struct Settings
    {
        Filter NameTags = Filter::kParty;
        Filter Backdrop = Filter::kParty;
        float FontSize = 16.f;
        float NameFadeStart = 3000.f;
        float NameInvisible = 5000.f;
        float PartyFadeStart = 3000.f;
        float PartyInvisible = 5000.f;
        // Drawn in italics instead of the name of players whose name is hidden.
        String StrangerLabel = "Stranger";
        // Players whose real name may be shown. The UI decides this from the
        // stranger name setting, party membership and the per-name eye.
        Set<uint32_t> RevealedPlayerIds;
    };

    NameTagService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~NameTagService() noexcept = default;

    TP_NOCOPYMOVE(NameTagService);

    void SetSettings(Settings aSettings) noexcept;

    // Tunables (kept few on purpose). Pixel values are at 1080p and scale with the screen height.
    static constexpr float kNearBypassDistance = 600.f;
    // The tag anchors to the neck bone, which stays on the body when dismemberment mods remove the head.
    // World units from the neck bone to the top of the head, for a 128-unit tall actor.
    static constexpr float kHeadTopAboveNeck = 24.f;
    // Clearance above the head for helmets, in world units, clamped on screen to [kMinGapPx, kMaxGapPx].
    static constexpr float kHelmetClearance = 16.f;
    static constexpr float kMinGapPx = 14.f;
    static constexpr float kMaxGapPx = 56.f;
    // Text scale = clamp(kScaleDistance / distance, kMinScale, kMaxScale).
    static constexpr float kScaleDistance = 800.f;
    static constexpr float kMinScale = 0.65f;
    static constexpr float kMaxScale = 1.2f;
    // UI font sizes are tuned for the overlay; tags match the HUD's activate prompt (16 -> 22 px).
    static constexpr float kFontSizeScale = 1.375f;
    static constexpr float kItalicShear = 0.2f;
    static constexpr float kVisibilityEpsilon = 1e-3f;

private:
    struct VisibilityInfo
    {
        bool Visible = true;
        uint64_t Tick = 0;
    };

    void OnDraw() noexcept;
    void OnUpdate(const UpdateEvent& acEvent) noexcept;

    [[nodiscard]] bool ShouldRenderTag(uint32_t aPlayerId) const noexcept;
    [[nodiscard]] bool ProjectWorldPoint(const NiPoint3& aWorldPoint, ImVec2& aScreenPos, float& aDepth, const ImVec2& aViewport) const noexcept;
    [[nodiscard]] NiPoint3 BuildHeadTopPoint(Actor* apActor) const noexcept;
    [[nodiscard]] static bool IsEnabledFor(Filter aFilter, bool aIsPartyMember) noexcept;
    [[nodiscard]] static ImU32 ColorWithAlpha(const ImVec4& aColor, float aAlpha) noexcept;

    World& m_world;

    Settings m_settings;
    std::unordered_map<uint32_t, VisibilityInfo> m_visibility;

    entt::scoped_connection m_drawConnection;
    entt::scoped_connection m_updateConnection;
};
