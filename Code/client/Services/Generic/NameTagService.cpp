#include <TiltedOnlinePCH.h>

#include <Services/NameTagService.h>

#include <Services/ImguiService.h>
#include <Services/PartyService.h>

#include <World.h>

#include <PlayerCharacter.h>
#include <Actor.h>
#include <Games/ActorExtension.h>
#include <Games/Skyrim/Interface/Menus/HUDMenuUtils.h>
#include <Games/Skyrim/Interface/UI.h>
#include <Games/Skyrim/Misc/BSFixedString.h>
#include <Games/Skyrim/NetImmerse/NiNode.h>
#include <Games/Skyrim/Misc/CrosshairPickData.h>

#include <imgui_internal.h>

#include <string>

namespace
{
float Clamp01(float aValue) noexcept
{
    return std::clamp(aValue, 0.f, 1.f);
}

float SmoothStep(float aEdge0, float aEdge1, float aValue) noexcept
{
    if (aEdge0 == aEdge1)
        return aValue >= aEdge1 ? 1.f : 0.f;

    const float t = Clamp01((aValue - aEdge0) / (aEdge1 - aEdge0));
    return t * t * (3.f - 2.f * t);
}

bool HasAllGlyphs(ImFont* apFont, const String& acText) noexcept
{
    const char* pText = acText.c_str();
    const char* pEnd = pText + acText.size();
    while (pText < pEnd)
    {
        unsigned int codepoint = 0;
        pText += ImTextCharFromUtf8(&codepoint, pText, pEnd);
        if (codepoint < 0x80)
            continue;
        if (!apFont->FindGlyphNoFallback(static_cast<ImWchar>(codepoint)))
            return false;
    }
    return true;
}

// The party widget's leader crown (skyrim_ui assets/images/group/crown.svg, a 26x26 box), drawn as
// convex pieces since ImGui 1.89 can't fill concave paths. Points are clockwise, as ImGui's AA fill expects.
void DrawPartyCrown(ImDrawList* apDrawList, ImVec2 aOrigin, float aSize, ImU32 aColor) noexcept
{
    const float s = aSize / 26.f;
    auto at = [&](float aX, float aY) { return ImVec2(aOrigin.x + aX * s, aOrigin.y + aY * s); };

    const ImVec2 centerSpike[] = {at(12.09f, 5.78f), at(13.91f, 5.78f), at(18.f, 14.f), at(8.f, 14.f)};
    const ImVec2 leftSpike[] = {at(2.41f, 10.97f), at(3.53f, 10.28f), at(8.f, 14.f), at(4.09f, 19.f)};
    const ImVec2 rightSpike[] = {at(21.91f, 19.f), at(18.f, 14.f), at(22.47f, 10.28f), at(23.59f, 10.97f)};
    const ImVec2 band[] = {at(8.f, 14.f), at(18.f, 14.f), at(21.91f, 19.f), at(4.09f, 19.f)};
    apDrawList->AddConvexPolyFilled(centerSpike, 4, aColor);
    apDrawList->AddConvexPolyFilled(leftSpike, 4, aColor);
    apDrawList->AddConvexPolyFilled(rightSpike, 4, aColor);
    apDrawList->AddConvexPolyFilled(band, 4, aColor);

    apDrawList->AddCircleFilled(at(13.f, 4.f), 2.f * s, aColor, 12);
    apDrawList->AddCircleFilled(at(2.f, 9.f), 2.f * s, aColor, 12);
    apDrawList->AddCircleFilled(at(24.f, 9.f), 2.f * s, aColor, 12);

    apDrawList->AddRectFilled(at(4.f, 21.f), at(22.f, 24.f), aColor, 1.5f * s);
}

// Text with a 1 px shadow; ImGui has no italic style, so italics shear the glyph quads around the baseline.
void DrawShadowedText(ImDrawList* apDrawList, ImFont* apFont, float aSize, ImVec2 aPos, const char* apBegin, const char* apEnd, ImU32 aColor, ImU32 aShadowColor, bool aItalic) noexcept
{
    const int firstVertex = apDrawList->VtxBuffer.Size;
    apDrawList->AddText(apFont, aSize, ImVec2(aPos.x + 1.f, aPos.y + 1.f), aShadowColor, apBegin, apEnd);
    apDrawList->AddText(apFont, aSize, aPos, aColor, apBegin, apEnd);

    if (aItalic)
    {
        const float baseline = aPos.y + aSize;
        for (int i = firstVertex; i < apDrawList->VtxBuffer.Size; ++i)
            apDrawList->VtxBuffer[i].pos.x += (baseline - apDrawList->VtxBuffer[i].pos.y) * NameTagService::kItalicShear;
    }
}
} // namespace

