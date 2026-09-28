#include "MyBlueTooth.h"
//MyBlueTooth::MyBlueTooth(uint8_t blueToothKeyPin,
//	uint8_t baseTransistorPin,
//	long baudRateProgramMode,
//	long baudRateReceveMode,
//	uint8_t rx, uint8_t tx)
//{
//	_hardwareSerial = nullptr;
//	_blueToothKeyPin = blueToothKeyPin;
//	_baseTransistorPin = baseTransistorPin;
//	_softwareSerial = new SoftwareSerial(rx, tx);
//
//	_baudRateProgramMode = baudRateProgramMode;
//	_baudRateReceveMode = baudRateReceveMode;
//
//	pinMode(_blueToothKeyPin, OUTPUT);
//
//	pinMode(_baseTransistorPin, OUTPUT);
//}
MyBlueTooth::MyBlueTooth(
	uint8_t blueToothKeyPin,
	uint8_t baseTransistorPin,
	long baudRateProgramMode,
	long baudRateReceveMode) {
	/*_softwareSerial = nullptr;*/
	_blueToothKeyPin = blueToothKeyPin;
	_baseTransistorPin = baseTransistorPin;


	_baudRateProgramMode = baudRateProgramMode;
	_baudRateReceveMode = baudRateReceveMode;

	pinMode(_blueToothKeyPin, OUTPUT);
	pinMode(_baseTransistorPin, OUTPUT);
}

MyBlueTooth::~MyBlueTooth() {
	/*delete(_softwareSerial);*/
	//delete(_hardwareSerial);
}

void MyBlueTooth::println(String message) {
	/*if (_softwareSerial != nullptr)
	{
		_softwareSerial->println(message);
	}
	else*/
	//{
	Serial.println(message);
	//	}
}

void  MyBlueTooth::Clear2() {
	//if (_softwareSerial != NULL)
	//{
	//	//_softwareSerial->clearWriteError();
	//	//_hardwareSerial->flush();
	//	//_softwareSerial->end();
	//}
	//else
	//{
	//	//_hardwareSerial->clearWriteError();
	//	//_hardwareSerial->flush();
	//	//_hardwareSerial->end();
	//}
}

void  MyBlueTooth::Flush() {
	/*if (_softwareSerial != NULL)
	{
		_softwareSerial->flush();
	}
	else
	{*/
	Serial.flush();
	//}

}

bool  MyBlueTooth::IsDeviceDetected(String deviceAddress, String deviceName) {
	//if (_softwareSerial != nullptr)
	//{
	//	return;
	//	_softwareSerial->readString();
	//	_softwareSerial->println(F("AT+INQM=0,5,65"));
	//	_softwareSerial->readString();
	//	//delay(2500);
	//	_softwareSerial->println(F("AT+INQ"));
	//	//delay(2500);
	//	if (available() > 0)
	//	{
	//		_softwareSerial->readString();
	//		/*String command = "AT+RNAME?" + deviceAddress + "\r\n";*/
	//		_softwareSerial->print("AT+RNAME?" + deviceAddress + "\r\n");
	//		//delay(2500);
	//		String phoneName = _softwareSerial->readString();
	//		//delay(1500);
	//		String command = "+RNAME:" + deviceName;
	//		if (phoneName.indexOf(command) > -1)
	//		{
	//			return true;
	//		}
	//		else
	//		{
	//			return false;
	//		}
	//	}
	//}
	//else {
	clearBuffer();

	Serial.print("AT+RNAME?" + deviceAddress + "\r\n");

	delay(5000);

	String phoneName = Serial.readString();

	//Serial.println(phoneName);

	String command = "+RNAME:" + deviceName;

	if (phoneName.indexOf(command) > -1) {
		//Serial.println("trovato");
		return true;
	}
	else {
		return false;
	}
}

