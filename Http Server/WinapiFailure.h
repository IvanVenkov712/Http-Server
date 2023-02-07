#pragma once
#include <exception>
#include <string>
#include <sstream>
#include <Windows.h>
/// <summary>
///		Used to handle WINAPI functions failure
/// </summary>
class WinapiFailure : public std::exception {
public:
	/// <summary>
	///
	/// </summary>
	/// <param name="funName">
	///		The name of the function which failed
	/// </param>
	/// <param name="funErrReturnCode">
	///		The return code of the function which indicated failure
	/// </param>
	WinapiFailure(const char* funName, DWORD funErrReturnCode) noexcept : std::exception() {
		
		std::stringstream ssMessage;
		ssMessage << funName << " returned " << funErrReturnCode << " error code.";
		LPSTR formattedMessage = NULL;
		DWORD formatMessageRes = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
			funErrReturnCode, 0, (LPSTR)&formattedMessage, 0, NULL
		);
		if (formatMessageRes != 0) {
			ssMessage << " Formatted error message: " << formattedMessage;
			LocalFree(formattedMessage);
		}
		this->message = ssMessage.str();

	}
	/// <summary>
	///		GetLastError() used to identify the failure
	/// </summary>
	/// <param name="funName">
	///		The name of the function which failed
	/// </param>
	explicit WinapiFailure(const char* funName) noexcept : std::exception() {
		std::stringstream ssMessage;
		ssMessage << funName << " failed with " << GetLastError() << " last error code.";
		LPSTR formattedMessage = NULL;
		DWORD formatMessageRes = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
			GetLastError(), 0, (LPSTR)&formattedMessage, 0, NULL
		);
		if (formatMessageRes != 0) {
			ssMessage << " Formatted error message: " << formattedMessage;
			LocalFree(formattedMessage);
		}
		this->message = ssMessage.str();
	}
	//Move ctor
	WinapiFailure(WinapiFailure&& right) noexcept : std::exception() {
		this->message = std::move(right.message);
	}

	//Copy ctor
	WinapiFailure(WinapiFailure const& right) noexcept : std::exception() {
		this->message = right.message;
	}

	//Move assignment operator
	const WinapiFailure& operator=(WinapiFailure&& right) noexcept {
		if (this == &right) {
			return *this;
		}
		this->message = std::move(right.message);
		return *this;
	}

	//Copy assignment operator
	const WinapiFailure& operator=(WinapiFailure const& right) noexcept {
		if (this == &right) {
			return *this;
		}
		this->message = right.message;
		return *this;
	}

	//Returns the error message.
	virtual const char* what() const noexcept override {
		return this->message.c_str();
	}

	WinapiFailure() = delete;
private:
	std::string message;
};

