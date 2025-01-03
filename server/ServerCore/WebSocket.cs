using System.Collections.Concurrent;
using System.Net.Security;
using System.Net.Sockets;
using System.Net;
using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using Fleck;
using Serilog;
using System.Net.WebSockets;
using System.Net.Http;
using System.Runtime.ConstrainedExecution;
using System.Security.Cryptography;
using System.Threading;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace ServerCore
{
    public class WebSocketServer
    {
        private readonly Fleck.WebSocketServer _webSocketServer;
        private readonly ConcurrentDictionary<IWebSocketConnection, ClientUser> _clients;
        private readonly Dictionary<string, Func<(ClientUser client, string message), Task<bool>>> _messageHandlers;
        private readonly string _uri;
        private readonly string _host;
        private readonly int _port;
        private ApiInterface _apiInterface;

        public WebSocketServer(string host, int port, string apiUri, string apiKey)
        {
            _host = host;
            _port = port;
            _clients = new ConcurrentDictionary<IWebSocketConnection, ClientUser>();
            _messageHandlers = new Dictionary<string, Func<(ClientUser client, string message), Task<bool>>>
            {
                { "login", HandleLoginRequest },
                { "version", HandleVersionRequest },
            };
            _uri = $"ws://{_host}:{_port}";
            _webSocketServer = new Fleck.WebSocketServer(_uri);
            _apiInterface = new ApiInterface(apiUri, apiKey);
        }

        public void Start()
        {
            Log.Information($"WebSocket server listening at {_uri}");

            _webSocketServer.Start(socket =>
            {
                socket.OnOpen = () =>
                {
                    AddClient(socket);
                };
                socket.OnClose = () =>
                   {
                       RemoveClient(socket);
                   };
                socket.OnMessage = async message =>
                   {
                       await ProcessMessageAsync(socket, message);
                   };
                socket.OnError = ex =>
                    {
                        Log.Error($"[SOCKET] OnError {ex.Message}");
                        RemoveClient(socket);
                    };
            });
        }

        public void Stop()
        {
            Log.Information("Server is stopping...");

            foreach (var kvp in _clients)
            {
                RemoveClient(kvp.Key);
            }

            _webSocketServer.Dispose();
        }

        public ClientUser? AddClient(IWebSocketConnection webSocketConnection)
        {
            var client = new ClientUser(webSocketConnection, _apiInterface);
            if (_clients.TryAdd(webSocketConnection, client))
            {
                return client;
            }
            return null;
        }

        public void RemoveClient(IWebSocketConnection webSocketConnection)
        {
            if (_clients.TryRemove(webSocketConnection, out var client))
            {
                client.Close();
            }
        }

        private async Task ProcessMessageAsync(IWebSocketConnection webSocketConnection, string message)
        {
            if (_clients.TryGetValue(webSocketConnection, out var client))
            {
                if (await HandleMessage(client, message))
                {
                    client.UpdateLastReceiveTime();
                }
            }
        }

        private async Task<bool> HandleMessage(ClientUser client, string message)
        {
            try
            {
                var msgHeader = JsonSerializer.Deserialize<MsgHeader>(message);
                if (msgHeader == null)
                {
                    Log.Error("[Message] Failed to deserialize JSON object!");
                    return false;
                }

                string messageId = msgHeader.Id;
                string messageData = msgHeader.Data;

                if (_messageHandlers.TryGetValue(messageId, out var handler))
                {
                    return await handler((client, messageData));
                }
                else
                {
                    Log.Debug($"[Message] Unknown message not handled: {messageId}");
                    return false;
                }
            }
            catch (Exception ex)
            {
                Log.Error($"[Message] Exception: {ex.Message}\nInner exception: {ex.InnerException?.Message}");
                return false;
            }
        }

        private async Task<bool> HandleVersionRequest((ClientUser client, string message) tuple)
        {
            Log.Debug($"[Version] Received version request message from {tuple.client.GetIpAddress()}");

            try
            {
                var msgVersion = JsonSerializer.Deserialize<MsgVersion>(tuple.message);
                if (msgVersion == null)
                {
                    Log.Error("[Version] Failed to deserialize JSON object!");
                    return false;
                }

                string hash = msgVersion.Hash;

                var (result, message) = await tuple.client.VerifyVersionAsync(hash);

                var msgResponse = new MsgResponse
                {
                    Success = result,
                    Message = message ?? "Internal server error",
                };

                await tuple.client.SendMessageAsync("version", JsonSerializer.Serialize(msgResponse));
                return true;
            }
            catch (Exception ex)
            {
                Log.Error($"[Version] Exception: {ex.Message}\nInner exception: {ex.InnerException?.Message}");
                return false;
            }
        }

        private async Task<bool> HandleLoginRequest((ClientUser client, string message) tuple)
        {
            Log.Debug($"[LoginRequest] Received login request from {tuple.client.GetIpAddress()}");

            try
            {
                var msgAuth = JsonSerializer.Deserialize<MsgLogin>(tuple.message);
                if (msgAuth == null)
                {
                    Log.Error("[LoginRequest] Failed to deserialize JSON object!");
                    return false;
                }

                string userToken = msgAuth.UserToken;
                string hardwareId = msgAuth.HardwareId;
                string hardwareInfo = msgAuth.HardwareInfo;

                if (string.IsNullOrEmpty(userToken) ||
                    string.IsNullOrEmpty(hardwareId) ||
                    string.IsNullOrEmpty(hardwareInfo))
                {
                    Log.Error("[LoginRequest] One or more required params not provided.");
                    return false;
                }

                var (result, message) = await tuple.client.LoginAsync(userToken, hardwareId, hardwareInfo);

                var msgLoginResponse = new MsgResponse
                {
                    Success = result,
                    Message = message ?? "Internal server error",
                };

                await tuple.client.SendMessageAsync("login", JsonSerializer.Serialize(msgLoginResponse));
                return true;
            }
            catch (Exception ex)
            {
                Log.Fatal($"[LoginRequest] Exception: {ex.Message}\nInner exception: {ex.InnerException?.Message}");
                return false;
            }
        }
    }
}
