#include "RequestCallbacks.h"

bool FindFile(std::vector<std::filesystem::path> const& lookupDirs, std::filesystem::path const& absPath, std::filesystem::path& result);

HANDLE OpenFileIfFound(std::vector<std::string> const& lookupDirs, std::filesystem::path const& absPath, std::filesystem::path& result);

ULONG SendNotFoundResponse(HANDLE hRequestQueue, HTTP_REQUEST_ID requestId);

std::filesystem::path ConcatPaths(std::filesystem::path const& first, std::filesystem::path const& second) {
	std::filesystem::path res(first);
	res += second;
	return res;
}

void RequestCallbackGET(HANDLE hRequestQueue, PCHTTP_REQUEST pRequest, size_t pRequestSize)
{
	std::vector<std::string> const& lookupDirs = prop::GetListProperty(LOOKUPDIRSPROP);
	std::filesystem::path filePath;
	std::wstring absPathStr = std::wstring(pRequest->CookedUrl.pAbsPath, pRequest->CookedUrl.AbsPathLength / sizeof(WCHAR));
	HANDLE hFile = OpenFileIfFound(lookupDirs, absPathStr, filePath);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ULONG result = SendNotFoundResponse(hRequestQueue, pRequest->RequestId);
		if (result != NO_ERROR) {
			ReportError(WinapiFailure("HttpSendHttpResponse", result));
		}
		return;
	}
	HTTP_DATA_CHUNK chunk{};
	HTTP_RESPONSE response{};
	std::string mimeType = GetMimeType(filePath.extension().string());
	chunk.DataChunkType = HttpDataChunkFromFileHandle;
	chunk.FromFileHandle.ByteRange.Length.QuadPart = HTTP_BYTE_RANGE_TO_EOF;
	chunk.FromFileHandle.ByteRange.StartingOffset.QuadPart = 0;
	chunk.FromFileHandle.FileHandle = hFile;

	response.pReason = "OK";
	response.StatusCode = 200;

	response.Headers.KnownHeaders[HttpHeaderContentType] = {
		(USHORT)mimeType.size(), mimeType.c_str()
	};
	response.pEntityChunks = &chunk;
	response.EntityChunkCount = 1;

	ULONG bytesSent = 0;
	ULONG result = HttpSendHttpResponse(
		hRequestQueue, pRequest->RequestId, 0, &response, NULL, &bytesSent, NULL, 0, NULL, NULL
	);
	if (result != NO_ERROR) {
		ReportError(WinapiFailure("HttpSendHttpResponse", result));
	}
	CloseHandle(hFile);
}

void RequestCallbackHEAD(HANDLE hRequestQueue, PCHTTP_REQUEST pRequest, size_t pRequestSize)
{
	std::vector<std::string> const& lookupDirs = prop::GetListProperty(LOOKUPDIRSPROP);
	std::filesystem::path filePath;
	std::wstring absPathStr = std::wstring(pRequest->CookedUrl.pAbsPath, pRequest->CookedUrl.AbsPathLength / sizeof(WCHAR));
	HANDLE hFile = OpenFileIfFound(lookupDirs, absPathStr, filePath);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ULONG result = SendNotFoundResponse(hRequestQueue, pRequest->RequestId);
		if (result != NO_ERROR) {
			ReportError(WinapiFailure("HttpSendHttpResponse", result));
		}
		return;
	}
	HTTP_RESPONSE response{};
	std::string mimeType = GetMimeType(filePath.extension().string());
	response.pReason = "OK";
	response.StatusCode = 200;
	response.Headers.KnownHeaders[HttpHeaderContentType] = {
		(USHORT)mimeType.size(), mimeType.c_str()
	};
	LARGE_INTEGER fSize{};
	if (GetFileSizeEx(hFile, &fSize))
	{
		std::string strFSize = std::to_string(fSize.QuadPart);
		response.Headers.KnownHeaders[HttpHeaderContentLength] = {
			(USHORT)strFSize.size(), strFSize.c_str()
		};
	}
	ULONG bytesSent = 0;
	ULONG result = HttpSendHttpResponse(
		hRequestQueue, pRequest->RequestId, 0, &response, NULL, &bytesSent, NULL, 0, NULL, NULL
	);
	if (result != NO_ERROR) {
		ReportError(WinapiFailure("HttpSendHttpResponse", result));
	}
	CloseHandle(hFile);
}

bool FindFile(std::vector<std::string> const& lookupDirs, std::filesystem::path const& absPath, std::filesystem::path& result) {

	for (const auto& pageDirPath : lookupDirs) {
		result = pageDirPath;
		result += absPath;

		if (std::filesystem::is_regular_file(result)) {
			return true;
		}
		else if (std::filesystem::is_directory(result)) {
			auto indexPageList = prop::GetListProperty(INDEXPAGELISTPROP);
			for (auto const& fName : indexPageList) {
				if (std::filesystem::is_regular_file(result / fName)) {
					result /= fName;
					return true;
				}
			}
		}
		else {
			auto const& exts = prop::GetListProperty(DEFEXTSPROP);
			for (auto const& ext : exts) {
				if (std::filesystem::is_regular_file(ConcatPaths(result, ext))) {
					result += ext;
					return true;
				}
			}
		}
	}
	return false;
}
HANDLE OpenFileIfFound(std::vector<std::string> const& lookupDirs, std::filesystem::path const& absPath, std::filesystem::path& result) {
	if (!FindFile(lookupDirs, absPath, result)) {
		return INVALID_HANDLE_VALUE;
	}
	else {
		return CreateFile(
			result.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
		);
	}
}
ULONG SendNotFoundResponse(HANDLE hRequestQueue, HTTP_REQUEST_ID requestId) {
	static HTTP_RESPONSE response;
	response.pReason = "Not found";
	response.StatusCode = 404;
	response.pEntityChunks = NULL;
	response.EntityChunkCount = 0;

	ULONG bytesSent = 0;
	return HttpSendHttpResponse(
		hRequestQueue, requestId, 0, &response, NULL, &bytesSent, NULL, 0, NULL, NULL
	);
}
