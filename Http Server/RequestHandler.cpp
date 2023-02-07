#include "RequestHandler.h"
#include "WinapiFailure.h"

RequestHandler::RequestHandler()
{
	this->callbackEnviron = {};
	this->tpCleanupGroup = NULL;
	this->pThreadpool = NULL;
	this->hTerminateBGWorkerThreadEvent = NULL;
	this->hTargetRequestQueue = NULL;
	this->running = false;
	this->runnable = false;

	this->defaultRequestCallback = RequestCallback();
	this->requestCallbacksMap = std::map<HTTP_VERB, RequestCallback>();
	this->mainBGWorkerThread = std::thread();
}

RequestHandler::RequestHandler(HANDLE hRequestQueue, DWORD minThreadsCount, DWORD maxThreadsCount)
{
	this->hTargetRequestQueue = hRequestQueue;
	this->pThreadpool = CreateThreadpool(NULL);
	if (!this->pThreadpool) {
		throw WinapiFailure("CreateThreadpool");
	}
	this->tpCleanupGroup = CreateThreadpoolCleanupGroup();
	if (!this->tpCleanupGroup) {
		throw WinapiFailure("CreateThreadpoolCleanupGroup");
	}
	this->hTerminateBGWorkerThreadEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
	if (!this->hTerminateBGWorkerThreadEvent) {
		throw WinapiFailure("CreateEventW");
	}
	this->defaultRequestCallback = [](HANDLE hRequestQueue, PCHTTP_REQUEST pRequest, size_t pRequestSize) {
		HTTP_RESPONSE response{};
		response.pReason = "Not implemented";
		response.StatusCode = 503;
		response.pEntityChunks = NULL;
		response.EntityChunkCount = 0;
		ULONG bytesSent = 0;
		ULONG result = HttpSendHttpResponse(
			hRequestQueue, pRequest->RequestId, 0, &response, NULL, &bytesSent, NULL, 0, NULL, NULL
		);
		if (result != NO_ERROR) {
			ReportError(WinapiFailure("HttpSendHttpResponse", result));
		}
	};
	InitializeThreadpoolEnvironment(&this->callbackEnviron);
	SetThreadpoolThreadMaximum(this->pThreadpool, maxThreadsCount);
	SetThreadpoolThreadMinimum(this->pThreadpool, minThreadsCount);
	SetThreadpoolCallbackPool(&this->callbackEnviron, this->pThreadpool);
	SetThreadpoolCallbackCleanupGroup(&this->callbackEnviron, this->tpCleanupGroup, NULL);
	this->runnable = true;
	this->running = false;
}

RequestHandler::RequestHandler(RequestHandler&& right) noexcept
{
	*this = std::move(right);
}

RequestHandler& RequestHandler::operator=(RequestHandler&& right) noexcept
{
	if (right.runnable && right.running)
	{
		if (!SetEvent(right.hTerminateBGWorkerThreadEvent)) {
			throw WinapiFailure("SetEvent");
		}
		if (right.mainBGWorkerThread.joinable()) {
			right.mainBGWorkerThread.join();
		}
	}
	this->Destroy();

	this->callbackEnviron = std::move(right.callbackEnviron);
	this->tpCleanupGroup = right.tpCleanupGroup;
	this->pThreadpool = right.pThreadpool;
	this->hTerminateBGWorkerThreadEvent = right.hTerminateBGWorkerThreadEvent;
	this->hTargetRequestQueue = right.hTargetRequestQueue;

	this->defaultRequestCallback = std::move(right.defaultRequestCallback);
	this->requestCallbacksMap = std::move(right.requestCallbacksMap);
	this->running = false;
	this->runnable = right.runnable.load();
	if (right.running && this->runnable) {
		this->Start();
	}
	else {
		this->mainBGWorkerThread = std::thread();
	}
	right.callbackEnviron = {};
	right.tpCleanupGroup = NULL;
	right.pThreadpool = NULL;
	right.hTerminateBGWorkerThreadEvent = NULL;
	right.hTargetRequestQueue = NULL;
	right.running = false;
	right.runnable = false;

	return *this;
}

