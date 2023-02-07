#include "ReportError.h"

/// <summary>
///		Used for output synchronization
/// </summary>
std::mutex g_ioMutex;

void ReportError(const char* functionName, ULONG errorCode)
{
	std::lock_guard lg(g_ioMutex);
	std::cout << "Error: " << functionName << " returned error code " << errorCode << std::endl;
	return;
}

void ReportError(const char* functionName)
{
	std::lock_guard lg(g_ioMutex);
	std::cout << "Error: " << functionName << " failed with last error code " << GetLastError() << std::endl;
	return;
}

void ReportError(std::exception const& cppException)
{
	std::lock_guard lg(g_ioMutex);
	std::cout << "Error: " << cppException.what() << std::endl;
	return;
}