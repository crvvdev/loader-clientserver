#pragma once

namespace ctrl
{
class cStatusBar : public cControl
{
  public:
    cStatusBar(HWND hwnd = NULL) : cControl(hwnd)
    {
    }

    LRESULT SetParts(const std::initializer_list<int> &coords)
    {
        int i = 0;
        std::unique_ptr<int[]> coordArray(new int[coords.size()]());
        for (auto &item : coords)
        {
            coordArray[i] = item;
            i++;
        }

        return SendMessage( SB_SETPARTS, i, (LPARAM)coordArray.get());
    }

    LRESULT SetText(int index, const std::wstring &text)
    {
        return SendMessage(SB_SETTEXT, MAKEWORD(index, SBT_NOBORDERS), reinterpret_cast<LPARAM>(text.data()));
    }
};
} // namespace ctrl