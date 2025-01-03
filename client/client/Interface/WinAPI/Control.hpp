#pragma once

namespace ctrl
{
class cControl : public cWindow
{
  public:
    cControl(HWND hwnd = NULL) : cWindow(hwnd)
    {
    }
};
} // namespace ctrl