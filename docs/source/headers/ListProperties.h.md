## Basic Information
The server's configuration name-value/values pairs which are later called properties are devided into two categories: string properties and list properties. String properties are those which have one string non-quoted value. On the other hand, list properties can have multiple values separated by zero or more white-space characters.  All list properties are stored in a map that shouldn't be accessed directly. For setting and retrieving values shuld be used the listed below functions. All of them are under the `prop` namespace. This header contains functions related with the list properties. All of the listed functions are defined in `ListProperties.cpp`.

## Functions

- `SetListProperty`

	- **Declaration**
	
		- **Overloads**
		
			- (1)
			
					bool SetListProperty(std::string const& propertyName, std::string const& value);
			
			- (2)
			
					bool SetListProperty(std::string const& propertyName, std::vector<std::string> const& value);
					bool SetListProperty(std::string const& propertyName, std::vector<std::string>&& value);

	- **Description**
	
		If `value` is valid, the `propertyName` is set to it and `true` is returned. Othrewise, the function returns `false`. For validation of `value` is used `IsValidListPropertyValue` which is described later.
		
		- (1)

			`value` must be a list of quoted strings delimited by zero or more spaces. Each string must be a valid value for the `propertyName` property. Example: If `value` equals to: "abcd" "efgh" "ijkl", then each of the three quoted strings ("abcd", "efgh" and "ijkl") must be a valid value for `propertyName`.
			
		- (2)
		
			Each of `value`'s elements must be a valid value for the `propertyName` property.
		
	

- `GetListProperty`
	
	- **Declaration**
	
			std::vector<std::string> const& GetListProperty(std::string const& propertyName);
			
	- **Description**

		Returns the `propertyName`'s value is found. Otherwise, throws an exception. The vector returned by `GetListProperty` shouldn't be modified

- `IsKnownListProperty`

	- **Declaration**
		
			bool IsKnownListProperty(std::string const& propName);
			
	- **Description**
	
		Returns `true` is `propName` is recognised as a server configuration property name, otherwise `false`.

- `IsValidListPropertyValue`
	
	- **Declaration**
		
			bool IsValidListPropertyValue(std::string const& propName, std::vector<std::string> const& value);
				
	- **Description**
		
		This is the basic validation function. It returns `true` if `value` is a valid property value for `propName`, otherwise `false`. If `propName` wasn't recognised as a property name, the function returns `false`. Criterias that `value` must meet in order to be valid:
		
		| Property name in `propName`      | Criteria for `value`                            |
		|----------------------------------|-------------------------------------------------|
		| `LookuDirs`                      | Each of `value`'s elements has to be a path to existing and accessible directory. |
		| `IndexPageList`                  | Each of `value`'s elements needs to be a valid filename for Windows (doesn't contain any of following characters: `\/:*?"<>\|`). |
		| `DefaultExtensions`              | Each of `value`'s elements has to be a valid file extension (starts with `'.'` and doesn't contain any of the characters listed in the criteria for `IndexPageList`). |
