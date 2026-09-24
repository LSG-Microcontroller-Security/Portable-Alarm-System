#include "MyBlueTooth.h"
#include <Arduino.h>
MyBlueTooth::MyBlueTooth(uint8_t blueToothKeyPin,
	uint8_t baseTransistorPin,
	long baudRateProgramMode,
	long baudRateReceveMode,
	uint8_t rx, uint8_t tx) {
	_hardwareSerial = nullptr;
	_blueToothKeyPin = blueToothKeyPin;
	_baseTransistorPin = baseTransistorPin;
	_softwareSerial = new SoftwareSerial(rx, tx);

	_baudRateProgramMode = baudRateProgramMode;
	_baudRateReceveMode = baudRateReceveMode;

	pinMode(_blueToothKeyPin, OUTPUT);

	pinMode(_baseTransistorPin, OUTPUT);
}

MyBlueTooth::MyBlueTooth(HardwareSerial* hardwareSerial,
	uint8_t blueToothKeyPin,
	uint8_t baseTransistorPin,
	long baudRateProgramMode,
	long baudRateReceveMode) {
	_softwareSerial = nullptr;
	_blueToothKeyPin = blueToothKeyPin;
	_baseTransistorPin = baseTransistorPin;
	_hardwareSerial = hardwareSerial;
	_baudRateProgramMode = baudRateProgramMode;
	_baudRateReceveMode = baudRateReceveMode;
	pinMode(_blueToothKeyPin, OUTPUT);
	pinMode(_baseTransistorPin, OUTPUT);
}
MyBlueTooth::~MyBlueTooth() {
	delete(_softwareSerial);
	delete(_hardwareSerial);
}
void MyBlueTooth::println(String message) {
	if (_softwareSerial != nullptr) {
		_softwareSerial->println(message);
	}
	else {
		_hardwareSerial->println(message);
	}
}
void  MyBlueTooth::Clear2() {
	if (_softwareSerial != NULL) {
		//_softwareSerial->clearWriteError();
		//_hardwareSerial->flush();
		//_softwareSerial->end();
	}
	else {
		//_hardwareSerial->clearWriteError();
		//_hardwareSerial->flush();
		//_hardwareSerial->end();
	}
}
void  MyBlueTooth::Flush() {
	if (_softwareSerial != NULL) {
		_softwareSerial->flush();
	}
	else {
		_hardwareSerial->flush();
	}

}
bool  MyBlueTooth::IsDeviceDetected(String deviceAddress, String deviceName) {
		clearBuffer();
		_hardwareSerial->print("AT+RNAME?" + deviceAddress + "\r\n");
		delay(2000);
		String phoneName = _hardwareSerial->readString();
		String command = "+RNAME:" + deviceName;
		if (phoneName.indexOf(command) > -1) {
			return true;
		}
		else {
			return false;
		}
	return false;
}
void MyBlueTooth::Reset_To_Master_Mode() {
	ProgramMode();
	_hardwareSerial->println(F("AT+ROLE=1"));
	delay(500);
	_hardwareSerial->readString();
	_hardwareSerial->println(F("AT+CMODE=1"));
	delay(500);
	_hardwareSerial->readString();
	_hardwareSerial->println(F("AT+INQM=0,5,5"));
	delay(500);
	_hardwareSerial->readString();
	delay(1000);
	_hardwareSerial->readString();
}
void  MyBlueTooth::findModeV3() {
	digitalWrite(_baseTransistorPin, LOW);
	delay(2000);
	digitalWrite(_baseTransistorPin, HIGH);
	delay(3000);
	digitalWrite(_blueToothKeyPin, HIGH);
	this->begin(_baudRateReceveMode);
	delay(2000);
}
void  MyBlueTooth::ProgramMode() {
	digitalWrite(_blueToothKeyPin, HIGH);
	digitalWrite(_baseTransistorPin, LOW);
	delay(1000);
	digitalWrite(_baseTransistorPin, HIGH);
	delay(3000);
	this->begin(_baudRateProgramMode);
	delay(2000);
}
void MyBlueTooth::ReceveMode() {
	digitalWrite(_blueToothKeyPin, LOW);

	digitalWrite(_baseTransistorPin, LOW);

	delay(1000);

	digitalWrite(_baseTransistorPin, HIGH);

	delay(3000);

	this->begin(_baudRateReceveMode);

	delay(2000);
}
void MyBlueTooth::print(String message) {
	if (_softwareSerial != NULL) {
		_softwareSerial->print(message);
	}
	else {
		//_hardwareSerial->print(message);
		_hardwareSerial->print(message);
	}
}
void MyBlueTooth::begin(long baudrate) {
	if (_softwareSerial != NULL) {
		_softwareSerial->begin(baudrate);
	}
	else {
		_hardwareSerial->begin(baudrate);
	}
}
int MyBlueTooth::available() {
	if (_softwareSerial != NULL) {
		//Serial.println("Leggo");
		return _softwareSerial->available();
	}
	else {
		//return _hardwareSerial->available();
		return _hardwareSerial->available();
	}
}
String MyBlueTooth::readString() {
	if (_softwareSerial != NULL) {
		return _softwareSerial->readString();
	}
	else {
		//return _hardwareSerial->readString();
		return _hardwareSerial->readString();
	}
}
void MyBlueTooth::clearBuffer() {
	if (_softwareSerial != NULL) {
		for (int i = 0; i < 2; i++) {
			_softwareSerial->readString();
		}
	}
	else {
		for (int i = 0; i < 2; i++) {
			_hardwareSerial->readString();
		}
	}
}
void MyBlueTooth::turnOnBlueTooth() {
	digitalWrite(_baseTransistorPin, HIGH);
	blueToothOff = false;
	blueToothOn = true;
}
void MyBlueTooth::turnOffBlueTooth() {
	digitalWrite(_baseTransistorPin, LOW);
	blueToothOff = true;
	blueToothOn = false;
}
bool MyBlueTooth::isBlueToothOff() {
	return blueToothOff;
}
bool MyBlueTooth::isBlueToothOn() {
	return blueToothOn;
}
void MyBlueTooth::SetPassword(String password) {
	ProgramMode();
	println("AT+PSWD=" + password);
	delay(1000);
	print(readString());
	println("AT+UART=38400,0,0");
	delay(1000);
	print(readString());
	ReceveMode();
}

