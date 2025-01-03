#pragma once

#define WM_LOGIN_RESULT (WM_USER + 1)
#define WM_CLIENT_EVENT (WM_LOGIN_RESULT + 1)

class cMessage
{
  public:
    enum eMsgType
    {
        Error,
        Warning,
        Info,
        Question,
    };

    static void ShowError(HWND parent, const std::wstring &msg, const ULONG timeout = INFINITE,
                          bool exit_on_close = false, const std::wstring &title = L"Error")
    {

        Show(msg, title, Error, timeout, exit_on_close, parent);
    }

    static void ShowWarning(HWND parent, const std::wstring &msg, const ULONG timeout = INFINITE,
                            bool exit_on_close = false, const std::wstring &title = L"Warning")
    {
        Show(msg, title, Warning, timeout, exit_on_close, parent);
    }

    static void ShowInfo(HWND parent, const std::wstring &msg, const ULONG timeout = INFINITE,
                         bool exit_on_close = false, const std::wstring &title = L"Info")
    {
        Show(msg, title, Info, timeout, exit_on_close, parent);
    }

    static bool ShowQuestion(HWND parent, const std::wstring &msg, const ULONG timeout = INFINITE,
                             bool exit_on_close = false, const std::wstring &title = L"Question")
    {
        return Show(msg, title, Question, timeout, exit_on_close, parent) == IDYES;
    }

  private:
    static int Show(const std::wstring &msg, const std::wstring &title, const eMsgType type,
                    const ULONG timeout = INFINITE, bool exit_on_close = false, HWND parent = NULL)
    {
        using MessageBoxTimeout_ = int(WINAPI *)(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType,
                                                 IN WORD wLanguageId, IN DWORD dwMilliseconds);
        static auto MessageBoxTimeoutW = reinterpret_cast<MessageBoxTimeout_>(reinterpret_cast<void *>(
            GetProcAddress(LoadLibrary(VMProtectDecryptStringW("user32.dll")), "MessageBoxTimeoutW")));

        UINT uType = MB_OK;

        switch (type)
        {
        case Error:
            uType = MB_OK | MB_ICONERROR;
            break;

        case Warning:
            uType = MB_OK | MB_ICONWARNING;
            break;

        case Info:
            uType = MB_OK | MB_ICONINFORMATION;
            break;

        case Question:
            uType = MB_YESNO | MB_ICONQUESTION;
            break;

        default:
            break;
        }

        const int result = MessageBoxTimeoutW(parent, msg.c_str(), title.c_str(), uType, 0, timeout);

        if (exit_on_close)
        {
            ExitProcess(EXIT_SUCCESS);
        }

        return result;
    }
};