#pragma once

class UserInformation
{
  public:
    UserInformation()
    {
        wchar_t userToken[256]{};

        GetPrivateProfileString(VMProtectDecryptStringW("User"), VMProtectDecryptStringW("Token"), L"", userToken,
                                ARRAYSIZE(userToken) - 1, VMProtectDecryptStringW("..\\login.ini"));

        _userToken = userToken;
    }

    std::wstring GetUserToken() const
    {
        return _userToken;
    }

    std::string GetGame() const
    {
        return _game;
    }

    void SetGame(const std::string &game)
    {
        _game = game;
    }

    std::string GetExpirationDate() const
    {
        return _expireAt;
    }

    void SetExpirationDate(const std::string &expirationDate)
    {
        _expireAt = expirationDate;
    }

    void UpdateLoginDetails(const std::wstring &userToken)
    {
        _userToken = userToken;

        WritePrivateProfileString(VMProtectDecryptStringW("User"), VMProtectDecryptStringW("Token"), _userToken.c_str(),
                                  VMProtectDecryptStringW("..\\login.ini"));
    }

  private:
    std::wstring _userToken{};
    std::string _game{};
    std::string _expireAt{};
};

inline UserInformation g_userInformation{};