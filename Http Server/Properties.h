#pragma once
#include "StringProperties.h"
#include "ListProperties.h"

namespace prop
{
	/// <summary>
	///		Resets the global properties to the defaults described in the table.
	/// </summary>
	void SetPropertiesDefault();

	/// <summary>
	///		Reads the configuration file pointed by 'ConfFilePath' and sets the global properties 
	///		corresponding to the file's content.
	///		Any unrecognised property or invalid value will be ignored and noted in the report.
	/// </summary>
	/// <returns>
	///		A report containing information about the properties' values, unrecognised properties
	///		and invalid property values.
	/// </returns>
	std::string SetPropertiesFromConfFile(std::string const& confFilePath) noexcept;
}
