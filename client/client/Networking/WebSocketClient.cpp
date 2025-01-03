#include "../Includes.hpp"

using nlohmann::json;

std::shared_ptr<CWebSocketClient> g_WebSocketClient = std::make_shared<CWebSocketClient>();

void CWebSocketClient::Run(const std::string &uri)
{
    _c.set_access_channels(websocketpp::log::alevel::all);
    _c.clear_access_channels(websocketpp::log::alevel::frame_payload);
    _c.set_error_channels(websocketpp::log::elevel::all);

    _c.init_asio();

    _c.set_message_handler(
        std::bind(&CWebSocketClient::OnMessage, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    _c.set_open_handler(std::bind(&CWebSocketClient::OnConnect, shared_from_this(), std::placeholders::_1));
    _c.set_fail_handler(std::bind(&CWebSocketClient::OnFail, shared_from_this(), std::placeholders::_1));
    _c.set_close_handler(std::bind(&CWebSocketClient::OnClose, shared_from_this(), std::placeholders::_1));

    websocketpp::lib::error_code ec;
    _con = _c.get_connection(uri, ec);
    if (ec)
    {
        throw std::runtime_error("Failed to create connection: " + ec.message());
    }

    _c.connect(_con);
    _c.run();
}

void CWebSocketClient::Stop()
{
    _c.stop();
}

void CWebSocketClient::SendVersionMessageAsync()
{
    try
    {
        MsgVersion msg(security::SHA256HashFile(utils::GetExecutablePath()));
        SendMessageAsync("version", msg.ToJsonString());
    }
    catch (std::exception &e)
    {
#if _DEBUG
        std::cout << "Exception: " << e.what() << std::endl;
#endif

        cMessage::ShowError(0, VMProtectDecryptStringW("Failed to send version message"));
    }
}

void CWebSocketClient::SendLoginMessageAsync(const std::wstring &userToken)
{
    try
    {
        MsgLogin msg(utils::WStringToString(userToken, CP_UTF8), security::GetHardwareId(), "{}");
        SendMessageAsync("login", msg.ToJsonString());
    }
    catch (std::exception &e)
    {
#if _DEBUG
        std::cout << "Exception: " << e.what() << std::endl;
#endif

        cMessage::ShowError(0, VMProtectDecryptStringW("Failed to send login message"));
    }
}

void CWebSocketClient::HandleVersionMessageReply(const json &j)
{
    try
    {
        MsgResponse msg(j);
        if (!msg.GetSuccess())
        {
            g_EventManager->NotifyEvent(CEventManager::EventType::VersionFailed, new EventParam(msg.GetMsg()));
        }
        else
        {
            MsgVersionResponse msgVersion(json::parse(msg.GetMsg()));
            if (msgVersion.GetUpdate())
            {
                g_EventManager->NotifyEvent(CEventManager::EventType::VersionUpdate,
                                            new EventParam("New update available"));
            }
            else
            {
                g_EventManager->NotifyEvent(CEventManager::EventType::VersionSuccess);
            }
        }
    }
    catch ([[maybe_unused]] const std::exception &e)
    {
        g_EventManager->NotifyEvent(CEventManager::EventType::ParseMessageError
#if VERBOSE_LOG
                                    ,
                                    new EventParam("Error parsing version message: " + std::string(e.what()))
#endif
        );
    }
}

void CWebSocketClient::HandleLoginMessageReply(const json &j)
{
    try
    {
        MsgResponse msg(j);
        if (!msg.GetSuccess())
        {
            g_EventManager->NotifyEvent(CEventManager::EventType::LoginFailed, new EventParam(msg.GetMsg()));
        }
        else
        {
            MsgLoginResponse msgVersion(json::parse(msg.GetMsg())); // just for fuck sake because there's no data

            g_EventManager->NotifyEvent(CEventManager::EventType::LoginSuccess,
                                        new LoginEventParam(msgVersion.GetExpirationDate(), msgVersion.GetGame()));
        }
    }
    catch ([[maybe_unused]] const std::exception &e)
    {
        g_EventManager->NotifyEvent(CEventManager::EventType::ParseMessageError
#if VERBOSE_LOG
                                    ,
                                    new EventParam("Error parsing " __FUNCTION__ " message: " + std::string(e.what()))
#endif
        );
    }
}

void CWebSocketClient::SendMessageAsync(const std::string &id, const std::string &data)
{
    MsgHeader msg(id, data);
    _con->send(msg.ToJsonString());
}

void CWebSocketClient::OnMessage(websocketpp::connection_hdl, client::message_ptr msg)
{
    const std::string message = msg->get_payload();
    if (message.empty())
    {
        return;
    }

    try
    {
        MsgHeader msgHeader(json::parse(message));
        const json messageData = json::parse(msgHeader.GetData());

        auto it = _messageHandlers.find(msgHeader.GetId());
        if (it != _messageHandlers.end())
        {
            it->second(messageData);
        }
        else
        {
#ifdef _DEBUG
            std::cout << "Unknown unhandled message: " << msgHeader.GetId() << std::endl;
            assert(false);
#endif
        }
    }
    catch ([[maybe_unused]] const std::exception &e)
    {
        g_EventManager->NotifyEvent(CEventManager::EventType::ParseMessageError
#if VERBOSE_LOG
                                    ,
                                    new EventParam("Failed to parse message: " + std::string(e.what()))
#endif
        );
    }
}

void CWebSocketClient::OnConnect(websocketpp::connection_hdl)
{
    g_EventManager->NotifyEvent(CEventManager::EventType::ConnectSuccess);
}

void CWebSocketClient::OnFail(websocketpp::connection_hdl hdl)
{
    websocketpp::lib::error_code ec;
    auto con = _c.get_con_from_hdl(hdl, ec);
    g_EventManager->NotifyEvent(CEventManager::EventType::ConnectFailed, new EventParam(ec.message()));
}

void CWebSocketClient::OnClose(websocketpp::connection_hdl hdl)
{
    websocketpp::lib::error_code ec;
    auto con = _c.get_con_from_hdl(hdl, ec);
    g_EventManager->NotifyEvent(CEventManager::EventType::ConnectError, new EventParam(ec.message()));
}