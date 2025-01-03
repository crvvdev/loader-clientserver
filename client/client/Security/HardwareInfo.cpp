#include "..\Includes.hpp"

namespace security
{
Wmi::Win32_OperatingSystem GetSystemOperatingSystem()
{
    try
    {
        const auto query = Wmi::retrieveWmi<Wmi::Win32_OperatingSystem>();
        return query;
    }
    catch ([[maybe_unused]] const Wmi::WmiException &ex)
    {
#if _DEBUG
        std::cout << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << std::endl;
#endif
    }
    return {};
}

std::vector<Wmi::Win32_DiskDrive> GetSystemDiskDrives()
{
    try
    {
        return Wmi::retrieveAllWmi<Wmi::Win32_DiskDrive>();
    }
    catch ([[maybe_unused]] const Wmi::WmiException &ex)
    {
#if _DEBUG
        std::cout << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << std::endl;
#endif
    }
    return {};
}

Wmi::Win32_Processor GetSystemCPU()
{
    std::string result = VMProtectDecryptStringA("<None>");
    try
    {
        const auto query = Wmi::retrieveWmi<Wmi::Win32_Processor>();
        return query;
    }
    catch ([[maybe_unused]] const Wmi::WmiException &ex)
    {
#if _DEBUG
        std::cout << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << std::endl;
#endif
    }
    return {};
}

Wmi::Win32_ComputerSystem GetSystemComputerSystem()
{
    try
    {
        const auto query = Wmi::retrieveWmi<Wmi::Win32_ComputerSystem>();
        return query;
    }
    catch ([[maybe_unused]] const Wmi::WmiException &ex)
    {
#if _DEBUG
        std::cout << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << std::endl;
#endif
    }
    return {};
}

Wmi::Win32_BaseBoard GetSystemBaseBoard()
{
    std::string result = VMProtectDecryptStringA("<None>");
    try
    {
        const auto query = Wmi::retrieveWmi<Wmi::Win32_BaseBoard>();
        return query;
    }
    catch ([[maybe_unused]] const Wmi::WmiException &ex)
    {
#if _DEBUG
        std::cout << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << std::endl;
#endif
    }
    return {};
}

// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\TCPIP6\Parameters\Dhcpv6DUID

std::string GetSystemDhcpv6DUID()
{
    std::string result = VMProtectDecryptStringA("<None>");
    try
    {
        HKEY hKey{};
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                          VMProtectDecryptStringA("CurrentControlSet\\Services\\TCPIP6\\Parameters"), 0,
                          KEY_WOW64_64KEY | KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            std::vector<uint8_t> tmp(64);
            DWORD type = REG_BINARY;
            auto cb = static_cast<DWORD>(tmp.size());

            if (RegQueryValueExA(hKey, VMProtectDecryptStringA("Dhcpv6DUID"), nullptr, &type, tmp.data(), &cb) ==
                ERROR_SUCCESS)
            {
                result = SHA256Hash(tmp);
            }
        }
    }
    catch ([[maybe_unused]] const Wmi::WmiException &ex)
    {
#if _DEBUG
        std::cout << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << std::endl;
#endif
    }
    return result;
}

std::string GetSystemMachineId()
{
    std::string result = VMProtectDecryptStringA("<None>");
    try
    {
        HKEY hKey{};
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, VMProtectDecryptStringA("SYSTEM\\HardwareConfig"), 0,
                          KEY_WOW64_64KEY | KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            char tmp[256]{};
            DWORD type = REG_SZ;
            DWORD cb = ARRAYSIZE(tmp);

            if (RegQueryValueExA(hKey, VMProtectDecryptStringA("LastConfig"), nullptr, &type,
                                 reinterpret_cast<LPBYTE>(&tmp), &cb) == ERROR_SUCCESS)
            {
                result = tmp;
            }

            RegCloseKey(hKey);
        }
    }
    catch ([[maybe_unused]] const Wmi::WmiException &ex)
    {
#if _DEBUG
        std::cout << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << std::endl;
#endif
    }
    return result;
}

std::vector<Wmi::Win32_VideoController> GetSystemVideoControllers()
{
    try
    {
        return Wmi::retrieveAllWmi<Wmi::Win32_VideoController>();
    }
    catch ([[maybe_unused]] const Wmi::WmiException &ex)
    {
#if _DEBUG
        std::cout << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << std::endl;
#endif
    }
    return {};
}

