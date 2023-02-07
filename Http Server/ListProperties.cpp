#include "ListProperties.h"

namespace prop
{

	static bool IsValidIndexPageList(std::vector<std::string> const& value);

	static bool IsValidLookupDirs(std::vector<std::string> const& value);

	static bool IsValidDefaultExtensions(std::vector<std::string> const& extsList);

	static std::map<std::string, std::vector<std::string>> listPropertiesMap = {
		std::pair<std::string, std::vector<std::string>>(LOOKUPDIRSPROP, {"PageDir"}),
		std::pair<std::string, std::vector<std::string>>(INDEXPAGELISTPROP, {"index.html"}),
		std::pair<std::string, std::vector<std::string>>(DEFEXTSPROP, {".html"}),
	};
	static constexpr const char* const invalidChars = "\\/:*?\"<>|";

	bool SetListProperty(std::string const& propertyName, std::string const& value)
	{
		std::vector<std::string> result;
		std::string arrElem;
		bool inQuotes = false;
		for (char ch : value) {
			if (ch == '\"') {
				if (inQuotes) {
					result.push_back(arrElem);
					arrElem = "";
				}
				inQuotes = !inQuotes;
			}
			else if (inQuotes) {
				arrElem += ch;
			}
			else if (std::isspace(ch, std::locale::classic())) {
				return false;
			}
		}
		return SetListProperty(propertyName, std::move(result));
	}

	bool SetListProperty(std::string const& propertyName, std::vector<std::string> const& value)
	{
		if (IsValidListPropertyValue(propertyName, value)) {
			listPropertiesMap[propertyName] = value;
			return true;
		}
		return false;
	}

	bool SetListProperty(std::string const& propertyName, std::vector<std::string>&& value)
	{
		if (IsValidListPropertyValue(propertyName, value)) {
			listPropertiesMap[propertyName] = std::move(value);
			return true;
		}
		return false;
	}

	std::vector<std::string> const& GetListProperty(std::string const& propertyName)
	{
		return listPropertiesMap[propertyName];
	}

	bool IsKnownListProperty(std::string const& propName)
	{
		return listPropertiesMap.contains(propName);
	}

	bool IsValidListPropertyValue(std::string const& propName, std::vector<std::string> const& value)
	{
		if (!IsKnownListProperty(propName)) {
			return false;
		}
		else if (propName == LOOKUPDIRSPROP) {
			return IsValidLookupDirs(value);
		}
		else if (propName == INDEXPAGELISTPROP) {
			return IsValidIndexPageList(value);
		}
		else if (propName == DEFEXTSPROP) {
			return IsValidDefaultExtensions(value);
		}
		return false;
	}

	bool IsValidIndexPageList(std::vector<std::string> const& value)
	{
		return !value.empty() && std::all_of(value.begin(), value.end(), [](std::string const& str) {
			return !str.empty() && str.find_first_of(invalidChars) == std::string::npos;
		});
	}

	bool IsValidLookupDirs(std::vector<std::string> const& value)
	{
		return std::all_of(value.begin(), value.end(), [](std::string const& str) {
			return std::filesystem::is_directory(str);
		});
	}

	bool IsValidDefaultExtensions(std::vector<std::string> const& extsList)
	{
		return !extsList.empty() && std::all_of(extsList.begin(), extsList.end(), [](std::string const& str) {
			return !str.empty() && str.starts_with('.') && str.find_first_of(invalidChars) == std::string::npos;
		});
	}
}