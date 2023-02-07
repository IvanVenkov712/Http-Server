## Command Line
There are three variants of startup command line arguments:

- `path-to-program path-to-configuration-file`

    The path to the configuration file which will be read is provided explicitly.
 If the file can't be open or an error occurs, 
the server properties will be set to the defaults.
- `path-to-program [no options]`

    The program will try to open and read `Http Server.conf`.
If the file can't be open or an error occurs, 
the server properties will be set to the defaults.
- `path-to-program noconf`

    No configuration file is provided. All properties are set to the defaults.

After the server was started, it will output the chosen scheme, host name, port and maximal request threads count.
Also each error raised at the runtime of the program, will be shown to the user (the error code and additional information) 
as a console output.

## Configuration File
Configuration files are very important for the program's work. They define
its behavior and set essential values used in almost every aspect from the host name to the index pages. They typically have `.conf` extension but any extension or no extension can be used.

Configuration files have very simple syntax. As a delimiter 
between the property name and value is used the first
sequence of white-space characters. Each name-value 
pair must be present on a newline. The configuration file is case-sensitive. Invalid
property values and unknown properties are ignored. The valid values are described later.
Comments start with `#`.

## Properties
There are two categories of properties:
- String properties

    A single string value. At the moment these properties are: `SchemeName`, `HostName`, 
`PortName` and `MaxRequestThreads`.
    
    Example for string property: "www.example.com"
    
- List properties

    A list of quoted string values delimited by zero or more white-space characters.
Currently these properties are: `LookupDirs`, `IndexPageList`, `DefaultExtensions`.

    Example for list property: '"C:/Users/User1/Folder1" "C:/Users/User1/Folder2" "C:/Users/User1/Folder3" '

If some of these properties are not present in the configuration file, they will be set to the [default](#default-property-values).

## Property Meaning
- `SchemeName`

    The desired scheme defining the protocol used to transfer data (`http` or `https`).

- `HostName`

    The host of the instance of the program. It must be a valid DNS domain, Ip address or wildcard character.

- `PortName`

    The port on which the server will listen.

The properties above form the URLs which will the server listen on.
That's how the target URLs are formed: 
`SchemeName`://`HostName`:`PortName`/`[optional]absolute path`?`[optional]query string`

- `MaxRequestThreads`

    The maximal number of the threads which the program can create to process requests.

- `LookupDirs`

    A list of directories in which the server searches the files or paths specified by the 
request's URL. The directories are checked in the order in which they were listed in the configuration file.

- `IndexPageList`

    A list of valid filenames which will be appended to the URL's absolute path if it points to a directory.
The first matching file (`URL absolute path`/`IndexPageList[n]`) if one will be sent to the client as
a response. The filenames will be appended in the order in which they were listed in the configuration file.

- `DefaultExtensions`

    A list of valid file extensions which will be appended to the URL's absolute path if it point to a file
and that file wasn't found. The first matching file (`URL absolute path` + `DefaultExtensions[n]`) if any will
be sent to the client as a response. The extensions will be appended in the order in which they were listed in
the configuration file.

## Default Property Values

| Property Name     | Value        |
|-------------------|--------------|
| SchemeName        | http         |
| HostName          | +            |
| PortName          | 80           |
| MaxRequestThreads | unlimited    |
| LookupDirs        | "PageDir"    |
| IndexPageList     | "index.html" |
| DefaultExtensions | ".html"      |

## Source Code

For a source code documentation, visit [source](source).
