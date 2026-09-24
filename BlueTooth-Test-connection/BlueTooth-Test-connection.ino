#include "MyBlueTooth.h"

const uint8_t bluetoothKeyPin = 10;
const uint8_t bluetoothTransistorPin = 6;

MyBlueTooth btSerial(
	&Serial,
	bluetoothKeyPin,
	bluetoothTransistorPin,
	38400,
	9600
);

void setup() {
	//---Used only with the V3 version when inquiry mode is required.
	//btSerial.Reset_To_Slave_Mode();
	//---Used only with the V3 version when inquiry mode is required.
	//btSerial.findModeV3();
	
	//--- Used only with the V2 and V4 version when inquiry mode is required.
	btSerial.Reset_To_Master_Mode();
}
void loop() {
	//Always use 
	bool found = btSerial.IsDeviceDetected(
		"6045,CB,3122BD",
		"ASUS_Z017D"
	);
	Serial.println(found);
	delay(2000);
}