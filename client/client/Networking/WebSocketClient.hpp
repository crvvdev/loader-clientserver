#pragma once

#include "MessageHandlers.hpp"

#define MESSAGE_CALLBACK_HANDLER(n) void n(const json &)

typedef websocketpp::client<websocketpp::config::asio_client> client;

class CWebSocketClient : public std::enable_shared_from_this<CWebSocketClient>
{
  public:
    CWebSocketClient() = default;
    ~CWebSocketClient()
    {
        Stop();
    }

    void Run(const std::string &uri);
    void Stop();

    void SendVersionMessageAsync();
    void SendLoginMessageAsync(const std::wstring &userToken);

    void OnConnect(websocketpp::connection_hdl);
    void OnClose(websocketpp::connection_hdl);
    void OnFail(websocketpp::connection_hdl);

  private:
    MESSAGE_CALLBACK_HANDLER(HandleVersionMessageReply);
    MESSAGE_CALLBACK_HANDLER(HandleLoginMessageReply);

    client _c{};
    client::connection_ptr _con{};
    std::string _uri{};

    using MessageHandlerFn = std::function<void(const json &)>;

    const std::unordered_map<std::string, MessageHandlerFn> _messageHandlers = {
        {"version", std::bind(&CWebSocketClient::HandleVersionMessageReply, this, std::placeholders::_1)},
        {"login", std::bind(&CWebSocketClient::HandleLoginMessageReply, this, std::placeholders::_1)}};

    void SendMessageAsync(const std::string &id, const std::string &msg);
    void OnMessage(websocketpp::connection_hdl, client::message_ptr msg);
};

extern std::shared_ptr<CWebSocketClient> g_WebSocketClient;