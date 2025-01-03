#pragma once

#include <Richedit.h>

namespace ctrl
{
class cRichEditControl : public cControl
{
  public:
    cRichEditControl(HWND hwnd = NULL) : cControl(hwnd)
    {
        LoadLibrary(L"riched20.dll");
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