#pragma once
// A header containing help functions related to file paths and file types.

#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>

//Exclude Winsock 1.0 definitions from Windows.h
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/// <summary>
///		Encodes the special HTML characters in the URL contained in value.
///		It returns a string which contains the % - encoded characters.
/// </summary>
/// <typeparam name="ChType">
///		The template parameter ChType denotes the character type of the input and the returned strings.
/// </typeparam>
/// <param name="value">
///		Value must contain a valid URL address.
/// </param>
/// <returns>
///		The encoded URL.
/// </returns>
template <typename ChType>
std::basic_string<ChType> EncodeUrl(std::basic_string<ChType> const& value) {
	using UChType = typename std::make_unsigned<ChType>::type;
	using innerOstream = std::basic_ostringstream<ChType>;
	using innerString = std::basic_string<ChType>;
	innerOstream escaped;
	escaped.fill('0');
	escaped << std::hex;

	for (typename innerString::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		typename innerString::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (std::isalnum<ChType>(c, std::locale::classic()) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
		}
		else {
			// Any other characters are percent-encoded
			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((UChType)c);
			escaped << std::nouppercase;
		}
	}
	return escaped.str();
}

/// <summary>
///		Decodes the % - encoded special HTML characters in the url contained value. 
/// </summary>
/// <typeparam name="ChType">
///		The template parameter ChType denotes the character type of the input and the returned strings.
/// </typeparam>
/// <param name="value">
///		Must contain a valid encoded URL address.
/// </param>
/// <returns>
///		The decoded URL.
/// </returns>
template <typename ChType>
std::basic_string<ChType> DecodeUrl(std::basic_string<ChType> const& value) {
	using UChType = typename std::make_unsigned<ChType>::type;
	using innerOstream = std::basic_ostringstream<ChType>;
	using innerString = std::basic_string<ChType>;
	innerOstream decoded;

	for (typename innerString::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		typename innerString::value_type c = (*i);
		if (c == '%') {
			ChType arr[2] = { *(i + 1), *(i + 2) };
			UChType t = (UChType)std::stoul(arr, nullptr, 16);
			decoded << (ChType)t;
			i += 2;
		}
		else {
			decoded << c;
		}
	}

	return decoded.str();
}

/// <summary>
///		Creates a valid file path, a combination from 'parent' and 'urlAbsPath'. 
///		The query string, if there is one, is ignored.
/// </summary>
/// <param name="parent">
///		A valid path to a directory.
/// </param>
/// <param name="urlAbsPath">
///		The absolute path of a url.
///		For example, if 'http://hostname:port/sub1/sub2/file.html?param1=value1' is a valid url,
///		the absolute path will be 'sub1/sub2/file.html?param1=value1'. The query string will be ignored.
/// </param>
/// <returns>
///		The combined path from 'parent' and 'urlAbsPath'
/// </returns>
std::filesystem::path FilePathFromUrl(std::wstring const& parent, std::wstring const& urlAbsPath);

/// <summary>
///		This function transforms the given file extension to a MIME type, used in the response header Content-Type.
///		The MIME type string is searched in the Registry.
/// </summary>
/// <param name="extension">
///		A file extension. for example '.html', '.exe' and etc.
/// </param>
/// <returns>
///		A string containing the MIME type for the chosen extension.
///		If a corresponding MIME type for the extension wasn't found,
///		the function returns "application/octet-stream".
/// </returns>
std::string GetMimeType(const std::string& extension);