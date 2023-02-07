## Used technologies

This project is completely written in C++ 20. MSVC is used to build
the binary. The project has dependencies on the C++ 20's standatd library.
No third-party libraries are used.
This program is intended to be used only on Microsoft Windows x32 and x64.
There is a different binary for x32 and x64 CPU architecture.

The web services of the project are implemented with the help of
Windows HTTP Server API, version 2.

For the asynchronous request processing, Windows Threadpool API and 
Windows Synchronization API are used.

## Components

There are three major compinents which construct the program:
- **Request processing**

    The request processing is made up using instances of the `RequestHandler`
class. It is implemented to handle requests asynchronously with the
help of `RequestCallback` callbacks which are executed each time
the server recieves a http request. `RequestHandler` offers the opportunity
to set and remove a callback for a specific request method (verb). It can also
be set a default callback which is executed when there hasn't been set a callback for
the specific HTTP verb. `RequestHandler` internally uses a threadpool.

    `RequestCallback` has also the capability of being started, stopped and
destroyed at each time.

    The class is declared in `RequestHandler.h` and
its methods are defined in `RequestHandler.cpp`.

- **Configuration Properties**

    The mechanisms of setting and retrieving property values are
implemented in `StringProperties.h`, `StringProperties.cpp`, `ListProperties.h`,
`ListProperties.cpp`, `Properties.h` and `Properties.cpp`. Each property-related
function is found under the namespace `prop`.

- **Error Handling**

    Each Winapi function failure is handled using the class `Win32Exception` defined
in the `Win32Exception.h` header. It is derived from `std::exception` class. Its purpose
is to provide more additional information about the failure that occurred. Internally
uses `FormatMessage`.

## Further Reading

The information is grouped by header.
For detailed description of each class, method, function used, visit the below links:

[FileControl.h.md](headers/FileControl.h.md)<br>
[StringProperties.h.md](headers/StringProperties.h.md)<br>
[ListProperties.h.md](headers/ListProperties.h.md)<br>
[ReportError.h.md](headers/ReportError.h.md)<br>
[RequestHandler.h.md](headers/RequestHandler.h.md)<br>
[RequestCallbacks.h.md](headers/RequestCallbacks.h.md)<br>
[Properties.h.md](headers/Properties.h.md)<br>
[Win32Exception.h.md](headers/Win32Exception.h.md)<br>






