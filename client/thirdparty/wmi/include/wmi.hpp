/**
  *
  * WMI
  * @author Thomas Sparber (2016)
  *
 **/

#ifndef WMI_HPP
#define WMI_HPP

#include <string>
#include <type_traits>

#include <wmiexception.hpp>
#include <wmiresult.hpp>

namespace Wmi
{
	inline std::string wstrtostr(const std::wstring& wstr)
	{
		int sizeNeeded = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()),
			NULL, 0, NULL, NULL);
		if (sizeNeeded)
		{
			std::string str(sizeNeeded, 0);
			if (WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], sizeNeeded, NULL,
				NULL))
			{
				return str;
			}
		}
		return {};
	}

	inline std::wstring strtowstr(const std::string& str)
	{
		int sizeNeeded =
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
		if (sizeNeeded)
		{
			std::wstring wstr(sizeNeeded, 0);
			if (MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(wstr.size()), &wstr[0],
				sizeNeeded))
			{
				return wstr;
			}
		}
		return {};
	}

	//SFINAE test
	//default wmi path if wmi class does not implement getWmiPath
	template <class WmiClass>
	std::string CallGetWmiPath(...)
	{
		return "cimv2";
	}

	template <class WmiClass>
	typename std::enable_if<std::is_function<decltype(WmiClass::getWmiPath)>::value, std::string>::type
		CallGetWmiPath(int /* required, otherwise we get an ambiguitiy error... */)
	{
		return WmiClass::getWmiPath();
	}

	void query(const std::string& q, const std::string& p, WmiResult& out);

	inline WmiResult query(const std::string& q, const std::string& p)
	{
		WmiResult result;
		query(q, p, result);
		return result;
	}

	template <class WmiClass>
	inline void retrieveWmi(WmiClass& out)
	{
		WmiResult result;
		const std::string q = std::string("SELECT * FROM ") + WmiClass::getWmiClassName();
		query(q, CallGetWmiPath<WmiClass>(0), result);
		out.setProperties(result, 0);
	}

	template <class WmiClass>
	inline void retrieveWmi(WmiClass& out, std::string columns)
	{
		WmiResult result;
		const std::string q = std::string("SELECT ") + columns + std::string(" FROM ") + WmiClass::getWmiClassName();
		query(q, CallGetWmiPath<WmiClass>(0), result);
		out.setProperties(result, 0);
	}

	template <class WmiClass>
	inline WmiClass retrieveWmi()
	{
		WmiClass temp;
		retrieveWmi(temp);
		return temp;
	}

	template <class WmiClass>
	inline WmiClass retrieveWmi(std::string columns)
	{
		WmiClass temp;
		retrieveWmi(temp, columns);
		return temp;
	}

	template <class WmiClass>
	inline void retrieveAllWmi(std::vector<WmiClass>& out)
	{
		WmiResult result;
		const std::string q = std::string("SELECT * FROM ") + WmiClass::getWmiClassName();
		query(q, CallGetWmiPath<WmiClass>(0), result);

		out.clear();
		for (std::size_t index = 0; index < result.size(); ++index)
		{
			WmiClass temp;
			temp.setProperties(result, index);
			out.push_back(std::move(temp));
		}
	}

	template <class WmiClass>
	inline void retrieveAllWmi(std::vector<WmiClass>& out, std::string columns)
	{
		WmiResult result;
		const std::string q = std::string("SELECT ") + columns + std::string(" FROM ") + WmiClass::getWmiClassName();
		query(q, CallGetWmiPath<WmiClass>(0), result);

		out.clear();
		for (std::size_t index = 0; index < result.size(); ++index)
		{
			WmiClass temp;
			temp.setProperties(result, index);
			out.push_back(std::move(temp));
		}
	}

	template <class WmiClass>
	inline std::vector<WmiClass> retrieveAllWmi()
	{
		std::vector<WmiClass> ret;
		retrieveAllWmi(ret);

		return ret;
	}

	template <class WmiClass>
	inline std::vector<WmiClass> retrieveAllWmi(std::string columns)
	{
		std::vector<WmiClass> ret;
		retrieveAllWmi(ret, columns);

		return ret;
	}

}; //end namespace Wmi

#endif //WMI_HPP