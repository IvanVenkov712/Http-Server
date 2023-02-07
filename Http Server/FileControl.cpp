#include "FileControl.h"

std::filesystem::path FilePathFromUrl(std::wstring const& parent, std::wstring const& urlAbsPath)
{
	std::wstring pathFromUrl = urlAbsPath.substr(0, urlAbsPath.find(L'?'));
	return std::filesystem::path(parent).concat(pathFromUrl);
}

std::string GetMimeType(std::string const& extension)
{
	// return mime type for extension
	HKEY hKey = NULL;
	std::string result = "application/octet-stream";

	// open registry key
	LSTATUS s = ERROR_SUCCESS;
	if ((s = RegOpenKeyExA(HKEY_CLASSES_ROOT, extension.c_str(), 0, KEY_READ, &hKey)) == ERROR_SUCCESS)
	{
		// define buffer
		CHAR szBuffer[256] = { 0 };
		DWORD dwBuffSize = sizeof(szBuffer);

		// get content type
		if (RegQueryValueExA(hKey, "Content Type", NULL, NULL, (LPBYTE)szBuffer, &dwBuffSize) == ERROR_SUCCESS)
		{
			// success
			result = szBuffer;
		}
		// close key
		RegCloseKey(hKey);
	}
	return result;
}
