#pragma once

#define WND_HANDLER(n) LRESULT n(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)

class cWindow
{
  public:
    using fnWndProc = std::function<LRESULT(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)>;
    using mapWndProc = std::map<UINT, std::pair<cWindow *, fnWndProc>>;

    cWindow(HWND hwnd = NULL) : _hwnd(hwnd), _subThunk(&cWindow::SubProc, this)
    {
    }

    void Attach(HWND hwnd)
    {
        _hwnd = hwnd;
    }

    void Attach(HWND hDlg, UINT id)
    {
        _id = id;
        _hwnd = GetDlgItem(hDlg, id);
    }

    HWND GetHwnd()
    {
        return _hwnd;
    }

    UINT GetId()
    {
        return _id;
    }

    WNDPROC GetOldWndProc()
    {
        return _oldProc;
    }

    void SetFocus()
    {
        ::SetFocus(_hwnd);
    }

    void Enable()
    {
        EnableWindow(_hwnd, TRUE);
    }

    void Disable()
    {
        EnableWindow(_hwnd, FALSE);
    }

    std::wstring Text()
    {
        wchar_t buf[512] = {NULL};
        GetWindowText(_hwnd, buf, ARRAYSIZE(buf));
        return buf;
    }

    BOOL Text(const std::wstring &text)
    {
        return SetWindowText(_hwnd, text.c_str());
    }

    LRESULT SendMessage(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return ::SendMessage(_hwnd, msg, wParam, lParam);
    }

    void SubClass(UINT message, fnWndProc handler, cWindow *instance = nullptr)
    {
        if (handler == nullptr)
        {
            if (_subMessages.count(message))
            {
                _subMessages.erase(message);

                if (_subMessages.empty())
                {
                    SetWindowLongPtr(_hwnd, GWLP_WNDPROC, reinterpret_cast<LRESULT>(_oldProc));
                    _oldProc = nullptr;
                }
                return;
            }
        }
        else
        {
            _subMessages[message] = std::make_pair(instance ? instance : this, handler);
            if (!_oldProc)
            {
                _oldProc = reinterpret_cast<WNDPROC>(
                    SetWindowLongPtr(_hwnd, GWLP_WNDPROC, reinterpret_cast<LRESULT>(_subThunk.GetThunk())));
            }
        }
    }

  private:
    LRESULT SubProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (_subMessages.count(message))
        {
            return this->_subMessages[message].first->_subMessages[message].second(hwnd, message, wParam, lParam);
        }
        return CallWindowProc(_oldProc, hwnd, message, wParam, lParam);
    }

  protected:
    HWND _hwnd = NULL;
    UINT _id = 0;
    WNDPROC _oldProc = nullptr;
    mapWndProc _subMessages;
    cWin32Thunk<WNDPROC, cWindow> _subThunk;
};