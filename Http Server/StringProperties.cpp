#include "StringProperties.h"

namespace prop
{

	// Checks whether 'HostName' is a valid host name.
	bool IsValidHostName(std::string const& hostName);

	// Checks whether 'portName' is a valid port name
	bool IsValidPort(std::string const& portName);

	bool IsValidMaxRequestThreads(std::string const& maxRequestThreads);

	std::wstring StringToWideString(std::string const& string);

	/// <summary>
	///		Global, a map, containing all configuration properties of the server
	/// </summary>
	static std::map<std::string, std::string> stringPropertiesMap = {
		std::pair<std::string, std::string>(SCHEMENAMEPROP, "http"),
		std::pair<std::string, std::string>(HOSTNAMEPROP, "+"),
		std::pair<std::string, std::string>(PORTNAMEPROP, "80"),
		std::pair<std::string, std::string>(MAXREQUESTTHREADSPROP, "unlimited"),
	};
	
	bool SetStringProperty(std::string const& propertyName, std::string const& value)
	{
		if (IsValidStringPropertyValue(propertyName, value))
		{
			stringPropertiesMap[propertyName] = value;
			return true;
		}
		return false;
	}

	std::string const& GetStringProperty(std::string const& propertyName)
	{
			return stringPropertiesMap[propertyName];
	}

	std::wstring GetStringPropertyW(std::string const& propertyName)
	{
		return StringToWideString(GetStringProperty(propertyName));
	}

	DWORD GetMaxRequestThreadsProperty() {
		std::string const& prop = prop::GetStringProperty(MAXREQUESTTHREADSPROP);
		if (prop == "unlimited") {
			return 500;
		}
		else return std::stoul(prop);
	}

	bool IsKnownStringProperty(std::string const& propName)
	{
		return stringPropertiesMap.contains(propName);
	}

	bool IsValidStringPropertyValue(std::string const& propName, std::string const& propValue)
	{
		try {
			if (!stringPropertiesMap.contains(propName)) {
				return false;
			}
			if (propName == SCHEMENAMEPROP) {
				return propValue == "http" || propValue == "https";
			}
			else if (propName == HOSTNAMEPROP) {
				return IsValidHostName(propValue);
			}
			else if (propName == PORTNAMEPROP) {
				return IsValidPort(propValue);
			}
			else if (propName == MAXREQUESTTHREADSPROP) {
				return IsValidMaxRequestThreads(propValue);
			}
		}
		catch (std::exception&) {
			return false;
		}
		return false;
	}

	bool IsValidHostName(std::string const& hostName)
	{
		if (hostName == "+" || hostName == "*") {
			return true;
		}
		bool firstIsLetter = !hostName.empty() && std::isalpha(*(hostName.begin()));
		bool isValidName = firstIsLetter  && std::all_of(hostName.begin(), hostName.end(), [](char ch)->bool {
			return std::isdigit(ch) || std::isalpha(ch) || ch == '.';
		});
		return(isValidName);
	}

	bool IsValidPort(std::string const& portName)
	{
		try {
			bool allAreDigits = !portName.empty() && std::all_of(portName.begin(), portName.end(),
				[](char ch)->bool {
					return std::isdigit(ch, std::locale::classic());
				});
			if (!allAreDigits) {
				return false;
			}
			int res = std::stoi(portName);
			return (res > (std::numeric_limits<unsigned short>::max)() || res < 1);
		}
		catch (...) {
			return false;
		}
	}

	bool IsValidMaxRequestThreads(std::string const& maxRequestThreads)
	{
		if (maxRequestThreads == "unlimited") {
			return true;
		}
		try {
			bool allAreDigits = !maxRequestThreads.empty() && std::all_of(maxRequestThreads.begin(), maxRequestThreads.end(),
				[](char ch)->bool {
					return std::isdigit(ch);
				});
			if (!allAreDigits) {
				return false;
			}
			long dummy = std::stoul(maxRequestThreads);
			return (dummy <= 0);
		}
		catch (...) {
			return false;
		}
	}

	std::wstring StringToWideString(std::string const& string)
	{
		const char* cStr = string.c_str();
		int res = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, cStr, -1, NULL, 0);
		if (res == 0) {
			throw WinapiFailure("MultiByteToWideChar");
		}
		LPWSTR buffer = new WCHAR[res];
		res = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, cStr, -1, buffer, res);
		if (res == 0) {
			delete[] buffer;
			throw WinapiFailure("MultiByteToWideChar");
		}
		std::wstring stringToReturn(buffer);
		delete[] buffer;
		return stringToReturn;
	}
}
