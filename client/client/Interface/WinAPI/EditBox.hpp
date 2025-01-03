#pragma once

namespace ctrl
{
class cEditBox : public cControl
{
  public:
    cEditBox(HWND hwnd = NULL) : cControl(hwnd)
    {
    }

    long Integer()
    {
        return std::stol(Text());
    }

    std::wstring Text()
    {
        wchar_t buf[512] = {NULL};
        Edit_GetText(_hwnd, buf, ARRAYSIZE(buf));

        return buf;
    }

    BOOL Text(const std::wstring &text)
    {
        return Edit_SetText(_hwnd, text.c_str());
    }

    BOOL Reset()
    {
        return Edit_SetText(_hwnd, L"");
    }
};
} // namespace ctrl
