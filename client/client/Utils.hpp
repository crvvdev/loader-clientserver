#pragma once

namespace utils
{
std::string WStringToString(const std::wstring &wstr, UINT codePage = CP_ACP);
std::wstring StringToWString(const std::string &str, UINT codePage = CP_ACP);

RTL_OSVERSIONINFOEXW GetWindowsVersionInfo();

std::string GetWindowsCodenameByBuildNumber(const int buildNumber);
std::string GetWindowsCodenameByBuildNumber();

std::string FormatByteSize(uint64_t sizeInBytes);
time_t TimestampStringToTime(const std::string &dateTimeStr);
std::string GetExecutablePath();
} // namespace utils