#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <http.h>
#include <string>
#include <exception>
#include <thread>
#include <atomic>
#include <filesystem>
#include <map>
#include <functional>

#include "ReportError.h"

typedef const HTTP_REQUEST* PCHTTP_REQUEST;

/// <summary>
///		Enables the user to process HTTP requests in a Windows request queue asynchronously.
///		The request processing is implemented with the help of callbacks which the user can specify.
///		If the instance is a runnable object, it can be started and stopped at any time.
///		Any necessary cleanup is performed in the destructor.
///		Internally uses a threadpool.
///		Not copy-assignable.
/// </summary>
class RequestHandler {
public:
	using RequestCallback = std::function<void(HANDLE hRequestQueue, PCHTTP_REQUEST pRequest, size_t pRequestSize)>;
	RequestHandler();
	
	/// <summary>
	///		Constructs a RequestHandler object. The only allowed constructor 
	/// </summary>
	/// <param name = "lookupDirs">
	///		The path where all the requested URLs are searched.
	/// </param>
	/// <param name = "hRequestQueue">
	///		A handle to a valid request queue which requests will be handled by the instance.
	/// </param>
	/// <param name = "minThreadsCount">
	///		The minimal count of threads created in order to start to process requests.
	/// </param>
	/// <param name = "maxThreadsCount">
	///		The max count of the threads that can be created for processing requests.
	/// </param>
	RequestHandler(HANDLE hRequestQueue, DWORD minThreadsCount = 1, DWORD maxThreadsCount = std::thread::hardware_concurrency());
	RequestHandler(RequestHandler&& right) noexcept;
	RequestHandler& operator=(RequestHandler&& right) noexcept;

	void SetRequestCallback(HTTP_VERB handledVerb, RequestCallback const& handlerFunc);
	void SetDefaultRequestCallback(RequestCallback const& handlerFunc);

	void RemoveRequestCallback(HTTP_VERB verb);

	/// <summary>
	///		Causes to start working in the background, does not block the calling thread.
	/// </summary>
	void Start();

	void Stop();

	bool Runnable() const;

	bool Running() const;

	/// <summary>
	///		Destructor: Calls Destroy();
	/// </summary>
	~RequestHandler();

	//Deleted
	RequestHandler(RequestHandler const&) = delete;
	RequestHandler& operator=(RequestHandler const&) = delete;




private:
	//Internals
	std::atomic_bool running = false;
	std::atomic_bool runnable = false;

	TP_CALLBACK_ENVIRON callbackEnviron{};
	PTP_POOL pThreadpool = NULL;
	PTP_CLEANUP_GROUP tpCleanupGroup = NULL;
	HANDLE hTargetRequestQueue = NULL;
	HANDLE hTerminateBGWorkerThreadEvent = NULL;

	std::thread mainBGWorkerThread;
	std::map<HTTP_VERB, RequestCallback> requestCallbacksMap;
	RequestCallback defaultRequestCallback;

	/// <summary>
	///		This is the routine which is executed in the background. All requests are processed here.
	/// </summary>
	void BGWorkerThreadRoutine();

	/// <summary>
	///		Sends a response corresponding to the request method and the requested URL.
	/// </summary>
	/// <param name="pRequest">
	///		pointer to a HTTP_REQUEST structure, containing data about the request. Will not be modified.
	/// </param>
	void HandleRequest(PCHTTP_REQUEST pRequest, size_t pRequestSize);

	/// <summary>
	///		Safely terminates the work of the instance: terminates all running threads and discards all pending requests.
	///		If the background worker is not running, has no effect.
	/// </summary>
	void Destroy();
};