void RequestHandler::SetRequestCallback(HTTP_VERB handledVerb, RequestCallback const& handlerFunc)
{
	this->requestCallbacksMap[handledVerb] = handlerFunc;
}

void RequestHandler::SetDefaultRequestCallback(RequestCallback const& handlerFunc)
{
	this->defaultRequestCallback = handlerFunc;
}

void RequestHandler::RemoveRequestCallback(HTTP_VERB verb)
{
	if (this->requestCallbacksMap.contains(verb)) {
		this->requestCallbacksMap.erase(verb);
	}
}

void RequestHandler::Start()
{
	if (!this->runnable) {
		throw std::exception("Trying to start an object that is not runnable");
	}
	if (!this->running) {
		if (!ResetEvent(this->hTerminateBGWorkerThreadEvent)) {
			throw WinapiFailure("ResetEvent");
		}
		this->mainBGWorkerThread = std::thread(&RequestHandler::BGWorkerThreadRoutine, this);
		this->running = true;
	}
}

void RequestHandler::Stop()
{
	if (!this->runnable) {
		throw std::exception("Trying to stop an object that is not runnable");
	}
	if (this->running)
	{
		if (!SetEvent(this->hTerminateBGWorkerThreadEvent)) {
			throw WinapiFailure("SetEvent");
		}
		if (this->mainBGWorkerThread.joinable()) {
			this->mainBGWorkerThread.join();
		}
		this->running = false;
	}
}

bool RequestHandler::Runnable() const
{
	return this->runnable;
}

bool RequestHandler::Running() const
{
	return this->runnable && this->running;
}

