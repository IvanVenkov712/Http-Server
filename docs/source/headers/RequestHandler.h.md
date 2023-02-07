## Basic Information

This header contains the declaration of the `RequestHandler` class which is an essential component of the project. `RequestHandler`'s purpose is to provide confortable mechanisms for handling incoming client HTTP Requests using callbacks. It is implemented to handle all requests asynchronously. There are two kinds of instances: runnable and non-runnable. Runnable are those instances on which `Start` and `Stop` can be called. `RequestHandler` class implements the move semantics. All of `RequestHandler`'s methods are defined in the `RequestHandler.cpp` source file.

## Classes

- `RequestHandler`
	
	- **Types**
		
		- `RequestCallback`
		
			- **Declaration**
			
					using RequestCallback = std::function<void(HANDLE hRequestQueue, PCHTTP_REQUEST pRequest, size_t pRequestSize)>;
					
			- **Description**
			
				Represents a callback (callable object) used to handle a request. Each callable object must meet the following criterias in order to be used to handle requests:<br>
				- The return type of the overloaded call operator (`()`) must be `void`.<br>
				- It must take as parameters `HANDLE`, `PCHTTP_REQUEST` (`const HTTP_REQUEST*`) and `size_t`. The meaning of the parameters as named in the declaration is:
					- `hRequestQueue`

						A handle to the request queue which the instance of the `RequestHandler` class handles. The callback has to send a response using this queue. It mustn't call `HttpCloseRequestQueue` on that handle or modify it in any other way.
						
					- `pRequest`

						This parameter points to a `HTTP_REQUESTS` structure which occupies `pRequestSize` bytes. That structure contains essential information for the received request that the callback must handle (the request headers, method and id). The function object mustn't deallocate the memory pointed to by `pRequest` or modify it in any other way.  If necessary, the memory can be copied.
				- In order to communicate with the client, to receive additional data or to do another job related to `hRequestQueue` and `pRequest`, the callback must use the Windows Http Server API Version 2.0 functions.
	
	- **Constructor**

		1. `(default)`
		
			Constructs a non-runnable instance of `RequestHandler` not associated with any request queue.
			
		2. `(move)`

		3. `RequestHandler(HANDLE hRequestQueue, DWORD minThreadsCount, DWORD maxThreadsCount)`

			Constructs a runnable instance of `RequestHandler` which will handle requests coming from `hRequestQueue`.<br>
			`hRequestQueue` must be a handle to a request queue created with the Winapi function `HttpCreateRequestQueue`.<br>
			`minThreadsCount` represents the minimal number of threads created in order to start processing requests. Its default value is `1`.<br>
			`maxThreadsCount` is the maximal number of threads that can be created. Has default value `std::thread::hardware_concurrency()`.<br>
			The instance won't start processing requests immediately. You must call `Start` on it.
			
	- **Destructor**

		Calls `Destroy`.
			
	- **Methods**
	
		- **Public**

			- `Start`

				- **Declaration**
				
						void RequestHandler::Start();
					
				- **Description**

					Causes a runnable instance to start processing HTTP requests in the background until `Stop` is called or the object is destroyed. After an object was stopped with `Stop`, a call to `Start` causes it to resume its work. If the instance is already running, `Start` has no effect. Does not block the main thread. If called on a non-runnable instance throws exceprion.

			- `Stop`

				- **Declaration**
				
						void RequestHandler::Stop();
						
				- **Description**
				
					Stops the job of a running runnable object. If the instance is already stopped, has no effect. Throws an exception when called on a non-runnable object.
		
			- `SetRequestCallback`

				- **Declaration**
				
						void RequestHandler::SetRequestCallback(HTTP_VERB handledVerb, RequestCallback const& handlerFunc);
						
				- **Description**

					Sets a callback `handlerFunc` handling all HTTP requests with `handledVerb` verb (method). That means each time such a request is received, the callback gets called. As `pRequest` is passed a pointer to a `HTTP_REQUEST` structure with size `pRequestSize` bytes containing the method, the id and the headers of the received request. `hRequestQueue` is the handle to the request queue. If the verb is already associated with another callback, that callback is replaced by `handlerFunc`.

			- `SetDefaultRequestCallback`

				- **Declaration**
				
						void RequestHandler::SetDefaultRequestCallback(RequestCallback const& handlerFunc);
						
				- **Description**

					Sets a default callback called when there is no another functor associated with the received request's verb. By default, a status `503 NOT IMPLEMENTED` is sent on such requests.

			- `RemoveRequestCallback`

				- **Declaration**
				
						void RequestHandler::RemoveRequestCallback(HTTP_VERB verb);
						
				- **Description**

					Removes the callback associated with `verb` if there is one. After that, requests with verb (method) `verb` will be handled by the default request callback.
			
			- `Runnable`
				- **Declaration**

						bool RequestHandler::Runnable() const;

				- **Description**

					Checks whether the object is runnable or not.
			- `Running`
				
				- **Declaration**
				
						bool RequestHandler::Running() const;
						
				- **Description**

					Checks whether the instance is running in the background. Returns `false` for non-runnable objects.

		- **Private**

			- `BGWorkerThreadRoutine`

				- **Declaration**
					
						void RequestHandler::BGWorkerThreadRoutine();
						
				- **Description**
					
					Maps HTTP requests coming from the instance's request queue to the callbacks associated with the request's verb asynchronously. If a non-fatal error occurs, reports it using `ReportError`. As the name suggests, it's intended to work in the background (concurrently with the other program's threads).
				
			- `HandleRequest`
			- `Destroy`
				
				Frees any resources held by the instance. Its request queue remains valid even after a call to this method. If the object's background worker is still running, calls `Stop` and after that does the cleanup. After it was called, the instance isn't runnable anymore.