std::vector<Wmi::Win32_NetworkAdapter> GetSystemNetworkAdapters()
{
    try
    {
        return Wmi::retrieveAllWmi<Wmi::Win32_NetworkAdapter>();
    }
    catch ([[maybe_unused]] const Wmi::WmiException &ex)
    {
#if _DEBUG
        std::cout << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << std::endl;
#endif
    }
    return {};
}

std::vector<Wmi::Win32_PhysicalMemory> GetSystemPhysicalMemories()
{
    try
    {
        return Wmi::retrieveAllWmi<Wmi::Win32_PhysicalMemory>();
    }
    catch ([[maybe_unused]] const Wmi::WmiException &ex)
    {
#if _DEBUG
        std::cout << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << std::endl;
#endif
    }
    return {};
}

std::pair<std::string, std::string> GetMacAddresses(const std::string &guid)
{
    std::pair<std::string, std::string> result = {VMProtectDecryptStringA("<None>"), VMProtectDecryptStringA("<None>")};
    std::string dosDeviceName = VMProtectDecryptStringA("\\\\?\\") + guid;

    HANDLE fileHandle =
        CreateFileA(dosDeviceName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        return result;
    }

    SCOPE_EXIT
    {
        CloseHandle(fileHandle);
    };

    auto FormatMacAddressToString = [](uint8_t *macAddress) -> std::string {
        std::ostringstream oss{};
        for (int i = 0; i < 6; i++)
        {
            if (i != 0)
            {
                oss << ':';
            }
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(macAddress[i]);
        }
        return oss.str();
    };

    DWORD bytesReturned = 0;
    DWORD oid = OID_802_3_CURRENT_ADDRESS;
    uint8_t macAddress[6]{};

    // Obtain current MAC address
    //
    if (DeviceIoControl(fileHandle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid), &macAddress,
                        ARRAYSIZE(macAddress), &bytesReturned, nullptr))
    {
        result.first = FormatMacAddressToString(macAddress);
    }

    // Obtain permanent MAC address
    //
    oid = OID_802_3_PERMANENT_ADDRESS;

    if (DeviceIoControl(fileHandle, IOCTL_NDIS_QUERY_GLOBAL_STATS, &oid, sizeof(oid), &macAddress,
                        ARRAYSIZE(macAddress), &bytesReturned, nullptr))
    {
        result.second = FormatMacAddressToString(macAddress);
    }

    return result;
}

