## Basic Information

The header `FileControl.h` contains functions related to 
the file naming, file extensions and Mime types.

## Functions

- `GetMimeType`

    - **Declaration**

            std::string GetMimeType(const std::string& extension);

    - **Description**
        
        This function transforms the given file extension to a MIME type, used in the response header `Content-Type`.
        The MIME type string is searched in the Registry.
        
    - **Parameters**
    
        - extension
        
            A file extension. for example '.html', '.exe' and etc.
    
    - **Returns**
        
        A string containing the MIME type for the chosen extension. If a corresponding MIME type for the extension wasn't found,
        the function returns "application/octet-stream".
        
    - **Definition**
        
        Defined in `FileControl.cpp`.

- `EncodeUrl`

    **Note: Currently not used in the project.**

    - **Declaration**

            template <typename ChType>
            std::basic_string<ChType> EncodeUrl(std::basic_string<ChType> const& value);

    - **Description**

        Encodes the special HTML characters in the URL contained in `value`. It
        returns a string which contains the `%` - encoded characters.
        The template parameter `ChType` denotes the character type of the input and
        the returned strings.
		
	- **Parameters**
		- value 
			
			must contain a valid URL address.
			
    - **Definition**

        Defined in `FileControl.h`.

- `DecodeUrl`

    **Note: Currently not used in the project.**

    - **Declaration**

            template <typename ChType>
            std::basic_string<ChType> DecodeUrl(std::basic_string<ChType> const& value);

    - **Description**

        Decodes the `%` - encoded special HTML characters in the url contained `value`.
        The template parameter `ChType` denotes the character type of the input and
        the returned strings.
		
	- **Parameters**
		- value 
			
			must contain a valid URL address.
			
    - **Definition**

        Defined in `FileControl.h`.
        
- `FilePathFromUrl`

    **Note: Currently not used in the project.**
    
    - **Declaration**
    
            std::filesystem::path FilePathFromUrl(std::wstring const& parent, std::wstring const& urlAbsPath);
        
    - **Description**

        Creates a file path, a combination from 'parent' and 'urlAbsPath'. 
        The query string, if there is one, is ignored.
        
    - **Parameters**
        - parent

            A valid path to a directory.
         
         - UrlAbsPath

            The absolute path of a url.
            For example, in the given url 'http://hostname:port/sub1/sub2/file.html?param1=value1',
            the absolute path will is 'sub1/sub2/file.html?param1=value1'. The query string will be ignored.
            
    - **Returns**
    
        The combined path from 'parent' and 'urlAbsPath'
        
    - **Definition**
        
        Defined in `FileControl.cpp`.
