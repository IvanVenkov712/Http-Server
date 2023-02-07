#include "Properties.h"

namespace prop
{
	void StrTrim(std::string& str) {
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](char ch) {
			return !std::isspace(ch);
			}
		));
		str.erase(std::find_if(str.rbegin(), str.rend(), [](char ch) {
			return !std::isspace(ch);
			}
		).base(), str.end());
	}

	void SetPropertiesDefault()
	{

		prop::SetStringProperty(SCHEMENAMEPROP, "http");
		prop::SetStringProperty(HOSTNAMEPROP, "+");
		prop::SetStringProperty(PORTNAMEPROP, "80");
		prop::SetStringProperty(MAXREQUESTTHREADSPROP, "unlimited");

		prop::SetListProperty(LOOKUPDIRSPROP, std::vector<std::string>({ "PageDir" }));
		prop::SetListProperty(INDEXPAGELISTPROP, std::vector<std::string>({ "index.html" }));
	}

	std::string SetPropertiesFromConfFile(std::string const& confFilePath) noexcept
	{
		std::vector<std::string> setProperties;
		std::stringstream reportStream;

		if (!std::filesystem::is_regular_file(confFilePath)) {
			reportStream << "The configuration file " << std::quoted(confFilePath) << " could not be found." << std::endl;
		}
		else {
			try {
				int nLine = 0;
				std::ifstream ifs(confFilePath);
				std::string strLine;
				while (!ifs.eof()) {
					nLine++;
					std::getline(ifs, strLine);
					if (!(strLine.empty() || strLine[0] == '#')) {
						std::string strProperty;
						std::string strValue;
						std::stringstream lineStream(strLine);
						lineStream >> strProperty;
						char ch;
						while (true) {
							lineStream.read(&ch, 1);
							if (lineStream.eof() || ch == '#') {
								break;
							}
							strValue.push_back(ch);
						}
						StrTrim(strProperty);
						StrTrim(strValue);
						if (prop::IsKnownStringProperty(strProperty)) {
							if (std::find(setProperties.begin(), setProperties.end(), strProperty) != setProperties.end()) {
								reportStream << "Line " << nLine << ": The property " << std::quoted(strProperty) << " is already set. This occurrence will be ignored." << std::endl;
							}
							else if (!prop::SetStringProperty(strProperty, strValue)) {
								reportStream << "Line " << nLine << ": Invalid value \"" << strValue
									<< "\" for property \"" << strProperty << "\" will be ignored." << std::endl;
							}
							else {
								setProperties.push_back(strProperty);
							}
						}
						else if (prop::IsKnownListProperty(strProperty)) {
							if (std::find(setProperties.begin(), setProperties.end(), strProperty) != setProperties.end()) {
								reportStream << "Line " << nLine << ": The property " << std::quoted(strProperty) << " is already set. This occurrence will be ignored." << std::endl;
							}
							else if (!prop::SetListProperty(strProperty, strValue)) {
								reportStream << "Line " << nLine << ": Invalid value \"" << strValue
									<< "\" for property \"" << strProperty << "\" will be ignored." << std::endl;
							}
							else {
								setProperties.push_back(strProperty);
							}
						}
						else {
							reportStream << "Line " << nLine << ": Unknown property " << std::quoted(strProperty)
								<< " will be ignored." << std::endl;
						}
					}
				}
				ifs.close();
			}
			catch (std::exception& e) {
				reportStream = std::stringstream();
				reportStream << "Error: The configuration file could not be read: " << e.what() << std::endl;
				prop::SetPropertiesDefault();
			}
		}
		reportStream << std::endl;
		reportStream << "Info: Chosen scheme:       " << prop::GetStringProperty(SCHEMENAMEPROP) << std::endl;
		reportStream << "Info: Host name:           " << prop::GetStringProperty(HOSTNAMEPROP) << std::endl;
		reportStream << "Info: Port name:           " << prop::GetStringProperty(PORTNAMEPROP) << std::endl;
		reportStream << "Info: Max request threads: " << prop::GetStringProperty(MAXREQUESTTHREADSPROP) << std::endl;
		return reportStream.str();
	}
}