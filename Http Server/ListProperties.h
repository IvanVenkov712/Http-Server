#pragma once

#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <iomanip>
#include <cwchar>
#include <cwctype>
#include <Windows.h>
#include "WinapiFailure.h"

#define LOOKUPDIRSPROP "LookupDirs"
#define INDEXPAGELISTPROP "IndexPageList"
#define DEFEXTSPROP "DefaultExtensions"

namespace prop
{
	bool SetListProperty(std::string const& propertyName, std::string const& value);

	bool SetListProperty(std::string const& propertyName, std::vector<std::string> const& value);

	bool SetListProperty(std::string const& propertyName, std::vector<std::string>&& value);

	std::vector<std::string> const& GetListProperty(std::string const& propertyName);

	bool IsKnownListProperty(std::string const& propName);

	bool IsValidListPropertyValue(std::string const& propName, std::vector<std::string> const& value);
}