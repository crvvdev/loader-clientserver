#include "..\Includes.hpp"

std::shared_ptr<CEventManager> g_EventManager = std::make_shared<CEventManager>();

CEventManager::CEventManager() : _thread(&CEventManager::EventNotifyThread, this)
{
}

CEventManager::~CEventManager()
{
    _stopThread = true;
    _eventCondition.notify_one();

    if (_thread.joinable())
    {
        _thread.join();
    }
}

void CEventManager::EventNotifyThread()
{
    while (!_stopThread)
    {
        std::unique_lock<std::mutex> lock(_eventMutex);
        _eventCondition.wait(lock, [this] { return !_eventQueue.empty() || _stopThread; });

        if (_stopThread)
        {
            break;
        }

        const HWND activeWindow = ActiveWindowManager::Instance().GetActiveWindow();

        while (!_eventQueue.empty() && activeWindow != nullptr)
        {
            auto event = new Event(_eventQueue.top());
            _eventQueue.pop();

            PostMessage(activeWindow, WM_CLIENT_EVENT, 0, reinterpret_cast<LPARAM>(event));
        }
    }
}