std::string GetHardwareInfo()
{
    json j{};

    {
        const Wmi::Win32_ComputerSystem &computerSystem = GetSystemComputerSystem();

        j[VMProtectDecryptStringA("computer")] = {
            {VMProtectDecryptStringA("hostname"), computerSystem.DNSHostName},
            {VMProtectDecryptStringA("username"), computerSystem.UserName},
            {VMProtectDecryptStringA("model"), computerSystem.Model},
            {VMProtectDecryptStringA("manufacturer"), computerSystem.Manufacturer},
            {VMProtectDecryptStringA("family"), computerSystem.SystemFamily},
            {VMProtectDecryptStringA("sku"), computerSystem.SystemSKUNumber},
            {VMProtectDecryptStringA("operating_system"), json::array()}};

        const Wmi::Win32_OperatingSystem &os = GetSystemOperatingSystem();

        json obj = {
            {VMProtectDecryptStringA("name"), os.Name},
            {VMProtectDecryptStringA("version"), os.Version},
            {VMProtectDecryptStringA("build_number"), os.BuildNumber},
            {VMProtectDecryptStringA("country_code"), os.CountryCode},
            {VMProtectDecryptStringA("serial_number"), SHA1Hash(os.SerialNumber)},
            {VMProtectDecryptStringA("machine_id"), GetSystemMachineId()},
            {VMProtectDecryptStringA("physical_memory"), utils::FormatByteSize(computerSystem.TotalPhysicalMemory)},
            {VMProtectDecryptStringA("memory_slots"), json::array()}};

        j[VMProtectDecryptStringA("computer")][VMProtectDecryptStringA("operating_system")] = obj;
    }

    const std::vector<Wmi::Win32_PhysicalMemory> &physicalMemories = GetSystemPhysicalMemories();

    for (const Wmi::Win32_PhysicalMemory &slot : physicalMemories)
    {
        json obj = {{VMProtectDecryptStringA("bank"), slot.BankLabel},
                    {VMProtectDecryptStringA("clock_speed"), slot.ConfiguredClockSpeed},
                    {VMProtectDecryptStringA("model"), slot.Model},
                    {VMProtectDecryptStringA("manufacturer"), slot.Manufacturer},
                    {VMProtectDecryptStringA("part_number"), slot.PartNumber},
                    {VMProtectDecryptStringA("serial_number"), SHA1Hash(slot.SerialNumber)},
                    {VMProtectDecryptStringA("sku"), slot.SKU}};

        j[VMProtectDecryptStringA("operating_system")][VMProtectDecryptStringA("memory_slots")].push_back(obj);
    }

    const Wmi::Win32_Processor &cpu = GetSystemCPU();

    j[VMProtectDecryptStringA("processor")] = {{VMProtectDecryptStringA("name"), cpu.Name},
                                               {VMProtectDecryptStringA("thread_count"), cpu.ThreadCount},
                                               {VMProtectDecryptStringA("serial_number"), SHA1Hash(cpu.SerialNumber)}};

    const Wmi::Win32_BaseBoard &baseBoard = GetSystemBaseBoard();

    j[VMProtectDecryptStringA("base_board")] = {
        {VMProtectDecryptStringA("product"), baseBoard.Product},
        {VMProtectDecryptStringA("model"), baseBoard.Model},
        {VMProtectDecryptStringA("manufacturer"), baseBoard.Manufacturer},
        {VMProtectDecryptStringA("serialnumber"), SHA1Hash(baseBoard.SerialNumber)},
        {VMProtectDecryptStringA("sku"), baseBoard.SKU},
        {VMProtectDecryptStringA("part_number"), baseBoard.PartNumber}};

    j[VMProtectDecryptStringA("network_adapters")] = {{VMProtectDecryptStringA("dhcpv6uid"), GetSystemDhcpv6DUID()},
                                                      {VMProtectDecryptStringA("slots"), json::array()}};

    const std::vector<Wmi::Win32_NetworkAdapter> &networkAdapters = GetSystemNetworkAdapters();
    for (const Wmi::Win32_NetworkAdapter &slot : networkAdapters)
    {
        if (!slot.PhysicalAdapter)
        {
            continue;
        }

        const std::pair<std::string, std::string> &macAddressess = GetMacAddresses(slot.GUID);

        json obj = {{VMProtectDecryptStringA("name"), slot.Name},
                    {VMProtectDecryptStringA("mac_address"), SHA1Hash(macAddressess.first)},
                    {VMProtectDecryptStringA("permanent_address"), SHA1Hash(macAddressess.second)},
                    {VMProtectDecryptStringA("manufacturer"), slot.Manufacturer},
                    {VMProtectDecryptStringA("product_name"), slot.ProductName}};

        j[VMProtectDecryptStringA("network_adapters")][VMProtectDecryptStringA("slots")].push_back(obj);
    }

    j[VMProtectDecryptStringA("gpus")] = json::array();

    const std::vector<Wmi::Win32_VideoController> &videoControllers = GetSystemVideoControllers();
    for (const Wmi::Win32_VideoController &slot : videoControllers)
    {
        json obj = {{VMProtectDecryptStringA("name"), slot.Name},
                    {VMProtectDecryptStringA("driver_version"), slot.DriverVersion}};

        j[VMProtectDecryptStringA("gpus")].push_back(obj);
    }

    j[VMProtectDecryptStringA("disk_drives")] = json::array();

    const std::vector<Wmi::Win32_DiskDrive> &diskDrives = GetSystemDiskDrives();
    for (const Wmi::Win32_DiskDrive &slot : diskDrives)
    {
        json obj = {{VMProtectDecryptStringA("name"), slot.Name},
                    {VMProtectDecryptStringA("model"), slot.Model},
                    {VMProtectDecryptStringA("serial_number"), SHA1Hash(slot.SerialNumber)},
                    {VMProtectDecryptStringA("capacity"), utils::FormatByteSize(std::stoull(slot.Size))}};

        j[VMProtectDecryptStringA("disk_drives")].push_back(obj);
    }

    return j.dump();
}

std::string GetHardwareId()
{
    std::string hardwareId{};

    int hwidLen = VMProtectGetCurrentHWID(NULL, 0);
    if (hwidLen)
    {
        char *hwidBuf = new char[hwidLen];
        memset(hwidBuf, 0, hwidLen);

        VMProtectGetCurrentHWID(hwidBuf, hwidLen);
        hardwareId.insert(hardwareId.end(), hwidBuf, hwidBuf + hwidLen);

        delete[] hwidBuf;
    }

    const std::string machineId = GetSystemMachineId();
    const std::string hardwareIdHased = SHA256Hash(machineId + hardwareId);

    return hardwareIdHased;
}
} // namespace security