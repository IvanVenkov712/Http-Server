#pragma once

//Standard C++ headers
#include <filesystem>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <queue>
#include <exception>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

//Winapi headers
//Exclude Winsock 1.0 definitions from Windows.h
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <http.h>

//Program specific headers
#include "WinapiFailure.h"
#include "ReportError.h"
#include "FileControl.h"
#include "Properties.h"
#include "RequestHandler.h"
#include "RequestCallbacks.h"

typedef const HTTP_REQUEST* PCHTTP_REQUEST;

/// <summary>
///		Handles GET requests.
/// </summary>
/// <param name="lookupDirs"></param>
/// <param name="hRequestQueue"></param>
/// <param name="pRequest"></param>
void RequestCallbackGET(HANDLE hRequestQueue, PCHTTP_REQUEST pRequest, size_t pRequestSize);

/// <summary>
///		Handles HEAD requests
/// </summary>
/// <param name="lookupDirs"></param>
/// <param name="hRequestQueue"></param>
/// <param name="pRequest"></param>
void RequestCallbackHEAD(HANDLE hRequestQueue, PCHTTP_REQUEST pRequest, size_t pRequestSize);
