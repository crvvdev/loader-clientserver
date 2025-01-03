#include "Includes.hpp"
#include "Interface\Login\LoginDlg.h"
#include "Interface\Main\MainDlg.h"

void ClientNetworkThread()
{
    try
    {
        g_WebSocketClient->Run("ws://localhost:50000");
    }
    catch ([[maybe_unused]] std::exception &e)
    {
#if _DEBUG
        std::cout << "Exception: " << e.what() << std::endl;
#endif

        g_EventManager->NotifyEvent(CEventManager::EventType::ConnectError,
                                    new EventParam("Failed to connect to server!"));
    }
}

int main(int argc, char *argv[])
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    // Initialize Win32 application
    //
    const HINSTANCE hInst = GetModuleHandleA(nullptr);
    InitCommonControls();

#if _DEBUG
    std::cout << security::GetHardwareInfo() << std::endl;
    std::cout << security::GetHardwareId() << std::endl;
#endif

    // Networking thread runs in separated detached thread
    //
    std::thread(&ClientNetworkThread).detach();

    cLoginDlg loginDlg(hInst);

    loginDlg.RunModeless();

    if (!loginDlg.IsLoggedIn())
    {
        exit(EXIT_FAILURE);
    }

    // Run the main dialog
    //
    cMainDlg mainDlg(hInst);
    return (INT)mainDlg.RunModeless();
}