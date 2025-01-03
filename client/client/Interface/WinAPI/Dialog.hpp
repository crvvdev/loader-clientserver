#pragma once

#define DLG_HANDLER(n) INT_PTR n(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)

class cDialog : public cWindow
{
  public:
    using fnDlgProc = std::function<INT_PTR(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)>;

    using mapMsgProc = std::map<UINT, fnDlgProc>;
    using mapCtrlProc = std::map<WORD, fnDlgProc>;
    using mapNotifyProc = std::map<UINT_PTR, fnDlgProc>;
    using mapClientEventProc = std::map<CEventManager::EventType, CEventManager::fnEventHandler>;

    cDialog(int dialogID) : _dialogID(dialogID)
    {
        _messages[WM_INITDIALOG] = std::bind(&cDialog::OnInit, this, std::placeholders::_1, std::placeholders::_2,
                                             std::placeholders::_3, std::placeholders::_4);
        _messages[WM_COMMAND] = std::bind(&cDialog::OnCommand, this, std::placeholders::_1, std::placeholders::_2,
                                          std::placeholders::_3, std::placeholders::_4);
        _messages[WM_CLOSE] = std::bind(&cDialog::OnClose, this, std::placeholders::_1, std::placeholders::_2,
                                        std::placeholders::_3, std::placeholders::_4);
        _messages[WM_NOTIFY] = std::bind(&cDialog::OnNotify, this, std::placeholders::_1, std::placeholders::_2,
                                         std::placeholders::_3, std::placeholders::_4);
        _messages[WM_CLIENT_EVENT] = std::bind(&cDialog::OnClientEvent, this, std::placeholders::_1,
                                               std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

        _clientEvents[CEventManager::EventType::ConnectError] =
            std::bind(&cDialog::OnConnectError, this, std::placeholders::_1);
        _clientEvents[CEventManager::EventType::ParseMessageError] =
            std::bind(&cDialog::OnParseMessageError, this, std::placeholders::_1);
    }

    INT_PTR RunModal(HWND parent = NULL)
    {
        cWin32Thunk<DLGPROC, cDialog> dlgProc(&cDialog::DlgProc, this);
        return DialogBox(NULL, MAKEINTRESOURCE(_dialogID), parent, dlgProc.GetThunk());
    }

    INT_PTR RunModeless(HWND parent = NULL, int accelID = NULL)
    {
        MSG msg = {NULL};
        HACCEL hAccel = LoadAccelerators(NULL, MAKEINTRESOURCE(accelID));
        _modeless = true;

        cWin32Thunk<DLGPROC, cDialog> dlgProc(&cDialog::DlgProc, this);

        _hwnd = CreateDialog(NULL, MAKEINTRESOURCE(_dialogID), parent, dlgProc.GetThunk());
        assert(_hwnd && "Unable to RunModelesss - CreateDialog failed!");

        ShowWindow(_hwnd, SW_SHOW);

        while (IsWindow(_hwnd) && GetMessage(&msg, NULL, NULL, NULL) > 0)
        {
            if (TranslateAccelerator(_hwnd, hAccel, &msg))
                continue;

            if (!IsDialogMessage(_hwnd, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        return msg.wParam;
    }

  protected:
    INT_PTR CloseDialog()
    {
        BOOL result;

        if (_modeless)
        {
            result = DestroyWindow(_hwnd);
        }
        else
        {
            result = EndDialog(_hwnd, EXIT_SUCCESS);
        }

        _hwnd = NULL;

        return result;
    }

    INT_PTR DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (_messages.count(message))
        {
            auto res = _messages[message](hDlg, message, wParam, lParam);
            return res;
        }
        return FALSE;
    }

    INT_PTR OnInit(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(message);
        UNREFERENCED_PARAMETER(wParam);
        UNREFERENCED_PARAMETER(lParam);

        _hwnd = hDlg;
        ActiveWindowManager::Instance().SetActiveWindow(GetHwnd());

        return TRUE;
    }

    INT_PTR OnCommand(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (_events.count(LOWORD(wParam)))
        {
            return _events[LOWORD(wParam)](hDlg, message, wParam, lParam);
        }

        if (_events.count(HIWORD(wParam)))
        {
            return _events[HIWORD(wParam)](hDlg, message, wParam, lParam);
        }

        return FALSE;
    }

    INT_PTR OnNotify(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(hDlg);
        UNREFERENCED_PARAMETER(message);
        UNREFERENCED_PARAMETER(wParam);

        LPNMHDR pnmh = reinterpret_cast<LPNMHDR>(lParam);

        if (_notifications.count(pnmh->idFrom))
        {
            return _notifications[pnmh->idFrom](hDlg, message, wParam, lParam);
        }

        return FALSE;
    }

    INT_PTR OnClientEvent(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(hDlg);
        UNREFERENCED_PARAMETER(message);
        UNREFERENCED_PARAMETER(wParam);

        if (lParam)
        {
            auto *event = reinterpret_cast<CEventManager::Event *>(lParam);

            if (_clientEvents.count(event->type))
            {
                this->_clientEvents[event->type](event->param);
            }

            if (event->param)
            {
                delete event->param;
            }

            delete event;
        }

        return TRUE;
    }

    INT_PTR OnClose(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(hDlg);
        UNREFERENCED_PARAMETER(message);
        UNREFERENCED_PARAMETER(wParam);
        UNREFERENCED_PARAMETER(lParam);

        ActiveWindowManager::Instance().ResetWindow();

        return CloseDialog();
    }

    void OnConnectError(void *param)
    {
        assert(param);
        [[maybe_unused]] auto eventParam = reinterpret_cast<EventParam *>(param);

        this->Disable();
        cMessage::ShowError(GetHwnd(),
                            VMProtectDecryptStringW("Failed to connect to server ")
#ifdef _DEBUG
                                + utils::StringToWString(eventParam->error)
#endif
                                ,
                            15 * 1000, true);
    }

    void OnParseMessageError(void *param)
    {
        assert(param);
        [[maybe_unused]] auto eventParam = reinterpret_cast<EventParam *>(param);

        this->Disable();
        cMessage::ShowError(GetHwnd(),
                            VMProtectDecryptStringW("Failed to parse message ")
#ifdef _DEBUG
                                + utils::StringToWString(eventParam->error)
#endif
                                ,
                            15 * 1000, true);
    }

    mapMsgProc _messages;
    mapCtrlProc _events;
    mapNotifyProc _notifications;
    mapClientEventProc _clientEvents;
    int _dialogID;
    bool _modeless = false;
};
