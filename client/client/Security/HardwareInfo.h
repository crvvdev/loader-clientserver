#pragma once

#ifndef OID_802_3_PERMANENT_ADDRESS
#define OID_802_3_PERMANENT_ADDRESS 0x01010101
#endif

#ifndef OID_802_3_CURRENT_ADDRESS
#define OID_802_3_CURRENT_ADDRESS 0x01010102
#endif

#ifndef IOCTL_NDIS_QUERY_GLOBAL_STATS
#define IOCTL_NDIS_QUERY_GLOBAL_STATS 0x00170002
#endif

namespace security
{
std::vector<Wmi::Win32_DiskDrive> GetSystemDiskDrives();
std::vector<Wmi::Win32_VideoController> GetSystemVideoControllers();
std::vector<Wmi::Win32_NetworkAdapter> GetSystemNetworkAdapters();
std::vector<Wmi::Win32_PhysicalMemory> GetSystemPhysicalMemories();
Wmi::Win32_OperatingSystem GetSystemOperatingSystem();
Wmi::Win32_Processor GetSystemCPU();
Wmi::Win32_ComputerSystem GetSystemComputerSystem();
Wmi::Win32_BaseBoard GetSystemBaseBoard();
std::string GetSystemMachineId();
std::string GetSystemDhcpv6DUID();
std::string GetHardwareInfo();
std::string GetHardwareId();
}; // namespace security