NameTagService::NameTagService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
{
    auto& imgui = m_world.ctx().at<ImguiService>();
    m_drawConnection = imgui.OnDraw.connect<&NameTagService::OnDraw>(this);
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&NameTagService::OnUpdate>(this);
}

void NameTagService::OnUpdate(const UpdateEvent&) noexcept
{
    auto* pLocalPlayer = PlayerCharacter::Get();
    if (!pLocalPlayer)
    {
        m_visibility.clear();
        return;
    }

    const uint64_t tick = m_world.GetTick();

    auto view = m_world.view<PlayerComponent, RemoteComponent, FormIdComponent>();
    for (auto entity : view)
    {
        const auto& playerComponent = view.get<PlayerComponent>(entity);
        const auto& formComponent = view.get<FormIdComponent>(entity);

        Actor* pActor = Cast<Actor>(TESForm::GetById(formComponent.Id));
        if (!pActor)
            continue;

        const float dx = pLocalPlayer->position.x - pActor->position.x;
        const float dy = pLocalPlayer->position.y - pActor->position.y;
        const float dz = pLocalPlayer->position.z - pActor->position.z;
        const float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

        bool isVisible = !pActor->IsDead();
        if (!isVisible && distance <= kNearBypassDistance)
            isVisible = true;

        VisibilityInfo& info = m_visibility[playerComponent.Id];
        info.Visible = isVisible;
        info.Tick = tick;
    }

    constexpr uint64_t kMaxAgeMs = 2000;
    for (auto it = m_visibility.begin(); it != m_visibility.end();)
    {
        if (tick - it->second.Tick > kMaxAgeMs)
            it = m_visibility.erase(it);
        else
            ++it;
    }
}

void NameTagService::SetSettings(Settings aSettings) noexcept
{
    aSettings.NameFadeStart = std::max(aSettings.NameFadeStart, 0.f);
    aSettings.NameInvisible = std::max(aSettings.NameInvisible, aSettings.NameFadeStart + 1.f);
    aSettings.PartyFadeStart = std::max(aSettings.PartyFadeStart, 0.f);
    aSettings.PartyInvisible = std::max(aSettings.PartyInvisible, aSettings.PartyFadeStart + 1.f);
    aSettings.FontSize = std::clamp(aSettings.FontSize, 8.f, 48.f);

    m_settings = std::move(aSettings);
}

