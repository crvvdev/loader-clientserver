#include "Includes.hpp"

namespace utils
{
std::string WStringToString(const std::wstring &wstr, UINT codePage)
{
    int sizeNeeded = WideCharToMultiByte(codePage, 0, wstr.c_str(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded)
    {
        std::string str(sizeNeeded, 0);
        if (WideCharToMultiByte(codePage, 0, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], sizeNeeded, NULL,
                                NULL))
        {
            return str;
        }
    }
    return {};
}

std::wstring StringToWString(const std::string &str, UINT codePage)
{
    int sizeNeeded = MultiByteToWideChar(codePage, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded)
    {
        std::wstring wstr(sizeNeeded, 0);
        if (MultiByteToWideChar(codePage, 0, str.c_str(), static_cast<int>(wstr.size()), &wstr[0], sizeNeeded))
        {
            return wstr;
        }
    }
    return {};
}

RTL_OSVERSIONINFOEXW GetWindowsVersionInfo()
{
    RTL_OSVERSIONINFOEXW osvi{};
    osvi.dwOSVersionInfoSize = sizeof(osvi);

    using RtlGetVersion_ = NTSTATUS(WINAPI *)(LPOSVERSIONINFOEXW);
    static RtlGetVersion_ RtlGetVersion = reinterpret_cast<RtlGetVersion_>(
        (void *)GetProcAddress(GetModuleHandle(VMProtectDecryptStringW("ntdll")), "RtlGetVersion"));

    RtlGetVersion(&osvi);
    return osvi;
}

std::string GetWindowsCodenameByBuildNumber(const int buildNumber)
{
    const std::unordered_map<int, std::string> winCodenames = {
        {19041, VMProtectDecryptStringA("Win10 2004")}, {19042, VMProtectDecryptStringA("Win10 20H2")},
        {19043, VMProtectDecryptStringA("Win10 21H1")}, {19044, VMProtectDecryptStringA("Win10 21H2")},
        {19045, VMProtectDecryptStringA("Win10 22H2")}, {19044, VMProtectDecryptStringA("Win10 21H2")},
        {19045, VMProtectDecryptStringA("Win10 22H2")}, {22000, VMProtectDecryptStringA("Win11 21H2")},
        {22621, VMProtectDecryptStringA("Win11 22H2")}, {22631, VMProtectDecryptStringA("Win11 23H2")},
        {26100, VMProtectDecryptStringA("Win11 24H2")}};

    auto it = winCodenames.find(buildNumber);
    if (it != winCodenames.end())
    {
        return it->second;
    }
    return VMProtectDecryptStringA("Unknown");
}

std::string GetWindowsCodenameByBuildNumber()
{
    RTL_OSVERSIONINFOEXW osvi = GetWindowsVersionInfo();
    return GetWindowsCodenameByBuildNumber(osvi.dwBuildNumber);
}

time_t TimestampStringToTime(const std::string &dateTimeStr)
{
    std::tm t{};

    int parseCount = sscanf_s(dateTimeStr.c_str(), VMProtectDecryptStringA("%d-%d-%dT%d:%d:%dZ"), &t.tm_year, &t.tm_mon,
                              &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);
    if (parseCount < 6)
    {
        return {};
    }

    t.tm_year -= 1900;
    t.tm_mon -= 1;
    t.tm_isdst = -1;

    return mktime(&t);
}

std::string FormatByteSize(uint64_t sizeInBytes)
{
    constexpr uint64_t KB = 1024;
    constexpr uint64_t MB = KB * 1024;
    constexpr uint64_t GB = MB * 1024;
    constexpr uint64_t TB = GB * 1024;
    constexpr uint64_t PB = TB * 1024;
    constexpr uint64_t EB = PB * 1024;

    std::ostringstream oss{};
    oss << std::fixed << std::setprecision(0);

    if (sizeInBytes >= EB)
    {
        oss << (sizeInBytes / (double)EB) << VMProtectDecryptStringA(" EB");
    }
    else if (sizeInBytes >= PB)
    {
        oss << (sizeInBytes / (double)PB) << VMProtectDecryptStringA(" PB");
    }
    else if (sizeInBytes >= TB)
    {
        oss << (sizeInBytes / (double)TB) << VMProtectDecryptStringA(" TB");
    }
    else if (sizeInBytes >= GB)
    {
        oss << (sizeInBytes / (double)GB) << VMProtectDecryptStringA(" GB");
    }
    else if (sizeInBytes >= MB)
    {
        oss << (sizeInBytes / (double)MB) << VMProtectDecryptStringA(" MB");
    }
    else if (sizeInBytes >= KB)
    {
        oss << (sizeInBytes / (double)KB) << VMProtectDecryptStringA(" KB");
    }
    else
    {
        oss << sizeInBytes << VMProtectDecryptStringA(" Bytes");
    }
    return oss.str();
};

std::string GetExecutablePath()
{
    char path[1024];
    GetModuleFileNameA(NULL, path, sizeof(path));
    return std::string(path);
}
} // namespace utils