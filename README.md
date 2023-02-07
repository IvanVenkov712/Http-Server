A simple and light-weight Http server for Windows. This product is ideal for hosting static html-only websites. The program provides the basic functionality of a web server. Its simplicity make it perfect for unexperienced users with little knowledge in using servers. The server offers the opportunity to start your career as a web developer. You can also customize the program to fit your needs, add new features and contribute to the project in other various ways. I will appreciate each contribution.

Some of its notable features are:

- Asynchronous request processing
- Simple to no configuration (can be started without configuration file)
- Stability
- Supports both HTTP and HTTPS
- Server-site scripting is not supported, which makes it more secure.
- Runs on both x32 and x64 CPU architecture.
- Simple, tidy, clean and well-documented API with the possibility to easily add new features.

The program is also fast and not resource-consuming. For example, the memory usage can drop below 2 mb.

## Getting Started
Before using Http Server, you must ensure that the latest supported version of [Visual C++ Redistributable](https://learn.microsoft.com/en-US/cpp/windows/latest-supported-vc-redist?view=msvc-170) is installed on your machine.

Follow these steps to start Http Server for the first time.

1. Download from the [latest release](https://github.com/IvanVenkov712/Http-Server/releases) `Http_Server_x32.exe` or `Http_Server_x64.exe` depending on your architecture.
2. After that, create a folder named `PageDir` in the same path as the executable.
3. In `PageDir` create a file and name it `index.html`.
4. Paste the following HTML code into `index.html`:

        <html>
            <head>
                <title>Home</title>
            </head>
            <body>
                <h1>Hello from HTTP Server! This is my index page.</h1>
            </body>
        </html>

5. Now run the Command prompt as administrator, change the directory to that of the executable and type the following command: `"Http_Server_x32.exe" noconf` or respectively `"Http_Server_x64.exe" noconf`. This way you will start the server without configuration file. 
All properties are set to the [defaults](docs/user/configuration.md#default-property-values). 
An output like this:

        Info: Chosen schema:       http
        Info: Host name:           +
        Info: Port name:           80
        Info: Max request threads: unlimited

    indicates that the program has been started successfully.
6. Visit http://localhost:80 and you will see the `index` page on your browser.
7. To exit, you move the focus on the console and press <kbd>Ctrl</kbd>+<kbd>C</kbd>.

This short tutorial shows how easy is to use Http Server. This program will serve great on your hosting and testing purposes. After you learn how to use it, you will have the basics to learn using much more complicated web software.
You can learn how to configure the server [here](docs/user/configuration.md)

## Documentation

User documentation is available [here](docs/user).

Source documentation is available [here](docs/source).

## Contacts
You can contact me on email <ivan.venkov.developer@gmail.com>.
