## Basic Information

The server's configuration name-value/values pairs which are later called properties are devided into two categories: string properties and list properties. String properties are those which have one string non-quoted value. On the other hand, list properties can have multiple values separated by zero or more white-space characters. All list properties are stored in a map that shouldn't be accessed directly. For setting and retrieving values shuld be used the listed below functions. All of them are under the `prop` namespace. This header contains functions related to the string properties. All of the listed functions are defined in `StringProperties.cpp`.

## Functions

- `SetStringProperty`

	- **Declaration**
			
			bool SetStringProperty(std::string const& propertyName, std::string const& value);
			
	- **Description**
	
		If `propertyName` is recognised as a string server configuration property and if `value` is a valid value for that property, sets `propertyName`'s value to `value` and returns `true`. Otherwise, returns false. For value validation is used `IsValidStringPropertyValue` which is described later.

- `GetStringProperty`

	- **Declaration**
	
			std::string const& GetStringProperty(std::string const& propertyName);
			
	- **Description**
	
		If `propertyName` is a name of a string property, returns its value, otherwise throws an exception.

- `GetStringPropertyW`

	- **Declaration**
	
			std::wstring GetStringPropertyW(std::string const& propertyName);
			
	- **Description**

		A Unicode widestring version of `GetStringProperty`. Requered because Windows Http Server API uses widechar strings.

- `GetMaxRequestThreadsProperty`

	- **Declaration**
	
			DWORD GetMaxRequestThreadsProperty();
			
	- **Description**

		A help function. Its purpose is to simplify retreiving the numeric value of the `MaxRequestThreads` property. if the value of `MaxRequestThreads` equals "unlimited", returns `500`, otherwise the value converted to a number.

- `IsKnownStringProperty`

	- **Declaration**

			bool IsKnownStringProperty(std::string const& propName)
			
	- **Description**

		Returns `true` if `propName` is recognised as a server configuration string property name, otherwise `false`.

- `IsValidStringPropertyValue`

	- **Declaration**

			bool IsValidStringPropertyValue(std::string const& propName, std::string const& propValue);
			
	- **Description**
	
		The basic validation function. If `propName` is not a name of a property, returns false. Otherwise, returns if `propValue` is a valid property value for the `propName` property. In order `value` to be valid, it must meet the following criterias:
		
		| The property name contained in `propName` | Criteria for `value` |
		|-------------------------------------------|----------------------|
		| `SchemeName` | Equals to `http` or `https` |
		| `HostName` | Not empty, equals to `+` or `*`, or starts with a letter and contains only alphanumeric characters and `.` |
		| `PortName` | A numeric value between `1` and `65535` |
		| `MaxRequestThreads` | A numeric value bigger than `0` or the value of `unlimited` |
