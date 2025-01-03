#pragma once

#define EVENT_CALLBACK_HANDLER(n) void n(void *)

struct EventParam
{
    std::string error{};

    EventParam() = default;
    EventParam(const std::string &error) : error(error)
    {
    }
};

struct LoginEventParam
{
    std::string expirationDate{};
    std::string game{};

    LoginEventParam(const std::string &expirationDate, const std::string &game)
        : expirationDate(expirationDate), game(game)
    {
    }
};

class CEventManager
{
  public:
    CEventManager();
    ~CEventManager();

    enum class EventType
    {
        ConnectSuccess,
        ConnectFailed,
        ConnectError,
        VersionSuccess,
        VersionUpdate,
        VersionFailed,
        LoginSuccess,
        LoginFailed,
        ParseMessageError,
    };

    enum class EventPriority
    {
        Normal,
        High
    };

    struct Event
    {
        EventType type;
        void *param;
        EventPriority priority;

        bool operator<(const Event &other) const
        {
            return priority < other.priority;
        }
    };

    using fnEventHandler = std::function<void(void *param)>;

    void NotifyEvent(EventType type, void *param = nullptr)
    {
        const std::lock_guard<std::mutex> lock(_eventMutex);

        const EventPriority priority = _events.at(type);
        _eventQueue.push({type, param, priority});
        _eventCondition.notify_one();
    }

  private:
    const std::unordered_map<EventType, EventPriority> _events = {
        {EventType::ConnectSuccess, EventPriority::Normal}, {EventType::VersionSuccess, EventPriority::Normal},
        {EventType::LoginSuccess, EventPriority::Normal},   {EventType::VersionUpdate, EventPriority::Normal},
        {EventType::VersionFailed, EventPriority::Normal},  {EventType::LoginFailed, EventPriority::Normal},
        {EventType::ConnectError, EventPriority::High},     {EventType::ParseMessageError, EventPriority::High}};

    std::thread _thread;
    std::atomic_bool _stopThread;

    std::mutex _eventMutex{};
    std::condition_variable _eventCondition{};
    std::priority_queue<Event> _eventQueue{};

    void EventNotifyThread();
};

extern std::shared_ptr<CEventManager> g_EventManager;