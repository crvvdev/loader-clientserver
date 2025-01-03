#pragma once

namespace ctrl
{
class cCheckBox : public cControl
{
  public:
    cCheckBox(HWND hwnd = NULL) : cControl(hwnd)
    {
    }

    std::wstring Text()
    {
        const int length = GetWindowTextLength(_hwnd);
        std::wstring text(length, L'\0');
        GetWindowText(_hwnd, &text[0], length + 1);
        return text;
    }

    BOOL Text(const std::wstring &text)
    {
        return SetWindowText(_hwnd, text.c_str());
    }

    void Check()
    {
        SendMessage(BM_SETCHECK, BST_CHECKED, 0);
    }

    void Uncheck()
    {
        SendMessage(BM_SETCHECK, BST_UNCHECKED, 0);
    }

    BOOL IsChecked()
    {
        return SendMessage(BM_GETCHECK, 0, 0) == BST_CHECKED;
    }
};
} // namespace ctrl