void  MyBlueTooth::Reset_To_Slave_Mode() {
	ProgramMode();

	//if (_softwareSerial != NULL)
	//{
	//	//_softwareSerial->println("AT+ORGL");
	//	//_softwareSerial->readString();
	//	delay(1500);
	//	//_softwareSerial->println("AT+RMAAD");
	//	//_softwareSerial->readString();
	//	//delay(1500);
	//	_softwareSerial->println(F("AT+ROLE=0"));
	//	_softwareSerial->readString();
	//	delay(1500);
	//	_softwareSerial->println("AT+UART=" + String(_baudRateReceveMode) + ",0,0");
	//	_softwareSerial->readString();

	//	delay(1500);
	//	ReceveMode();
	//	//Clear2();
	//}
	//else
	//{
	Serial.println(F("AT+ROLE=0"));

	Serial.readString();

	delay(1500);
	//UART STANDARD
	Serial.println(F("AT+UART=9600,0,0"));

	Serial.readString();

	delay(1500);

	ReceveMode();
}


void MyBlueTooth::Reset_To_Master_Mode() {
	ProgramMode();
	/*if (_softwareSerial != nullptr)
	{
		_softwareSerial->println(F("AT+ROLE=1"));
		_softwareSerial->readString();
		delay(500);
		_softwareSerial->println(F("AT+CMODE=0"));
		_softwareSerial->readString();
	}
	else
	{*/
	Serial.println(F("AT+ROLE=1"));

	delay(2000);

	Serial.println(F("AT+CMODE=0"));

	delay(2000);

}

void  MyBlueTooth::findModeV3() {
	digitalWrite(_baseTransistorPin, LOW);
	delay(2000);
	digitalWrite(_blueToothKeyPin, HIGH);
	delay(4000);
	digitalWrite(_baseTransistorPin, HIGH);
	delay(3000);
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
	/*if (_softwareSerial != NULL)
	{
		_softwareSerial->print(message);
	}
	else
	{*/
	//_hardwareSerial->print(message);
	Serial.println(message);
	//}
}

void MyBlueTooth::begin(long baudrate) {
	/*if (_softwareSerial != NULL)
	{
		_softwareSerial->begin(baudrate);
	}
	else
	{*/
	Serial.begin(baudrate);
	//}
}

int MyBlueTooth::available() {
	//if (_softwareSerial != NULL)
	//{
	//	//Serial.println("Leggo");
	//	return _softwareSerial->available();
	//}
	//else
	//{
		//return _hardwareSerial->available();
	return Serial.available();
	//}
}

String MyBlueTooth::readString() {
	/*if (_softwareSerial != NULL)
	{
		return _softwareSerial->readString();
	}
	else
	{*/
	//return _hardwareSerial->readString();
	return Serial.readString();
	//}
}

void MyBlueTooth::clearBuffer() {
	/*if (_softwareSerial != NULL)
	{
		for (int i = 0; i < 2; i++)
		{
			_softwareSerial->readString();
		}
	}
	else
	{*/
	for (int i = 0; i < 2; i++) {
		Serial.readString();
	}
	//}
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
	//println("AT+PSWD=" + password);
	Serial.print(F("AT+PSWD=")); Serial.println(password);
	delay(1000);
	print(readString());
	Serial.println(F("AT+UART=38400,0,0"));
	delay(1000);
	print(readString());
	ReceveMode();
}

void MyBlueTooth::SetBlueToothName(String name) {
	ProgramMode();
	Serial.print(F("AT+NAME=")); Serial.println(name);
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

	Serial.println(F("AT+PSWD?"));

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

	Serial.println(F("AT+PSWD?"));

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
	Serial.println(F("AT+RESET"));
	delay(3000);
	ReceveMode();
}

String MyBlueTooth::getVersion() {
	ProgramMode();
	Serial.println(F("AT+VERSION?"));
	delay(1000);
	if (Serial.available() > 0) {
		ReceveMode();
		return Serial.readString();
	}
	ReceveMode();
}






