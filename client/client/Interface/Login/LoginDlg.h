#pragma once

class cLoginDlg : public cDialog
{
  public:
    cLoginDlg(HINSTANCE instance);

    void LoginAsync(const std::wstring& userToken);

    bool IsLoggedIn() const
    {
        return this->_loggedIn;
    }

  private:
    DLG_HANDLER(OnInit);
    DLG_HANDLER(OnClose);
    DLG_HANDLER(OnOK);
    DLG_HANDLER(OnCancel);

    EVENT_CALLBACK_HANDLER(OnLoginSuccess);
    EVENT_CALLBACK_HANDLER(OnLoginFailed);
    EVENT_CALLBACK_HANDLER(OnConnectSuccess);
    EVENT_CALLBACK_HANDLER(OnVersionUpdate);
    EVENT_CALLBACK_HANDLER(OnVersionSuccess);
    EVENT_CALLBACK_HANDLER(OnVersionFailed);

  protected:
    ctrl::cEditBox _editUserToken{};
    ctrl::cButton _btnLogin{};
    bool _loggedIn = false;

    HINSTANCE _instance = nullptr;
};