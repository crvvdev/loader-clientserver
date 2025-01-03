using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace ServerCore
{
    public class MsgHeader
    {
        [JsonPropertyName("id")]
        public required string Id { get; set; }

        [JsonPropertyName("data")]
        public required string Data { get; set; }
    }

    public class MsgVersion
    {
        [JsonPropertyName("hash")]
        public required string Hash { get; set; }
    }

    public class MsgVersionResponse
    {
        [JsonPropertyName("update")]
        public required bool Update { get; set; }
        [JsonPropertyName("file")]
        public required string? File { get; set; }
        [JsonPropertyName("hash")]
        public string? Hash { get; set; }
    }

    public class MsgLogin
    {
        [JsonPropertyName("user_token")]
        public required string UserToken { get; set; }

        [JsonPropertyName("hwid")]
        public required string HardwareId { get; set; }

        [JsonPropertyName("hw_info")]
        public required string HardwareInfo { get; set; }
    }

    public class MsgLoginResponse
    {
        [JsonPropertyName("expire_at")]
        public required string ExpirationDate { get; set; }

        [JsonPropertyName("game")]
        public required string Game { get; set; }
    }

    public class MsgResponse
    {
        [JsonPropertyName("success")]
        public required bool Success { get; set; }

        [JsonPropertyName("message")]
        public required string Message { get; set; }
    }
}
