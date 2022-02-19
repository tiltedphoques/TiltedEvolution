#include <OverlayRenderProcessHandler.hpp>
#include <TiltedCore/Platform.hpp>

namespace TiltedPhoques
{
    OverlayRenderProcessHandler::OverlayRenderProcessHandler(std::string aCoreObjectName /* = "core" */) noexcept
        : m_coreObjectName(std::move(aCoreObjectName))
    {
    }

    void OverlayRenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
    {
        TP_UNUSED(frame);

        auto pObject = context->GetGlobal();
        m_pCoreObject = CefV8Value::CreateObject(nullptr, nullptr);

        m_pEventsHandler = new EventsV8Handler(context);
        m_pOverlayHandler = new OverlayV8Handler(browser);

        pObject->SetValue(m_coreObjectName, m_pCoreObject, V8_PROPERTY_ATTRIBUTE_NONE);
    }

    void OverlayRenderProcessHandler::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
    {
        TP_UNUSED(browser);
        TP_UNUSED(frame);

        context->GetGlobal()->DeleteValue(m_coreObjectName);

        m_pCoreObject = nullptr;
    }

    bool OverlayRenderProcessHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId sourceProcess,
        CefRefPtr<CefProcessMessage> message)
    {
        TP_UNUSED(frame);
        TP_UNUSED(sourceProcess);

        if (message->GetName() == "browser-event")
        {
            const auto pArguments = message->GetArgumentList();

            const auto functionName = pArguments->GetString(0).ToString();
            const auto functionArgs = pArguments->GetList(1);

            for (const auto& callback : m_pEventsHandler->GetCallbacks())
            {
                if (callback.first == functionName && callback.second.first->GetBrowser()->IsSame(browser))
                {
                    callback.second.first->Enter();

                    CefV8ValueList v8Arguments;

                    for (int i = 0; i < functionArgs->GetSize(); ++i)
                    {
                        v8Arguments.push_back(ConvertValue(functionArgs->GetValue(i)));
                    }

                    callback.second.second->ExecuteFunction(nullptr, v8Arguments);
                    callback.second.first->Exit();
                }
            }

            return true;
        }

        return false;
    }

    CefRefPtr<CefV8Value> OverlayRenderProcessHandler::ConvertValue(const CefRefPtr<CefValue>& apValue)
    {
        switch (apValue->GetType())
        {
        case VTYPE_NULL:
        {
            return CefV8Value::CreateNull();
        }
        case VTYPE_BOOL:
        {
            return CefV8Value::CreateBool(apValue->GetBool());
        }
        case VTYPE_DOUBLE:
        {
            return CefV8Value::CreateDouble(apValue->GetDouble());
        }
        case VTYPE_INT:
        {
            return CefV8Value::CreateInt(apValue->GetInt());
        }
        case VTYPE_STRING:
        {
            return CefV8Value::CreateString(apValue->GetString());
        }
        case VTYPE_LIST:
        {
            auto pOriginalList = apValue->GetList();
            auto pConvertedList = CefV8Value::CreateArray(static_cast<int>(pOriginalList->GetSize()));

            for (size_t i = 0; i < pOriginalList->GetSize(); ++i)
            {
                pConvertedList->SetValue(static_cast<int>(i), ConvertValue(pOriginalList->GetValue(static_cast<int>(i))));
            }

            return pConvertedList;
        }
        case VTYPE_DICTIONARY:
        {
            auto pOriginalDict = apValue->GetDictionary();
            auto pConvertedDict = CefV8Value::CreateObject(nullptr, nullptr);

            std::vector<CefString> keys;
            pOriginalDict->GetKeys(keys);

            for (const auto& key : keys)
            {
                pConvertedDict->SetValue(key, ConvertValue(pOriginalDict->GetValue(key)), V8_PROPERTY_ATTRIBUTE_NONE);
            }

            return pConvertedDict;
        }
        default:
            break;
        }

        return CefV8Value::CreateUndefined();
    }

    void OverlayRenderProcessHandler::CreateEvent(const std::string& acEventName) const
    {
        m_pCoreObject->SetValue(acEventName, CefV8Value::CreateFunction(acEventName, m_pEventsHandler), V8_PROPERTY_ATTRIBUTE_NONE);
    }

    void OverlayRenderProcessHandler::CreateFunction(const std::string& acFunctionName) const
    {
        m_pCoreObject->SetValue(acFunctionName, CefV8Value::CreateFunction(acFunctionName, m_pOverlayHandler), V8_PROPERTY_ATTRIBUTE_NONE);
    }
}