void NameTagService::OnDraw() noexcept
{
    if (m_settings.NameTags == Filter::kOff)
        return;

    auto* pLocalPlayer = PlayerCharacter::Get();
    if (!pLocalPlayer)
        return;

    // Don't draw nametags while menus that cover the screen are open.
    if (auto* pUI = UI::Get())
    {
        auto menuOpen = [pUI](const char* acName) {
            return acName && pUI->GetMenuOpen(BSFixedString(acName));
        };

        if (menuOpen("Dialogue Menu") || menuOpen("InventoryMenu") || menuOpen("StatsMenu") ||
            menuOpen("SkillsMenu") || menuOpen("MagicMenu") || menuOpen("MapMenu") || menuOpen("TweenMenu") ||
            menuOpen("FavoritesMenu"))
        {
            return;
        }
    }

    auto view = m_world.view<PlayerComponent, RemoteComponent, FormIdComponent>();
    if (view.begin() == view.end())
        return;

    const auto& partyService = m_world.GetPartyService();
    const auto& partyMembers = partyService.GetPartyMembers();

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    if (!drawList)
        return;

    const ImVec2 viewport = ImGui::GetIO().DisplaySize;
    const float uiScale = viewport.y / 1080.f;
    ImFont* font = m_world.ctx().at<ImguiService>().GetNameTagFont();
    if (!font)
        font = ImGui::GetFont();

    // The label comes translated from the UI; fall back to English when the
    // tag font lacks its glyphs (e.g. CJK).
    const char* strangerLabel = m_settings.StrangerLabel.c_str();
    if (!HasAllGlyphs(font, m_settings.StrangerLabel))
        strangerLabel = "Stranger";

    const glm::vec3 localPos = {pLocalPlayer->position.x, pLocalPlayer->position.y, pLocalPlayer->position.z};

    // The HUD's activate prompt shows over the player the crosshair is on, so their tag ducks.
    const CrosshairPickData* pCrosshair = CrosshairPickData::Get();
    const uint32_t crosshairTarget = pCrosshair ? pCrosshair->Target : 0;

    for (auto entity : view)
    {
        const auto& playerComponent = view.get<PlayerComponent>(entity);
        const auto& formComponent = view.get<FormIdComponent>(entity);

        Actor* pActor = Cast<Actor>(TESForm::GetById(formComponent.Id));
        if (!pActor)
            continue;

        const ActorExtension* pExtension = pActor->GetExtension();
        if (!pExtension || pExtension->IsLocalPlayer())
            continue;

        const bool isPartyMember = std::find(partyMembers.begin(), partyMembers.end(), playerComponent.Id) != partyMembers.end();
        if (!IsEnabledFor(m_settings.NameTags, isPartyMember))
            continue;

        if (!ShouldRenderTag(playerComponent.Id))
            continue;

        if (crosshairTarget != 0 && pActor->GetHandle().handle.iBits == crosshairTarget)
            continue;

        // Names are cached for the session, so a player dropped from the player list (e.g. after a kick)
        // keeps their name. Without one, the stranger label is shown; never a player id.
        bool isNameRevealed = m_settings.RevealedPlayerIds.count(playerComponent.Id) > 0;
        std::string displayName = strangerLabel;
        if (isNameRevealed)
        {
            if (const String* pName = partyService.GetKnownName(playerComponent.Id))
                displayName = pName->c_str();
            else
                isNameRevealed = false;
        }

        const bool isLeader = isPartyMember && partyService.GetLeaderPlayerId() == playerComponent.Id;

        const NiPoint3 headTop = BuildHeadTopPoint(pActor);

        ImVec2 headScreen;
        float depth = 0.f;
        if (!ProjectWorldPoint(headTop, headScreen, depth, viewport))
            continue;

        const float fadeStart = isPartyMember ? m_settings.PartyFadeStart : m_settings.NameFadeStart;
        const float invisible = isPartyMember ? m_settings.PartyInvisible : m_settings.NameInvisible;

        const float distance = glm::distance(localPos, glm::vec3{headTop.x, headTop.y, headTop.z});
        if (distance > invisible)
            continue;

        float alpha = 1.f;
        if (distance > fadeStart)
        {
            const float fadeT = Clamp01((distance - fadeStart) / (invisible - fadeStart));
            alpha = 1.f - SmoothStep(0.f, 1.f, fadeT);
        }
        alpha *= Clamp01(1.f - std::max(depth - 0.95f, 0.f) * 4.f);
        if (alpha <= 0.01f)
            continue;

        // Helmet clearance: a world-space gap, clamped on screen so close tags don't float and far ones don't sit on the head.
        const float heightScale = std::clamp(pActor->GetHeight() / 128.f, 0.5f, 2.f);
        NiPoint3 clearancePoint = headTop;
        clearancePoint.z += kHelmetClearance * heightScale;
        float gap = kMaxGapPx * uiScale;
        ImVec2 clearanceScreen;
        float clearanceDepth = 0.f;
        if (ProjectWorldPoint(clearancePoint, clearanceScreen, clearanceDepth, viewport))
            gap = std::clamp(headScreen.y - clearanceScreen.y, kMinGapPx * uiScale, kMaxGapPx * uiScale);

        const float distanceScale = std::clamp(kScaleDistance / std::max(distance, 1.f), kMinScale, kMaxScale);
        const float fontSize = m_settings.FontSize * kFontSizeScale * uiScale * distanceScale;
        // Proportions below are tuned at the 22 px base size.
        const float k = fontSize / 22.f;

        const ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.f, displayName.c_str(), displayName.c_str() + displayName.size());
        const float crownWidth = isLeader ? fontSize * 0.8f : 0.f;
        const float crownGap = isLeader ? fontSize * 0.25f : 0.f;
        const float paddingX = 5.f * k;
        const float paddingY = 2.f * k;
        const float contentWidth = crownWidth + crownGap + textSize.x;

        const ImVec2 bottomRight(headScreen.x + contentWidth * 0.5f + paddingX, headScreen.y - gap);
        const ImVec2 topLeft(headScreen.x - contentWidth * 0.5f - paddingX, bottomRight.y - textSize.y - paddingY * 2.f);

        // A thin box with no border, so it reads as a backdrop rather than a plate.
        if (IsEnabledFor(m_settings.Backdrop, isPartyMember))
            drawList->AddRectFilled(topLeft, bottomRight, ColorWithAlpha(ImVec4(0.f, 0.f, 0.f, 0.30f), alpha));

        const ImU32 shadowColor = ColorWithAlpha(ImVec4(0.f, 0.f, 0.f, 0.8f), alpha);
        const ImVec2 contentPos(topLeft.x + paddingX, topLeft.y + paddingY);

        if (isLeader)
        {
            // Same icon as the party widget, a square crownWidth wide, centred on the text.
            const ImVec2 origin(contentPos.x, contentPos.y + (textSize.y - crownWidth) * 0.5f);
            DrawPartyCrown(drawList, ImVec2(origin.x + 1.f, origin.y + 1.f), crownWidth, shadowColor);
            DrawPartyCrown(drawList, origin, crownWidth, ColorWithAlpha(ImVec4(0.95f, 0.95f, 0.95f, 1.f), alpha));
        }

        const ImVec2 namePos(contentPos.x + crownWidth + crownGap, contentPos.y);
        const ImVec4 nameColor = isNameRevealed ? ImVec4(0.95f, 0.95f, 0.95f, 1.f) : ImVec4(0.78f, 0.78f, 0.78f, 1.f);

        DrawShadowedText(drawList, font, fontSize, namePos, displayName.c_str(), displayName.c_str() + displayName.size(), ColorWithAlpha(nameColor, alpha), shadowColor, !isNameRevealed);
    }
}

