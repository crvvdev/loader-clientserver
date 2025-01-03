#pragma once

class ActiveWindowManager
{
  public:
    static ActiveWindowManager &Instance()
    {
        static ActiveWindowManager instance;
        return instance;
    }

    HWND GetActiveWindow()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _activeWindow;
    }

    void SetActiveWindow(HWND hwnd)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _activeWindow = hwnd;
    }

    void ResetWindow()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _activeWindow = nullptr;
    }

  private:
    HWND _activeWindow = nullptr;
    std::mutex _mutex;
};