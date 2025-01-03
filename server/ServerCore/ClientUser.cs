using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Security;
using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using Fleck;
using Serilog;
using System.Net.Http.Json;
using System.Net.Http;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace ServerCore
{
    public class ClientUser
    {
        public IWebSocketConnection _webSocketConnection { get; private set; }
        private ApiInterface _apiInterface;

        public string _userToken { get; private set; } = string.Empty;
        public string _jwtToken { get; private set; } = string.Empty;
        public string _hardwareId { get; private set; } = string.Empty;
        public string _hardwareInfo { get; private set; } = string.Empty;
        public string _cheat { get; private set; } = string.Empty;
        public string _ipAddress { get; private set; }
        private int _userId { get; set; }
        public DateTime _lastMessageReceivedTime { get; set; }

        private long __stage;

        private enum ClientUserStage : long
        {
            Unauthenticated,
            Authenticated
        }

        private ClientUserStage _stage
        {
            get => (ClientUserStage)Interlocked.CompareExchange(ref __stage, 0, 0);
            set => Interlocked.Exchange(ref __stage, (long)value);
        }

        public ClientUser(IWebSocketConnection webSocketConnection, ApiInterface apiInterface)
        {
            _webSocketConnection = webSocketConnection;
            _apiInterface = apiInterface;
            _lastMessageReceivedTime = DateTime.MinValue;
            _ipAddress = _webSocketConnection.ConnectionInfo.ClientIpAddress;
            _stage = ClientUserStage.Unauthenticated;

            Log.Information($"Client connected from {_ipAddress}");
        }

        private void SetUserId(int userId) { _userId = userId; }

        public int GetUserId() { return _userId; }

        private ClientUserStage GetStage() { return _stage; }

        private void SetStage(ClientUserStage stage)
        {
            _stage = stage;
        }

        public string GetIpAddress() { return _ipAddress; }

        public string GetHardwareId() { return _hardwareId; }

        private void SetHardwareId(string hardwareId) { _hardwareId = hardwareId; }

        public string GetHardwareInfo() { return _hardwareInfo; }

        private void SetHardwareInfo(string hardwareInfo) { _hardwareInfo = hardwareInfo; }

        public string GetUserToken() { return _userToken; }

        private void SetUserToken(string token) { _userToken = token; }

        public string GetJwtToken() { return _jwtToken; }

        private void SetJwtToken(string jwtToken) { _jwtToken = jwtToken; }

        public string GetCheat() { return _cheat; }

        private void SetCheat(string cheat) { _cheat = cheat; }

        public void UpdateLastReceiveTime()
        {
            _lastMessageReceivedTime = DateTime.UtcNow;
        }

        public async Task SendMessageAsync(string id, string msg)
        {
            var msgHeader = new MsgHeader
            {
                Id = id,
                Data = msg
            };

            string json = JsonSerializer.Serialize(msgHeader);
            await _webSocketConnection.Send(json);
        }

        public struct LoginApiResponse
        {
            [JsonPropertyName("result")]
            public string Result { get; set; }

            [JsonPropertyName("message")]
            public string? Message { get; set; }

            [JsonPropertyName("jwt_token")]
            public string? JwtToken { get; set; }

            [JsonPropertyName("cheat")]
            public string? Cheat { get; set; }
            [JsonPropertyName("expire_at")]
            public string? ExpirationDate { get; set; }

            [JsonPropertyName("userid")]
            public int UserId { get; set; }
        }

        public async Task<(bool result, string? message)> LoginAsync(string userToken, string hardwareId, string hardwareInfo)
        {
            if (GetStage() != ClientUserStage.Unauthenticated)
            {
                throw new InvalidOperationException("[Login] This message cannot be processed because client is already authenticated!");
            }

            try
            {
                var formData = new Dictionary<string, string>()
                {
                    { "user_token", userToken },
                    { "hardware_info", hardwareInfo },
                    { "hwid", hardwareId },
                    { "ip_address", GetIpAddress() }
                };

                var (httpResponse, response) = await _apiInterface.SendPostRequestAsync<LoginApiResponse>($"/api/login.php", formData);
                if (httpResponse.IsSuccessStatusCode &&
                    response.JwtToken != null &&
                    response.Cheat != null &&
                    response.ExpirationDate != null)
                {
                    SetStage(ClientUserStage.Authenticated);
                    SetUserId(response.UserId);
                    SetJwtToken(response.JwtToken);
                    SetCheat(response.Cheat);
                    SetUserToken(userToken);
                    SetHardwareId(hardwareId);
                    SetHardwareInfo(hardwareInfo);

                    Log.Debug($"[Login] Logged user from {GetIpAddress()}, uid: {GetUserId()}");

                    var msgLoginResponse = new MsgLoginResponse
                    {
                        ExpirationDate = response.ExpirationDate,
                        Game = response.Cheat
                    };

                    return (true, JsonSerializer.Serialize(msgLoginResponse));
                }

                Log.Error($"[Login] Unable to validate connection from {GetIpAddress()}, returned HTTP code: {httpResponse.StatusCode}");
                return (false, response.Message);
            }
            catch (Exception ex)
            {
                Log.Fatal($"[Login] Exception: {ex.Message}");
            }
            return (false, null);
        }

        public struct VersionApiResponse
        {
            [JsonPropertyName("result")]
            public string Result { get; set; }

            [JsonPropertyName("message")]
            public string? Message { get; set; }

            [JsonPropertyName("update")]
            public bool? Update { get; set; }
            [JsonPropertyName("file")]
            public string? File { get; set; }
            [JsonPropertyName("hash")]
            public string? Hash { get; set; }
        }

        public async Task<(bool result, string? message)> VerifyVersionAsync(string hash)
        {
            if (GetStage() != ClientUserStage.Unauthenticated)
            {
                throw new InvalidOperationException("[VerifyVersion] This message cannot be processed because client is already authenticated!");
            }

            try
            {
                var formData = new Dictionary<string, string>()
                {
                    { "hash", hash },
                };

                var (httpResponse, apiResonse) = await _apiInterface.SendPostRequestAsync<VersionApiResponse>($"/api/version.php", formData);
#if DEBUG
                var responseMessage = new MsgVersionResponse
                {
                    Update = false,
                    File = null,
                    Hash = null
                };

                return (true, JsonSerializer.Serialize(responseMessage));
#else
                
                if (httpResponse.IsSuccessStatusCode &&
                    apiResonse.Update != null &&
                     apiResonse.Hash != null &&
                    apiResonse.File != null)
                {
                    var responseMessage = new MsgVersionResponse
                    {
                        Update = (bool)apiResonse.Update,
                        File = apiResonse.File,
                        Hash = apiResonse.Hash
                    };

                    return (true, JsonSerializer.Serialize(responseMessage));
                }

                Log.Error($"[VerifyVersion] Unable to verify version from {GetIpAddress()}, returned HTTP code: {httpResponse.StatusCode}");
                return (false, apiResonse.Message);
#endif
            }
            catch (Exception ex)
            {
                Log.Fatal($"[VerifyVersion] Exception: {ex.Message}");
            }
            return (false, null);
        }

        public void Close()
        {
            Log.Information($"Disconnecting client from {GetIpAddress()}");
        }
    }
}
