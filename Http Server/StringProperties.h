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

#define SCHEMENAMEPROP "SchemeName"
#define HOSTNAMEPROP "HostName"
#define PORTNAMEPROP "PortName"
#define MAXREQUESTTHREADSPROP "MaxRequestThreads"

// A header containing functions related to global server properties
// Possible properties and values:
// Global properties are used to define the behaviour of the server.
// 
// |===================+=======================+==========================================================================|
// | Properties        | Default value         | Possible Values and Their Meaning                                        |
// |===================+=======================+==========================================================================+
// | SchemaName        | http                  | http or https                                                            |
// |-------------------+-----------------------+--------------------------------------------------------------------------+
// | HostName          | +                     | The host name or IP address of the machine that the server is running on |
// |-------------------+-----------------------+--------------------------------------------------------------------------+
// | PortName          | 80                    | The port the server will listen on                                       |
// |-------------------+-----------------------+--------------------------------------------------------------------------+
// | PageDirPath       | ProgramPath + PageDir | The folder containing the pages that the server will process             |
// |-------------------+-----------------------+--------------------------------------------------------------------------+
// |                   |                       | The count of the max threads handling requests that the server can       |
// | MaxRequestThreads | unlimited             | run simultaneously. A measure that prevents attacks that would           |
// |                   |                       | run out the resources of the machine                                     |
// |-------------------+-----------------------+--------------------------------------------------------------------------|
//

namespace prop
{

	/// <summary>
	///		Sets the global property pointed to by 'propertyName' to 'value' if that property exists.
	/// </summary>
	/// <returns>
	///		true if the property exists, othrewise false.
	/// </returns>
	bool SetStringProperty(std::string const& propertyName, std::string const& value);

	/// <summary>
	///		Gets the global property's value pointed to by 'propertyName'.
	/// </summary>
	/// <param name="propertyName"></param>
	/// <returns>
	///		A string reference containing the property value, or 'Not found' if the property was not found.
	/// </returns>
	std::string const& GetStringProperty(std::string const& propertyName);

	// A widestring version of GetStringProperty.
	// Necessary because Windows HTTP Server API uses wide char strings.
	std::wstring GetStringPropertyW(std::string const& propertyName);

	DWORD GetMaxRequestThreadsProperty();

	//Checks whether the 'propName' property is contained in the table.
	bool IsKnownStringProperty(std::string const& propName);

	// Checks whether 'propValue' is a valid value for 'propName'.
	// Valid property values are described in the table
	bool IsValidStringPropertyValue(std::string const& propName, std::string const& propValue);
}
