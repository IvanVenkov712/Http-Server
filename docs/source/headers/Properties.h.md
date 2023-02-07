## Basic Information

Contains the declaration of `SetPropertiesFromConfFile` and `SetPropertiesDefault`. A header that combines the `ListProperties.h` and `StringProperties.h` definitions. The functions declared in this header are defined in `Properties.cpp`.

## Functions

- `SetPropertiesDefault`

	- **Declaration**
	
			void SetPropertiesDefault();
			
	- **Description**
		
		Sets both list and string properties to the [default](/docs/README.md#default-property-values) values.
		
- `SetPropertiesFromConfFile`

	- **Declaration**

			std::string SetPropertiesFromConfFile(std::string const& confFilePath) noexcept;
			
	- **Description**
	
		Parses the configuration file pointed by `confFilePath` if exists and sets both list and string property values according to its content. Read more about configuration file syntax [here](/docs/README.md#Configuration%20File).<br>
		The function returns a report containing information about the file parsing results. In it information anout the chosen scheme, hostname, port and max threads count is also provided. When the same property is set more than once, the first occurrence is accepted and the others are ignored. Unrecognised property names and invalid values are also ignored and noted in the returned report.<br>
		`SetPropertiesFromConfFile` is intended not to throw. If an unrecoverable error occurs, all properties are set to the default.
