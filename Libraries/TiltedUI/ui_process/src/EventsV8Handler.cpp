#include <EventsV8Handler.hpp>

namespace TiltedPhoques
{
    EventsV8Handler::EventsV8Handler(const CefRefPtr<CefV8Context> apContext) noexcept
        : m_pContext(apContext)
    {
    }

    bool EventsV8Handler::Execute(const CefString& acName, CefRefPtr<CefV8Value> apObject, const CefV8ValueList& acArguments, CefRefPtr<CefV8Value>& aReturnValue, CefString& aException)
    {
        if (acName == "on")
        {
            if (acArguments.size() < 2)
            {
                aException = "Invalid number of arguments";
            }
            else if (!acArguments[0]->IsString() || !acArguments[1]->IsFunction())
            {
                aException = "Invalid arguments";
            }
            else
            {
                m_callbacks.emplace(std::make_pair(acArguments[0]->GetStringValue().ToString(), std::make_pair(m_pContext, acArguments[1])));
            }

            return true;
        }
        else if (acName == "off")
        {
            if (acArguments.empty())
            {
                aException = "Invalid number of arguments";
            }
            else if (!acArguments[0]->IsString() || (acArguments.size() != 1 && !acArguments[1]->IsFunction()))
            {
                aException = "Invalid arguments";
            }
            else
            {
                for (auto it = m_callbacks.begin(); it != m_callbacks.end();)
                {
                    if (it->first == acArguments[0]->GetStringValue().ToString() && it->second.first->IsSame(m_pContext) && (acArguments.size() == 1 || it->second.second->IsSame(acArguments[1])))
                    {
                        it = m_callbacks.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }

            return true;
        }

        return false;
    }

    const TCallbacks& EventsV8Handler::GetCallbacks() const noexcept
    {
        return m_callbacks;
    }
}
