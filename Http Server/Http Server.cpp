// Http Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


//Standard C++ headers
#include <thread>
#include <iostream>

//Winapi headers
//Exclude Winsock 1.0 definitions from Windows.h
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <http.h>

//Program specific headers
#include "WinapiFailure.h"
#include "ReportError.h"
#include "RequestHandler.h"
#include "RequestCallbacks.h"

#pragma comment(lib, "httpapi.lib")

void CleanupProgram(HTTP_SERVER_SESSION_ID sessionId = 0, HANDLE hRequestQueue = NULL, HTTP_URL_GROUP_ID groupId = 0) {
	if (groupId != 0) {
		HttpCloseUrlGroup(groupId);
	}
	if (hRequestQueue != NULL) {
		HttpCloseRequestQueue(hRequestQueue);
	}
	if (sessionId != NULL) {
		HttpCloseServerSession(sessionId);
	}
	HttpTerminate(HTTP_INITIALIZE_SERVER, NULL);
}

HANDLE hTerminateProgramEvent;
BOOL WINAPI CtrlEventHandler(DWORD code) {
	if (code == CTRL_C_EVENT || code == CTRL_BREAK_EVENT || code == CTRL_CLOSE_EVENT) {
		return SetEvent(hTerminateProgramEvent);
	}
	return FALSE;
}

int main(int argc, char** argv) {
	std::string confFileName;
	if (argc == 2) {
		confFileName = argv[1];
	}
	else if (argc == 1) {
		confFileName = "HttpServer.conf";
	}
	else {
		std::cout << "Correct syntax: program-name [optional]path-to-conf-file. The program will exit after 3 seconds." << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(3));
		return 0;
	}
	hTerminateProgramEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!hTerminateProgramEvent) {
		throw WinapiFailure("CreateEvent");
	}
	if (!SetConsoleCtrlHandler(CtrlEventHandler, TRUE)) {
		throw WinapiFailure("SetConsoleCtrlHandler");
	}
	std::cout << "Press Ctrl + C for exit." << std::endl;

	if (confFileName == "noconf") {
		prop::SetPropertiesDefault();
		std::cout << "Info: Chosen scheme:       " << prop::GetStringProperty(SCHEMENAMEPROP) << std::endl;
		std::cout << "Info: Host name:           " << prop::GetStringProperty(HOSTNAMEPROP) << std::endl;
		std::cout << "Info: Port name:           " << prop::GetStringProperty(PORTNAMEPROP) << std::endl;
		std::cout << "Info: Max request threads: " << prop::GetStringProperty(MAXREQUESTTHREADSPROP) << std::endl;
	}
	else {
		std::cout << "Configuration file read results:" << std::endl;
		std::string readRes = prop::SetPropertiesFromConfFile(confFileName);
		std::cout << readRes;
	}

	ULONG res = HttpInitialize(HTTPAPI_VERSION_2, HTTP_INITIALIZE_SERVER, NULL);
	if (res != NO_ERROR) {
		return res;
	}
	HTTP_SERVER_SESSION_ID serverSessionId{};
	res = HttpCreateServerSession(HTTPAPI_VERSION_2, &serverSessionId, 0);
	if (res != NO_ERROR) {
		CleanupProgram();
		return res;
	}
	HANDLE hRequestQueue;
	res = HttpCreateRequestQueue(HTTPAPI_VERSION_2, NULL, NULL, 0, &hRequestQueue);
	if (res != NO_ERROR) {
		CleanupProgram(serverSessionId);
		return res;
	}
	HTTP_URL_GROUP_ID groupId{};
	res = HttpCreateUrlGroup(serverSessionId, &groupId, 0);
	if (res != NO_ERROR) {
		CleanupProgram(serverSessionId, hRequestQueue);
		return res;
	}
	HTTP_PROPERTY_FLAGS pf{1};
	HTTP_BINDING_INFO bindInfo = { pf, hRequestQueue };
	res = HttpSetUrlGroupProperty(groupId, HttpServerBindingProperty, &bindInfo, sizeof(bindInfo));
	if (res != NO_ERROR) {
		CleanupProgram(serverSessionId, hRequestQueue, groupId);
		return res;
	}
	std::wstring url = prop::GetStringPropertyW(SCHEMENAMEPROP) + L"://" + prop::GetStringPropertyW(HOSTNAMEPROP) + L":" + prop::GetStringPropertyW(PORTNAMEPROP) + L"/";
	res = HttpAddUrlToUrlGroup(groupId, url.c_str(), 0, 0);
	if (res == NO_ERROR) {
		RequestHandler handler(hRequestQueue, 1, prop::GetMaxRequestThreadsProperty());
		handler.SetRequestCallback(HttpVerbGET, RequestCallbackGET);
		handler.SetRequestCallback(HttpVerbHEAD, RequestCallbackHEAD);
		handler.Start();
		if (WaitForSingleObject(hTerminateProgramEvent, INFINITE) == WAIT_FAILED) {
			ReportError(WinapiFailure("WaitForSingleObject"));
		}
	}
	else {
		CleanupProgram(serverSessionId, hRequestQueue, groupId);
		return res;
	}
	CleanupProgram(serverSessionId, hRequestQueue, groupId);
	return 0;
}