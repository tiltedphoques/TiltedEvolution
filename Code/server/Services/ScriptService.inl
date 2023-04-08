#include <sol/sol.hpp>

template <typename... Args>
std::tuple<bool, String> ScriptService::CallCancelableEvent(const String& acName, Args&&... args) noexcept
{
    m_eventCanceled = false;

    auto& callbacks = m_callbacks[acName];

    for (auto& callback : callbacks)
    {
        sol::protected_function pf{callback};
        auto result = pf(std::forward<Args>(args)...);

        if (!result.valid())
        {
            sol::error err = result;
            spdlog::error(err.what());
        }

        if (m_eventCanceled == true)
            return std::make_tuple(true, m_cancelReason);
    }

    return std::make_tuple(false, "");
}

template <typename... Args> void ScriptService::CallEvent(const String& acName, Args&&... args) noexcept
{
    auto& callbacks = m_callbacks[acName];

    for (auto& callback : callbacks)
    {
        sol::protected_function pf{callback};
        auto result = pf(std::forward<Args>(args)...);
        if (!result.valid())
        {
            sol::error err = result;
            spdlog::error(err.what());
        }
    }
}
