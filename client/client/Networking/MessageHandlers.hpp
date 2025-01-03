#pragma once

using nlohmann::json;

struct MsgHeader
{
  private:
    std::string _id;
    std::string _data;

  public:
    MsgHeader(const std::string &id, const std::string &data) : _id(id), _data(data)
    {
    }

    MsgHeader(const json &j)
    {
        _id = j[VMProtectDecryptStringA("id")];
        _data = j[VMProtectDecryptStringA("data")];
    }

    std::string ToJsonString() const
    {
        json j;
        j[VMProtectDecryptStringA("id")] = _id;
        j[VMProtectDecryptStringA("data")] = _data;

        return j.dump();
    }

    auto GetId() const
    {
        return _id;
    }

    auto GetData() const
    {
        return _data;
    }
};

class MsgResponse
{
  private:
    bool _success = false;
    std::string _message{};

  public:
    MsgResponse(const json &j)
    {
        _success = j[VMProtectDecryptStringA("success")];
        _message = j[VMProtectDecryptStringA("message")];
    }

    bool GetSuccess() const
    {
        return _success;
    }

    std::string GetMsg() const
    {
        return _message;
    }
};

class MsgVersion
{
  private:
    std::string _hash{};

  public:
    MsgVersion(const std::string &hash) noexcept : _hash(hash)
    {
    }

    std::string ToJsonString() const
    {
        json j{};
        j[VMProtectDecryptStringA("hash")] = _hash;
        return j.dump();
    }
};

struct MsgVersionResponse
{
  private:
    bool _update = false;
    std::string _file{};
    std::string _hash{};

  public:
    MsgVersionResponse() = default;
    MsgVersionResponse(const json &j)
    {
        _update = j[VMProtectDecryptStringA("update")];

        if (j.contains(VMProtectDecryptStringA("file")) && !j[VMProtectDecryptStringA("file")].is_null())
        {
            _file = j[VMProtectDecryptStringA("file")];
        }

        if (j.contains(VMProtectDecryptStringA("hash")) && !j[VMProtectDecryptStringA("hash")].is_null())
        {
            _hash = j[VMProtectDecryptStringA("hash")];
        }
    }

    bool GetUpdate() const
    {
        return _update;
    }

    std::string GetFile() const
    {
        return _file;
    }

    std::string GetHash() const
    {
        return _hash;
    }
};

class MsgLogin
{
  private:
    std::string _userToken;
    std::string _hardwareId;
    std::string _hardwareInfo;

  public:
    MsgLogin(const std::string &userToken, const std::string &hardwareId, const std::string &hardwareInfo)
        : _userToken(userToken), _hardwareId(hardwareId), _hardwareInfo(hardwareInfo)
    {
    }

    std::string ToJsonString() const
    {
        json j{};
        j[VMProtectDecryptStringA("user_token")] = _userToken;
        j[VMProtectDecryptStringA("hwid")] = _hardwareId;
        j[VMProtectDecryptStringA("hw_info")] = _hardwareInfo;

        return j.dump();
    }
};

class MsgLoginResponse
{
  private:
    std::string _expirationDate;
    std::string _game;

  public:
    MsgLoginResponse() = delete;
    MsgLoginResponse(const json &j)
    {
        _expirationDate = j[VMProtectDecryptStringA("expire_at")];
        _game = j[VMProtectDecryptStringA("game")];
    }

    std::string GetExpirationDate() const
    {
        return _expirationDate;
    }

    std::string GetGame() const
    {
        return _game;
    }
};