bool NameTagService::ShouldRenderTag(uint32_t aPlayerId) const noexcept
{
    const uint64_t tick = m_world.GetTick();
    auto it = m_visibility.find(aPlayerId);
    if (it == m_visibility.end())
        return true;

    constexpr uint64_t kStaleThresholdMs = 1500;
    if (tick - it->second.Tick > kStaleThresholdMs)
        return true;

    return it->second.Visible;
}

bool NameTagService::ProjectWorldPoint(const NiPoint3& aWorldPoint, ImVec2& aScreenPos, float& aDepth, const ImVec2& aViewport) const noexcept
{
    NiPoint3 projected{};
    if (!HUDMenuUtils::WorldPtToScreenPt3(aWorldPoint, projected))
        return false;

    if (projected.z <= kVisibilityEpsilon || projected.z >= 1.f)
        return false;

    aScreenPos.x = projected.x * aViewport.x;
    aScreenPos.y = (1.f - projected.y) * aViewport.y;
    aDepth = projected.z;

    return true;
}

NiPoint3 NameTagService::BuildHeadTopPoint(Actor* apActor) const noexcept
{
    NiPoint3 point = apActor->position;
    const float height = apActor->GetHeight();

    // The neck bone follows crouching, sitting and lying down, and stays on the body when a
    // dismemberment mod detaches the head. Never freed: the engine's string cache outlives us.
    static BSFixedString* s_pNeckNodeName = new BSFixedString("NPC Neck [Neck]");
    if (NiNode* pRoot = apActor->GetNiNode())
    {
        if (NiAVObject* pNeck = pRoot->GetByName(*s_pNeckNodeName))
        {
            point = pNeck->worldTranslate;
            point.z += kHeadTopAboveNeck * std::clamp(height / 128.f, 0.5f, 2.f);
            return point;
        }
    }

    // No 3D yet: the top of the bounding box.
    point.z += height;
    return point;
}

bool NameTagService::IsEnabledFor(Filter aFilter, bool aIsPartyMember) noexcept
{
    return aFilter == Filter::kOn || (aFilter == Filter::kParty && aIsPartyMember);
}

ImU32 NameTagService::ColorWithAlpha(const ImVec4& aColor, float aAlpha) noexcept
{
    ImVec4 adjusted = aColor;
    adjusted.w *= Clamp01(aAlpha);
    return ImGui::ColorConvertFloat4ToU32(adjusted);
}
