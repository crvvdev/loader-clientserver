#pragma once

class cMainDlg : public cDialog
{
  public:
    cMainDlg(HINSTANCE instance);
    ~cMainDlg();

  private:
    DLG_HANDLER(OnInit);
    DLG_HANDLER(OnClose);

  protected:
    ctrl::cButton _buttonLoad{};
    ctrl::cProgressBar _injectionState{};
    ctrl::cStatusBar _info{};
    ctrl::cRichEditControl _editCheatInformation1{};
    ctrl::cRichEditControl _editCheatInformation2{};

    HINSTANCE _instance = nullptr;
};