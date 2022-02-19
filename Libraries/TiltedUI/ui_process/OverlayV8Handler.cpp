#include <OverlayV8Handler.h>
#include <TiltedCore/Platform.hpp>

namespace TiltedPhoques
{
    OverlayV8Handler::OverlayV8Handler(const CefRefPtr<CefBrowser>& apBrowser) noexcept
        : m_pBrowser(apBrowser)
    {
    }

    bool OverlayV8Handler::Execute(const CefString& acName, CefRefPtr<CefV8Value> apObject, const CefV8ValueList& acArguments, CefRefPtr<CefV8Value>& aReturnValue, CefString& aException)
    {
        TP_UNUSED(apObject);
        TP_UNUSED(aReturnValue);
        TP_UNUSED(aException);

        Dispatch(acName, acArguments);

        return false;
    }

    void OverlayV8Handler::Dispatch(const CefString& acName, const CefV8ValueList& acArguments) const noexcept
    {
        auto pMessage = CefProcessMessage::Create("ui-event");

        auto pArguments = pMessage->GetArgumentList();
        auto pArgumentsList = CefListValue::Create();

        for (size_t i = 0; i < acArguments.size(); ++i)
        {
            pArgumentsList->SetValue(static_cast<int32_t>(i), ConvertValue(acArguments[i]));
        }

        pArguments->SetString(0, acName);
        pArguments->SetList(1, pArgumentsList);

        m_pBrowser->GetMainFrame()->SendProcessMessage(PID_BROWSER, pMessage);
    }

    CefRefPtr<CefValue> OverlayV8Handler::ConvertValue(const CefRefPtr<CefV8Value>& acpValue)
    {
        auto pValue = CefValue::Create();

        if (acpValue->IsBool())
        {
            pValue->SetBool(acpValue->GetBoolValue());
        }
        else if (acpValue->IsInt())
        {
            pValue->SetInt(acpValue->GetIntValue());
        }
        else if (acpValue->IsDouble())
        {
            pValue->SetDouble(acpValue->GetDoubleValue());
        }
        else if (acpValue->IsNull())
        {
            pValue->SetNull();
        }
        else if (acpValue->IsString())
        {
            pValue->SetString(acpValue->GetStringValue());
        }
        else if (acpValue->IsArray())
        {
            auto pList = CefListValue::Create();

            for (int i = 0; i < acpValue->GetArrayLength(); ++i)
            {
                pList->SetValue(i, ConvertValue(acpValue->GetValue(i)));
            }

            pValue->SetList(pList);
        }
        else if (acpValue->IsObject())
        {
            auto pDict = CefDictionaryValue::Create();

            std::vector<CefString> keys;
            acpValue->GetKeys(keys);

            for (const auto& key : keys)
            {
                pDict->SetValue(key, ConvertValue(acpValue->GetValue(key)));
            }

            pValue->SetDictionary(pDict);
        }

        return pValue;
    }
}