void RequestHandler::BGWorkerThreadRoutine()
{
	/// <summary>
	///		Used to pass this and a pointer to HTTP_REQUEST structure to SimpleCallback
	/// </summary>
	struct SimpleCallbackParams {
		RequestHandler* hndThis = NULL;
		PCHTTP_REQUEST pRequest = NULL;
		size_t pRequestSize = 0;
		SimpleCallbackParams(RequestHandler* hndThis, PCHTTP_REQUEST pRequest, size_t pRequestSize) :
			hndThis(hndThis), pRequest(pRequest), pRequestSize(pRequestSize) {}
		~SimpleCallbackParams() {
			HeapFree(GetProcessHeap(), 0, const_cast<PHTTP_REQUEST>(this->pRequest));
			this->pRequest = NULL;
			this->hndThis = NULL;
		}
	};

	HANDLE hRequestEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
	if (!hRequestEvent) {
		throw WinapiFailure("CreateEventW");
	}
	HTTP_REQUEST_ID requestId{};
	HTTP_SET_NULL_ID(&requestId);
	ULONG requestBufferSize = 4096 + sizeof(HTTP_REQUEST);
	ULONG bytesReturned = 0;
	ULONG receiveResult = 0;
	while (true) {
		// The buffer containing the HTTP_REQUEST structure
		BYTE* requestBuffer = (BYTE*)HeapAlloc(GetProcessHeap(), 0, requestBufferSize);
		PHTTP_REQUEST pRequest = (PHTTP_REQUEST)(requestBuffer);
		if (!requestBuffer) {
			throw std::bad_alloc();
		}

		OVERLAPPED overlapped = {.hEvent = hRequestEvent};
		receiveResult = HttpReceiveHttpRequest(
			this->hTargetRequestQueue, requestId, 0, pRequest, requestBufferSize, NULL, &overlapped
		);

		HANDLE eventsToWait[] = { hRequestEvent, this->hTerminateBGWorkerThreadEvent };
		DWORD waitRes = WaitForMultipleObjects(2, eventsToWait, FALSE, INFINITE);
		if (waitRes == WAIT_FAILED) {
			HeapFree(GetProcessHeap(), 0, requestBuffer);
			throw WinapiFailure("WaitForMultipleObjects");
		}
		else {
			waitRes = WaitForSingleObject(this->hTerminateBGWorkerThreadEvent, 0);
			if (waitRes == WAIT_FAILED) {
				HeapFree(GetProcessHeap(), 0, requestBuffer);
				throw WinapiFailure("WaitForSingleObject");
			}
			else if (waitRes == WAIT_OBJECT_0) {
				HttpCancelHttpRequest(this->hTargetRequestQueue, requestId, &overlapped);
				HeapFree(GetProcessHeap(), 0, requestBuffer);
				requestBuffer = NULL;
				break;
			}
		}
		BOOL overlappedRes = GetOverlappedResult(this->hTargetRequestQueue, &overlapped, &bytesReturned, TRUE);
		if (overlappedRes)
		{
			SimpleCallbackParams* params = new SimpleCallbackParams(this, pRequest, requestBufferSize);
			BOOL submitted = TrySubmitThreadpoolCallback([](PTP_CALLBACK_INSTANCE, PVOID pContext)->VOID {
					SimpleCallbackParams* params = (SimpleCallbackParams*)pContext;
					params->hndThis->HandleRequest(params->pRequest, params->pRequestSize);
					delete params;
					return;
				}, params, &this->callbackEnviron
			);
			if (!submitted) {
				HeapFree(GetProcessHeap(), 0, requestBuffer);
				requestBuffer = NULL;
				throw WinapiFailure("TrySubmitThreadpoolCallback");
			}
			HTTP_SET_NULL_ID(&requestId);
		}
		else {
			DWORD err = GetLastError();
			if (err == ERROR_MORE_DATA) {
				requestBufferSize = bytesReturned;
				requestId = pRequest->RequestId;
				HeapFree(GetProcessHeap(), 0, requestBuffer);
				requestBuffer = NULL;
			}
			else if (err == ERROR_CONNECTION_INVALID) {
				ReportError(WinapiFailure("HttpReceiveHttpRequest"));
				HTTP_SET_NULL_ID(&requestId);
				HeapFree(GetProcessHeap(), 0, requestBuffer);
				requestBuffer = NULL;
			}
			else {
				ReportError(WinapiFailure("HttpReceiveHttpRequest"));
				HeapFree(GetProcessHeap(), 0, requestBuffer);
				requestBuffer = NULL;
				break;
			}
		}
		if (!ResetEvent(hRequestEvent)) {
			throw WinapiFailure("ResetEvent");
		}
	}
	CloseThreadpoolCleanupGroupMembers(this->tpCleanupGroup, FALSE, NULL);
	CloseHandle(hRequestEvent);
}

void RequestHandler::Destroy()
{
	if (this->runnable) {
		this->Stop();
	}
	if (this->tpCleanupGroup) {
		CloseThreadpoolCleanupGroup(this->tpCleanupGroup);
		this->tpCleanupGroup = NULL;
	}
	if (this->pThreadpool) {
		CloseThreadpool(this->pThreadpool);
		this->pThreadpool = NULL;
	}
	if (this->hTerminateBGWorkerThreadEvent) {
		CloseHandle(this->hTerminateBGWorkerThreadEvent);
		this->hTerminateBGWorkerThreadEvent = NULL;
	}
	this->runnable = false;
}

RequestHandler::~RequestHandler()
{
	this->Destroy();
}

void RequestHandler::HandleRequest(PCHTTP_REQUEST pRequest, size_t pRequestSize)
{
	if (this->requestCallbacksMap.contains(pRequest->Verb)) {
		return this->requestCallbacksMap[pRequest->Verb](this->hTargetRequestQueue, pRequest, pRequestSize);
	}
	else {
		this->defaultRequestCallback(this->hTargetRequestQueue, pRequest, pRequestSize);
	}
}