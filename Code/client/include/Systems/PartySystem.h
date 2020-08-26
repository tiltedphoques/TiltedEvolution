#pragma once

struct ImguiService;

struct PartySystem
{
    PartySystem(entt::dispatcher& aDispatcher, ImguiService &aImguiService) noexcept;
    ~PartySystem() = default;

    TP_NOCOPYMOVE(PartySystem);

protected:

private:
    void OnDraw() noexcept;

    entt::scoped_connection m_drawConnection;
};
