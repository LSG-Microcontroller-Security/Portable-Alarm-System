#pragma once
#include <Arduino.h>
class MyBlueTooth{
public:
	/*MyBlueTooth(uint8_t blueToothKeyPin,
				uint8_t baseTransistorPin,
				long baudRateProgramMode,
				long baudRateReceveMode,
				uint8_t rx, uint8_t tx);*/
	MyBlueTooth(uint8_t blueToothKeyPin,uint8_t baseTransistorPin,long baudRateProgramMode,long baudRateReceveMode);
	~MyBlueTooth();
	void begin(long baudrate);
	void print(String message);
	void println(String message);
	String GetPassword();
	String GetPasswordV3();
	void SetPassword(String password);
	int available();
	String readString();
	bool isBlueToothOff();
	bool isBlueToothOn();
	void turnOffBlueTooth();
	void turnOnBlueTooth();
	void findModeV3();
	void ReceveMode();
	void  ProgramMode();
	void reset();
	void  Reset_To_Slave_Mode();
	void  Reset_To_Master_Mode();
	void SetBlueToothName(String name);
	void Clear2();
	void clearBuffer();
	void Flush();
	String getVersion();
	//Activate master mode before call this method
	bool  IsDeviceDetected(String deviceAddress, String deviceName);
	private:
	uint8_t _blueToothKeyPin;
	uint8_t _baseTransistorPin;
	String _oldPassword;
	long _baudRateProgramMode;
	long _baudRateReceveMode;
	bool blueToothOn;
	bool blueToothOff;
	String SplitStringIndex(String data, char separator, int index);
	String GetOldPassword();
	String GetOldPasswordV3();
};

