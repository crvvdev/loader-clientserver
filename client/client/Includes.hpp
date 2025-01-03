#pragma once

// Common includes
//
#include <Windows.h>
#include <winternl.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <cstdint>
#include <cassert>
#include <string>
#include <iostream>
#include <ctime>
#include <thread>
#include <mutex>
#include <map>
#include <queue>
#include <memory>
#include <future>
#include <random>
#include <initializer_list>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <unordered_map>

#pragma warning(push)
#pragma warning(disable : 4267)
#pragma warning(disable : 4127)
#pragma warning(disable : 4244)
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <wmi.hpp>
#include <wmiclasses.hpp>
#include <nlohmann/json.hpp>
#include <scope_guard.hpp>
#pragma warning(pop)

#if _DEBUG
#define VMProtectDecryptStringA(X) X
#define VMProtectDecryptStringW(X) X

#define VMProtectGetCurrentHWID(x, y) 0
#else
#include "../thirdparty/vmp/VMProtectSDK.h"
#endif

// Interfaces
//
#include "Utils.hpp"
#include "WindowManager.hpp"
#include "UserInfo.hpp"
#include "Resources/Resource.h"
#include "Events/EventManager.hpp"
#include "Security/Encryption.h"
#include "Security/HardwareInfo.h"
#include "Networking/WebSocketClient.hpp"

// GUI controls
//
#include "Interface/WinAPI/Thunk.hpp"
#include "Interface/WinAPI/Message.hpp"
#include "Interface/WinAPI/Window.hpp"
#include "Interface/WinAPI/Control.hpp"
#include "Interface/WinAPI/Dialog.hpp"
#include "Interface/WinAPI/CheckBox.hpp"
#include "Interface/WinAPI/EditBox.hpp"
#include "Interface/WinAPI/RichEditControl.hpp"
#include "Interface/WinAPI/Button.hpp"
#include "Interface/WinAPI/ListView.hpp"
#include "Interface/WinAPI/ProgressBar.hpp"
#include "Interface/WinAPI/Static.hpp"
#include "Interface/WinAPI/StatusBar.hpp"

// Dialogs
//
#include "Interface/Login/LoginDlg.h"
#include "Interface/Main/MainDlg.h"

#pragma comment(linker, "\"/manifestdependency:type='Win32' "                                                          \
                        "name='Microsoft.Windows.Common-Controls' "                                                    \
                        "version='6.0.0.0' "                                                                           \
                        "processorArchitecture='*' "                                                                   \
                        "publicKeyToken='6595b64144ccf1df' "                                                           \
                        "language='*'\"")