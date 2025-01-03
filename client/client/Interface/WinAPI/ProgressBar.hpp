#pragma once

namespace ctrl
{
class cProgressBar : public cControl
{
  public:
    cProgressBar(HWND hwnd = NULL) : cControl(hwnd)
    {
    }

    LRESULT SetMarque(bool marque)
    {
        return SendMessage(PBM_SETMARQUEE, marque, 30);
    }

    LRESULT SetRange(int minimum, int maximum)
    {
        return SendMessage(PBM_SETRANGE, 0, MAKELONG(minimum, maximum));
    }

    LRESULT SetPos(int pos)
    {
        return SendMessage(PBM_SETPOS, pos, 0);
    }
};
} // namespace ctrl