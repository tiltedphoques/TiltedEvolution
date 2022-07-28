#pragma once

#ifdef _WIN32

#include <Windows.h>
#include <optional>
#include <string>
#include <vector>

namespace Base
{
class TaskDialog final
{
  public:
    explicit TaskDialog(HICON aIcon, const wchar_t* apTitle, const wchar_t* apMessage,
                        const wchar_t* apMoreContext, const wchar_t* apOptionalDetails);
    ~TaskDialog();

    TaskDialog(const TaskDialog&) = delete;
    TaskDialog& operator=(const TaskDialog&) = delete;

    void SetDefaultButton(int aDefaultButton)
    {
        m_defaultButton = aDefaultButton;
    }

    void AppendButton(int aButtonId, const wchar_t* apButtonText);

    int Show(int aIconID = 102);

  private:
    static HRESULT CALLBACK TaskDialogCallbackProc(HWND hwnd, UINT notification, WPARAM w_param, LPARAM l_param,
                                                   LONG_PTR ref_data);

    HICON m_iconHandle;
    const wchar_t* m_pWindowTitle;
    const wchar_t* m_pMessage;
    const wchar_t* m_pDetails;
    const wchar_t* m_pContext;
    int m_defaultButton = -1;
    bool m_inFocus = true;
    std::vector<std::pair<int, std::wstring>> m_buttonList;
};
} // namespace Base

#endif
