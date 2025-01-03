#include "..\..\Includes.hpp"

cLoginDlg::cLoginDlg(HINSTANCE instance) : cDialog(IDD_LOGIN), _instance(instance)
{
    // setup message handlers
    _messages[WM_INITDIALOG] = std::bind(&cLoginDlg::OnInit, this, std::placeholders::_1, std::placeholders::_2,
                                         std::placeholders::_3, std::placeholders::_4);
    _messages[WM_COMMAND] = std::bind(&cLoginDlg::OnCommand, this, std::placeholders::_1, std::placeholders::_2,
                                      std::placeholders::_3, std::placeholders::_4);
    _messages[WM_CLOSE] = std::bind(&cLoginDlg::OnClose, this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4);

    // setup event handlers
    _events[IDCANCEL] = std::bind(&cLoginDlg::OnClose, this, std::placeholders::_1, std::placeholders::_2,
                                  std::placeholders::_3, std::placeholders::_4);
    _events[IDOK] = std::bind(&cLoginDlg::OnOK, this, std::placeholders::_1, std::placeholders::_2,
                              std::placeholders::_3, std::placeholders::_4);

    // network events handlers
    _clientEvents[CEventManager::EventType::ConnectSuccess] =
        std::bind(&cLoginDlg::OnConnectSuccess, this, std::placeholders::_1);

    _clientEvents[CEventManager::EventType::VersionUpdate] =
        std::bind(&cLoginDlg::OnVersionUpdate, this, std::placeholders::_1);

    _clientEvents[CEventManager::EventType::VersionFailed] =
        std::bind(&cLoginDlg::OnVersionFailed, this, std::placeholders::_1);

    _clientEvents[CEventManager::EventType::VersionSuccess] =
        std::bind(&cLoginDlg::OnVersionSuccess, this, std::placeholders::_1);

    _clientEvents[CEventManager::EventType::LoginSuccess] =
        std::bind(&cLoginDlg::OnLoginSuccess, this, std::placeholders::_1);

    _clientEvents[CEventManager::EventType::LoginFailed] =
        std::bind(&cLoginDlg::OnLoginFailed, this, std::placeholders::_1);
}

INT_PTR cLoginDlg::OnInit(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    cDialog::OnInit(hDlg, message, wParam, lParam);

    // load our icon
    HICON hIcon = LoadIcon(_instance, MAKEINTRESOURCE(IDI_ICON));
    ::SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    ::SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    DeleteObject(hIcon);

    // assign our control IDs to our control class objects
    Text(VMProtectDecryptStringW("Login"));

    _editUserToken.Attach(_hwnd, IDC_USERNAME);
    _editUserToken.Text(g_userInformation.GetUserToken());

    _btnLogin.Attach(_hwnd, IDOK);
    _btnLogin.Text(VMProtectDecryptStringW("Connecting..."));
    _btnLogin.Disable();

    return TRUE;
}

void cLoginDlg::OnConnectSuccess(void *)
{
    g_WebSocketClient->SendVersionMessageAsync();
}

void cLoginDlg::OnVersionUpdate(void *)
{
    cMessage::ShowInfo(GetHwnd(), L"An new update was found, the client will now restart to auto-update.", 15 * 1000,
                       true);
}

void cLoginDlg::OnVersionFailed(void *param)
{
    auto eventParam = reinterpret_cast<EventParam *>(param);

    cMessage::ShowInfo(GetHwnd(), utils::StringToWString(eventParam->error));
}

void cLoginDlg::OnVersionSuccess(void *)
{
    _btnLogin.Text(VMProtectDecryptStringW("Login"));
    _btnLogin.Enable();
}

void cLoginDlg::OnLoginSuccess(void *param)
{
    auto eventParam = reinterpret_cast<LoginEventParam *>(param);

    g_userInformation.UpdateLoginDetails(_editUserToken.Text());
    g_userInformation.SetExpirationDate(eventParam->expirationDate);
    g_userInformation.SetGame(eventParam->game);

    _loggedIn = true;
    CloseDialog();
}

void cLoginDlg::OnLoginFailed(void *param)
{
    auto eventParam = reinterpret_cast<EventParam *>(param);

    _btnLogin.Enable();
    cMessage::ShowError(GetHwnd(), utils::StringToWString(eventParam->error));
}

INT_PTR cLoginDlg::OnClose(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    // self-explanatory?
    return cDialog::OnClose(hDlg, message, wParam, lParam);
}

INT_PTR cLoginDlg::OnOK(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(hDlg);
    UNREFERENCED_PARAMETER(message);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    if (_editUserToken.Text().empty())
    {
        cMessage::ShowWarning(_hwnd, VMProtectDecryptStringW("User token cannot be empty."));
        return TRUE;
    }

    _btnLogin.Disable();
    LoginAsync(_editUserToken.Text());

    return TRUE;
}

void cLoginDlg::LoginAsync(const std::wstring &userToken)
{
    try
    {
        g_WebSocketClient->SendLoginMessageAsync(userToken);
    }
    catch (const std::exception &e)
    {
#if _DEBUG
        std::cerr << "Exception caught: " << e.what() << std::endl;
        assert(false);
#else
        UNREFERENCED_PARAMETER(e);
#endif
    }
}