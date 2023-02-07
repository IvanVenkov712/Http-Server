#pragma once
#include <mutex>
#include <exception>
#include <iostream>

//Exclude Winsock 1.0 definitions from Windows.h
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/// <summary>
///		Notifies the user that error was encountered. As error code 'GetLastError()' is used
/// </summary>
/// <param name="functionName">
///		The name of the function that failed.
/// </param>
void ReportError(const char* functionName);

/// <summary>
///		Notifies the user that error was encountered. As error code 'errorCode' is used
/// </summary>
/// <param name="functionName">
///		The name of the function that failed.
/// </param>
/// <param name="errorCode">
///		The system error code returned from 'functionName'
/// </param>
void ReportError(const char* functionName, ULONG errorCode);

/// <summary>
///		Notifies the user that error was encountered. As error identifier 'cppException' is used
/// </summary>
/// <param name="cppException">
///		The exception that will be mentioned.
/// </param>
void ReportError(std::exception const& cppException);