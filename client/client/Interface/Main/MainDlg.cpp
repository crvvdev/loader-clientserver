#include "..\..\Includes.hpp"

cMainDlg::cMainDlg(HINSTANCE instance) : cDialog(IDD_MAIN), _instance(instance)
{
    // setup event handlers
    _messages[WM_INITDIALOG] = std::bind(&cMainDlg::OnInit, this, std::placeholders::_1, std::placeholders::_2,
                                         std::placeholders::_3, std::placeholders::_4);
    _messages[WM_COMMAND] = std::bind(&cMainDlg::OnCommand, this, std::placeholders::_1, std::placeholders::_2,
                                      std::placeholders::_3, std::placeholders::_4);
    _messages[WM_CLOSE] = std::bind(&cMainDlg::OnClose, this, std::placeholders::_1, std::placeholders::_2,
                                    std::placeholders::_3, std::placeholders::_4);

    // setup events handlers
    _events[IDCLOSE] = std::bind(&cMainDlg::OnClose, this, std::placeholders::_1, std::placeholders::_2,
                                 std::placeholders::_3, std::placeholders::_4);
}

cMainDlg::~cMainDlg()
{
    // deconstructor
}

INT_PTR cMainDlg::OnInit(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    cDialog::OnInit(hDlg, message, wParam, lParam);

    // load our icon
    HICON hIcon = LoadIcon(_instance, MAKEINTRESOURCE(IDI_ICON));
    ::SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    ::SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    DeleteObject(hIcon);

    this->Text(VMProtectDecryptStringW(L"User panel"));

    // assign our control IDs to our control class objects
    _editCheatInformation1.Attach(_hwnd, IDC_EDIT_CHEAT_INFORMATION);
    _editCheatInformation2.Attach(_hwnd, IDC_EDIT_CHEAT_INFORMATION2);
    _buttonLoad.Attach(_hwnd, IDC_BUTTON_LOAD);
    _injectionState.Attach(_hwnd, IDC_PROGRESS_INJECTION);
    _info.Attach(CreateStatusWindow(WS_CHILD | WS_VISIBLE, L"", _hwnd, IDR_STATUS));

    // progress bar setup
    _injectionState.SetRange(0, 90);

    // edit check information box
    _editCheatInformation2.Text(VMProtectDecryptStringW(L"Expiration Date:\nCheat:\n"));

    wchar_t fmt[256]{};
    swprintf_s(fmt, VMProtectDecryptStringW(L"%S UTC\n%S"), g_userInformation.GetExpirationDate().c_str(),
               g_userInformation.GetGame().c_str());

    _editCheatInformation1.Text(fmt);

    // status bar setup
    _info.SetParts({-1});
    _info.SetText(0, VMProtectDecryptStringW(L"Idle."));

    return TRUE;
}

INT_PTR cMainDlg::OnClose(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    return cDialog::OnClose(hDlg, message, wParam, lParam);
}