void MyBlueTooth::SetBlueToothName(String name) {
	ProgramMode();
	print("AT+NAME=" + name + "\r\n");
	delay(2000);
	readString();
	ReceveMode();
}

String MyBlueTooth::GetPassword() {
	_oldPassword.trim();

	if (_oldPassword == "") {
		ProgramMode();

		GetOldPassword();

		ReceveMode();
	}
	return _oldPassword;
}

String MyBlueTooth::GetPasswordV3() {
	_oldPassword.trim();

	if (_oldPassword == "") {
		ProgramMode();

		GetOldPasswordV3();

		ReceveMode();
	}
	return _oldPassword;
}

String MyBlueTooth::GetOldPassword() {
	_oldPassword = "";

	clearBuffer();

	println("AT+PSWD?");

	delay(5000);

	if (available() > 0) {
		String getData = readString();

		if (getData.startsWith(F("+PSWD:"))) {
			_oldPassword = getData.substring(6);
			_oldPassword = SplitStringIndex(_oldPassword, '\r', 0);
		}
		return _oldPassword;
	}
	return _oldPassword;
}

String MyBlueTooth::GetOldPasswordV3() {
	_oldPassword = "";

	clearBuffer();

	println("AT+PSWD?");

	delay(5000);

	if (available() > 0) {
		String getData = readString();

		if (getData.startsWith(F("+PIN:"))) {
			_oldPassword = getData.substring(5);
			_oldPassword = SplitStringIndex(_oldPassword, '\r', 0);
		}
		return _oldPassword;
	}
	return _oldPassword;
}

String MyBlueTooth::SplitStringIndex(String data, char separator, int index) {
	int found = 0;
	int strIndex[] = { 0, -1 };
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}
	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void MyBlueTooth::reset() {
	ProgramMode();
	this->println(F("AT+RESET"));
	delay(3000);
	ReceveMode();
}

String MyBlueTooth::getVersion() {
	ProgramMode();
	this->println(F("AT+VERSION?"));
	delay(1000);
	if (this->available() > 0) {
		ReceveMode();
		return this->readString();
	}
	ReceveMode();
}






