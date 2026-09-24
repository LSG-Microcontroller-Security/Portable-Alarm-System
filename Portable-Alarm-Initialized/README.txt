Portable Alarm Initialized

Utility project for the Portable Alarm System, mainly used for testing.
Its primary purpose is the optional initialization of the product EEPROM with the default 
configuration values.
EEPROM initialization is not intended to run every time the main firmware is uploaded.
It should only be enabled when a test or a complete configuration reset is required.
The project also reads the stored configuration and displays diagnostic information on the 
Serial Monitor.
In particular, the Serial Monitor shows Bluetooth information, including the Bluetooth module 
version and other relevant parameters.
This project is mainly intended as an EEPROM initialization and diagnostic test tool for the 
Portable Alarm System.
