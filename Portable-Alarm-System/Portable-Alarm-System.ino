#if (defined(__AVR__))
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif
#ifndef _ON_MOCKING_TESTS
#define _ON_MOCKING_TESTS 0U
#endif
//#include <MemoryFree.h>
//#include <pgmStrToRAM.h>
#include <mf_commons_commonsLayer.h>
#include <mf_adapter_HardwareSerialAdapter.h>
#include <mf_adapter_SoftwareSerialAdapter.h>
#include <mf_repository_AvrMicroRepository.h>
#include <mf_repository_BlueToothRepository.h>
#include <LSGEEpromRW.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <MySim900.h>
#include <ActivityManager.h>
#include "BluetoothCommandUtil.h"
#include "BluetoothFrameWriter.h"
#include "BluetoothDynamicMenu.h"
#include "mf_repository_SimRepository.h"

#if _DEBUG_FOR_SERIAL
#define DEBUG_SERIAL_PRINT(...) do { Serial.print(__VA_ARGS__); } while (0)
#define DEBUG_SERIAL_PRINTLN(...) do { Serial.println(__VA_ARGS__); } while (0)
#else
#define DEBUG_SERIAL_PRINT(...) do { } while (0)
#define DEBUG_SERIAL_PRINTLN(...) do { } while (0)
#endif
char version[15] = "S001 7.86-RTM";
//Library version : 6.55-RTM
ActivityManager _delay_for_temperature(60);
ActivityManager _delay_for_voltage(60);
char _old_password[5] = {};
char _new_password[5] = {};
#pragma region pinsDefinition
const byte _pin_powerLed = 13;
const uint8_t _pin_pir = A5;
const uint8_t _pin_buzzer = 5;
const byte _pin_rxSIM900 = 7;
const byte _pin_txSIM900 = 8;
const byte _pin_reedRelay = A4;
MySim900 my_sim900(_pin_rxSIM900, _pin_txSIM900, false);
SoftwareSerial sim_serial(_pin_rxSIM900, _pin_txSIM900, false);
SoftwareSerialAdapter sim_serial_adapter(sim_serial);
SimRepository sim_repository(sim_serial_adapter);
#pragma endregion pinsDefinition
HardwareSerialAdapter bluetooth_serial_adapter(Serial);
AvrMicroRepository bluetooth_avr_repository(bluetooth_serial_adapter, mf::commons::commonsLayer::AnalogRefMode::DEFAULT_m, 5.0f);
BlueToothRepository bluetooth_repository(bluetooth_avr_repository, 10, 6, 38400, 9600);
BluetoothFrameWriter bluetooth_frame_writer(bluetooth_repository);
const byte _addressStartBufPhoneNumber = 1;
const byte _addressStartBufPrecisionNumber = 12;
const byte _addressStartBufTemperatureIsOn = 14;
const byte _addressStartBufTemperatureMax = 16;
const byte _addressStartBufPirSensorIsON = 19;
const byte _addressStartDeviceAddress = 21;
const byte _addressStartDeviceName = 36;
const byte _addressStartFindOutPhonesON = 51;
const byte _addressStartBTSleepIsON = 53;
const byte _addressDBPhoneIsON = 55;
const byte _addressStartBufPhoneNumberAlternative = 57;
const byte _addressStartFindMode = 68;
const byte _addressApn = 70;
const byte _addressOffSetTemperature = 95;
const byte _addressDelayFindMe = 100;
const byte _addressExternalInterruptIsOn = 102;
const byte _addressStartDeviceAddress2 = 104;
const byte _addressStartDeviceName2 = 119;
const byte _addressBuzzerIsOn = 134;
uint8_t _isPIRSensorActivated = 0;
bool _isBlueLedDisable = true;
bool _isDisableCall = false;
bool _isOnMotionDetect = false;
bool _isOnExternalMotionDetect = false;
bool _isPositionEnable = false;
unsigned long _sensitivityAlarm;
char _prefix[4] = "+39";
bool _isAlarmOn = false;
char _phoneNumber[11];
char _phoneNumberAlternative[11];
char _what_is_happened[2] = {};
uint8_t _isBTSleepON = 1;
uint8_t _isExternalInterruptOn = 0;
uint8_t _isBuzzerOn = 0;
uint8_t _phoneNumbers = 0;
uint8_t _findOutPhonesMode = 0;
uint8_t _tempMax = 0;
uint8_t _delayFindMe = 1;
unsigned int _offSetTempValue = 324;
float _voltageValue = 0;
float _voltageMinValue = 0;
bool _isMasterMode = false;
bool _isExtenalInterruptNormalyClosed = true;
unsigned long _timeToTurnOnAlarm = millis() + 300000;
//String _apn = "";
bool _isDeviceDetected = false;
const int BUFSIZEPHONENUMBER = 11;
const int BUFSIZEPHONENUMBERALTERANATIVE = 11;
const int BUFSIZEPIRSENSORISON = 2;
char _bufPirSensorIsON[BUFSIZEPIRSENSORISON];
const int BUFSIZEFINDOUTPHONESON = 2;
char _bufFindOutPhonesON[BUFSIZEFINDOUTPHONESON];
const int BUFSIZEDBPHONEON = 2;
char _bufDbPhoneON[BUFSIZEDBPHONEON];
const int BUFSIZETEMPERATUREMAX = 3;
char _bufTemperatureMax[BUFSIZETEMPERATUREMAX];
const int BUFSIZEDEVICEADDRESS = 15;
char _bufDeviceAddress[BUFSIZEDEVICEADDRESS];
char _bufDeviceAddress2[BUFSIZEDEVICEADDRESS];
const int BUFSIZEDEVICENAME = 15;
char _bufDeviceName[BUFSIZEDEVICENAME];
char _bufDeviceName2[BUFSIZEDEVICENAME];
const int BUFSIZEAPN = 25;
char _bufApn[BUFSIZEAPN];
const int BUFSIZEOFFSETTEMPERATURE = 5;
char _bufOffSetTemperature[BUFSIZEOFFSETTEMPERATURE];
const int BUFSIZEDELAYFINDME = 2;
char _bufDelayFindMe[BUFSIZEDELAYFINDME];
const int BUFSIZEEXTERNALINTERRUPTISON = 2;
char _bufExternalInterruptIsON[BUFSIZEEXTERNALINTERRUPTISON];
const int BUFSIZEBUZZERISON = 2;
char _bufBuzzerIsON[BUFSIZEBUZZERISON];
void setup() {
	my_sim900.Begin(19200);
	my_sim900.IsCallDisabled(false);
	inizializePins();
	inizializeInterrupts();
	bluetooth_repository.set_to_slave_mode();
	//my_sim900.getCCLK();
	char current_password[5] = {};
	bluetooth_repository.get_current_password(current_password, sizeof(current_password));
	memcpy(_old_password, current_password, sizeof(_old_password));
	//Serial.print("oldPassword : "); Serial.println(_old_password);
	bluetooth_repository.set_to_slave_mode();
	initilizeEEPromData();
	if (_findOutPhonesMode != 0) {
		_isBTSleepON = 0;
		bluetooth_frame_writer.send_program_frame(PSTR("Find activated"), BluetoothCommandUtil::Message);
		bluetooth_frame_writer.send_end();
	}
	bluetooth_repository.turnOnBlueTooth();
	_what_is_happened[0] = 'X';
	my_sim900.ATCommand("AT+CNETLIGHT=0");
	delay(500);
	my_sim900.ATCommand("AT+CPMS=\"SM\"");
	delay(500);
	/*if (my_sim900.IsAvailable() > 0)
	{
		String s = my_sim900.ReadIncomingChars2();
		Serial.println(s);
	}*/
	my_sim900.ATCommand("AT+CMGF=1");
	delay(500);
	/*delay(5000);
	if (my_sim900.IsAvailable() > 0)
	{
		String s = my_sim900.ReadIncomingChars2();
		Serial.println(s);
	}*/
	my_sim900.ATCommand("AT+CMGD=1,4");
	/*if (my_sim900.IsAvailable() > 0)
	{
		String s = my_sim900.ReadIncomingChars2();
		Serial.println(s);
	}*/
	delay(1000);
	my_sim900.ATCommand("AT+CBAND=""EGSM_PCS_MODE""");
	delay(1000);
	if (my_sim900.IsAvailable() > 0) {
		String s = my_sim900.ReadIncomingChars2();
		DEBUG_SERIAL_PRINTLN(s);
	}
	pinMode(_pin_pir, INPUT_PULLUP);
	blinkLedHideMode();
}
void initilizeEEPromData() {
	LSG_EEpromRW eeprom_rw;
	eeprom_rw.eeprom_read_string(_addressStartBufPhoneNumber, _phoneNumber, BUFSIZEPHONENUMBER);
	eeprom_rw.eeprom_read_string(_addressStartBufPhoneNumberAlternative, _phoneNumberAlternative, BUFSIZEPHONENUMBERALTERANATIVE);
	eeprom_rw.eeprom_read_string(_addressDBPhoneIsON, _bufDbPhoneON, BUFSIZEDBPHONEON);
	_phoneNumbers = atoi(&_bufDbPhoneON[0]);
	eeprom_rw.eeprom_read_string(_addressStartFindOutPhonesON, _bufFindOutPhonesON, BUFSIZEFINDOUTPHONESON);
	_findOutPhonesMode = atoi(&_bufFindOutPhonesON[0]);
	eeprom_rw.eeprom_read_string(_addressStartBufPirSensorIsON, _bufPirSensorIsON, BUFSIZEPIRSENSORISON);
	_isPIRSensorActivated = atoi(&_bufPirSensorIsON[0]);
	eeprom_rw.eeprom_read_string(_addressStartBufTemperatureMax, _bufTemperatureMax, BUFSIZETEMPERATUREMAX);
	_tempMax = atoi(_bufTemperatureMax);
	eeprom_rw.eeprom_read_string(_addressStartDeviceAddress, _bufDeviceAddress, BUFSIZEDEVICEADDRESS);
	eeprom_rw.eeprom_read_string(_addressStartDeviceName, _bufDeviceName, BUFSIZEDEVICENAME);
	eeprom_rw.eeprom_read_string(_addressStartDeviceAddress2, _bufDeviceAddress2, BUFSIZEDEVICEADDRESS);
	eeprom_rw.eeprom_read_string(_addressStartDeviceName2, _bufDeviceName2, BUFSIZEDEVICENAME);
	eeprom_rw.eeprom_read_string(_addressOffSetTemperature, _bufOffSetTemperature, BUFSIZEOFFSETTEMPERATURE);
	_offSetTempValue = atoi(_bufOffSetTemperature);
	eeprom_rw.eeprom_read_string(_addressDelayFindMe, _bufDelayFindMe, BUFSIZEDELAYFINDME);
	_delayFindMe = atoi(_bufDelayFindMe);
	eeprom_rw.eeprom_read_string(_addressExternalInterruptIsOn, _bufExternalInterruptIsON, BUFSIZEEXTERNALINTERRUPTISON);
	_isExternalInterruptOn = atoi(&_bufExternalInterruptIsON[0]);
	eeprom_rw.eeprom_read_string(_addressBuzzerIsOn, _bufBuzzerIsON, BUFSIZEBUZZERISON);
	_isBuzzerOn = atoi(&_bufBuzzerIsON[0]);
}
void inizializePins() {
	pinMode(_pin_powerLed, OUTPUT);
	pinMode(0, INPUT_PULLUP);
}
void inizializeInterrupts() {
	attachInterrupt(0, motionTiltInternalInterrupt, RISING);
	attachInterrupt(1, motionTiltExternalInterrupt, CHANGE);
}
void callSim900() {
	//Serial.println("Faccio chiamata");
	if (_isDisableCall) { return; }
	char phoneNumber[14];
	strcpy(phoneNumber, _prefix);
	if (_phoneNumbers == 1) {
		strcat(phoneNumber, _phoneNumber);
	}
	if (_phoneNumbers == 2) {
		strcat(phoneNumber, _phoneNumberAlternative);
	}
	my_sim900.DialVoiceCall(phoneNumber);
	delay(1000);
	//Inserita per scaricare buffer dopo chiamata
	//dove si puo aggiungere codice per recupero risultato.
	//E agevola la pulizia per la ricezione sms.
	my_sim900.ReadIncomingChars2();
}
void motionTiltExternalInterrupt() {
	if (_isExternalInterruptOn /*&& !_isPIRSensorActivated*/) {
		_isOnExternalMotionDetect = true;
	}
}
void motionTiltInternalInterrupt() {
	if (!_isPIRSensorActivated) {
		_isOnMotionDetect = true;
	}
}
void turnOffBluetoohIfTimeIsOver() {
	if (_findOutPhonesMode == 0
		&& (millis() > _timeToTurnOnAlarm)
		&& bluetooth_repository.isBluetoothOn()
		&& _isBTSleepON
		) {
		bluetooth_repository.turnOffBlueTooth();
	}
}
//void turnOnBlueToothIfMotionIsDetected()
//{
//	if (_isOnMotionDetect
//		&& !_isAlarmOn
//		&& !bluetooth_repository.isBluetoothOn()
//		&& _isBTSleepON
//		)
//	{
//		_isOnMotionDetect = false;
//		turnOnBlueToothAndSetTurnOffTimer(false);
//	}
//}
void findOutPhonesONAndSetBluetoothInMasterModeActivity() {
	if (_isDisableCall) { return; }
	//todo:da mettere in altro luogo.

	/*if ((_findOutPhonesMode == 1 || _findOutPhonesMode == 2) && _isAlarmOn)
	{*/
	/*	if (_findOutPhonesMode == 1 && !_isAlarmOn)
		{
			_isAlarmOn = true;
		}*/

	if (_isMasterMode == false) {
		bluetooth_repository.set_to_master_mode_v2();
		_isMasterMode = true;
	}

	for (uint8_t i = 0; i < _delayFindMe; i++) {
		if (_phoneNumbers == 1) {
			_isDeviceDetected = bluetooth_repository.is_device_detected(_bufDeviceAddress, _bufDeviceName);
			if (_isDeviceDetected) {
				break;
				//Serial.println("Find first BT");
			}
		}

		/*	if (_findOutPhonesMode == 1)
			{*/

		if (_phoneNumbers == 2) {
			_isDeviceDetected = bluetooth_repository.is_device_detected(_bufDeviceAddress2, _bufDeviceName2);
			if (_isDeviceDetected) {
				//Serial.println("Find second BT");
				break;
			};
		}
		//}
	}

	if (_isDeviceDetected) {
		blinkLedHideMode();
		//reedRelaySensorActivity(_pin_reedRelay);
	}
	else {
		if (_findOutPhonesMode == 2) {
			callSim900();
			_isMasterMode = false;
		}
	}
	//return _isDeviceDetected;
	//}
}
void loop() {
	readIncomingSMS();

	if ((millis() > _timeToTurnOnAlarm) && _isAlarmOn != true) {
		_isAlarmOn = true;
	}

	//if (!(_isOnMotionDetect && _isAlarmOn))
	//{
	//	readIncomingSMS();
	//}

	//if (_delayForSignalStrength->IsDelayTimeFinished(true))
	//{
	//	getSignalStrength();
	//}

	if (_isAlarmOn && (_findOutPhonesMode == 1 || _findOutPhonesMode == 2)) {
		findOutPhonesONAndSetBluetoothInMasterModeActivity();
	}

	//if (_delayForCallNumbers->IsDelayTimeFinished(true))
	//{
	//	_callNumbers = 0;
	//}

	if (!(_isOnMotionDetect && _isAlarmOn)) {
		turnOffBluetoohIfTimeIsOver();
	}

	/*if (!(_isOnMotionDetect && _isAlarmOn))
	{
		turnOnBlueToothIfMotionIsDetected();
	}*/

	if (!(_isOnMotionDetect && _isAlarmOn)) {
		internalTemperatureActivity();
	}

	if (!(_isOnMotionDetect && _isAlarmOn)) {
		voltageActivity();
	}

	//if (_isPositionEnable)
	//{
	//	if (_delayForGetCoordinates->IsDelayTimeFinished(true))
	//	{
	//		//getCoordinates();
	//	}
	//}

	if (!(_isOnMotionDetect && _isAlarmOn)) {
		pirSensorActivity();
	}

	motionDetectActivity();

	if (!(_isOnMotionDetect && _isAlarmOn)) {
		BluetoothDynamicMenu::process();
	}
}
void motionDetectActivity() {
	if (_isDisableCall || _findOutPhonesMode == 2 || _isPIRSensorActivated) {
		_isOnMotionDetect = false;
		return;
	}

	//if ((millis() - _millsStart) > _sensitivityAlarm)
	//{
	//	_millsStart = 0;
	//	_isFirstTilt = true;
	//}

	//if ((_isOnMotionDetect && _isAlarmOn) || (_isAlarmOn && _isExternalInterruptOn && (_isExtenalInterruptNormalyClosed ^ digitalRead(3))))
	//if ((_isOnMotionDetect && _isAlarmOn) || (_isAlarmOn && (_isExtenalInterruptNormalyClosed ^ digitalRead(3))) || _isExternalInterruptOn))	/*if(true)*/

	if (_isAlarmOn && ((_isOnMotionDetect && !_isExternalInterruptOn)
		|| _isOnExternalMotionDetect
		|| ((_isExtenalInterruptNormalyClosed ^ digitalRead(3))
			&& _isExternalInterruptOn))) {

		blinkLedHideMode();

		detachInterrupt(0);
		detachInterrupt(1);

		_what_is_happened[0] = 'M';
		DEBUG_SERIAL_PRINTLN(F("Motion detected"));

		if (_findOutPhonesMode == 1) {
			if (!_isDeviceDetected) {
				callSim900();
				_isMasterMode = false;
			}
		}
		else {
			callSim900();
			_isMasterMode = false;
		}

		////Accendo bluetooth con ritardo annesso solo se è scattato allarme,troppo critico
		////per perdere tempo se non scattato allarme.
		//if (!bluetooth_repository.isBluetoothOn() && _findOutPhonesMode == 0)
		//{
		//	delay(30000);
		//	turnOnBlueToothAndSetTurnOffTimer(false);
		//}
		////}

		//readIncomingSMS();

		/*	readIncomingSMS();

			findOutPhonesONAndSetBluetoothInMasterModeActivity();*/

	EIFR |= 1 << INTF1; //clear external interrupt 1
	EIFR |= 1 << INTF0; //clear external interrupt 0
		//EIFR = 0x01;
		sei();

		attachInterrupt(0, motionTiltInternalInterrupt, RISING);
		attachInterrupt(1, motionTiltExternalInterrupt, CHANGE);

		_isOnMotionDetect = false;
		_isOnExternalMotionDetect = false;
	}
}
//void restartBlueTooth()
//{
//	Serial.readString();
//}
void turnOnBlueToothAndSetTurnOffTimer() {
	Serial.flush();
	bluetooth_repository.set_to_slave_mode();
	//if (_findOutPhonesMode == 0 || isFromSMS)
	//{
	bluetooth_repository.set_to_slave_mode();
	bluetooth_repository.turnOnBlueTooth();
	_timeToTurnOnAlarm = millis() + 300000;
	_isAlarmOn = false;
	//	}
	_isMasterMode = false;
}
void blinkLedHideMode() {
	if (_isBlueLedDisable) { return; }
	for (uint8_t i = 0; i < 3; i++) {
		digitalWrite(_pin_powerLed, HIGH);
		delay(50);
		digitalWrite(_pin_powerLed, LOW);
		delay(50);
	}
}
void blinkLed(uint8_t blinkDelay, uint8_t numberOfBlinks) {
	for (uint8_t i = 0; i < numberOfBlinks; i++) {
		digitalWrite(_pin_powerLed, HIGH);
		delay(blinkDelay);
		digitalWrite(_pin_powerLed, LOW);
		delay(blinkDelay);
	}
}
void buzzerSensorActivity() {
	for (uint8_t i = 0; i < 15; i++) {
		tone(_pin_buzzer, 400, 500);
		delay(1000);
		noTone(_pin_buzzer);
	}
}
void pirSensorActivity() {
	if (_isDisableCall) { return; }
	if (_isPIRSensorActivated && _isAlarmOn) {
		if (digitalRead(_pin_pir)) {
			blinkLedHideMode();
			_what_is_happened[0] = 'P';
			DEBUG_SERIAL_PRINTLN(F("pir sensor"));
			if (_findOutPhonesMode == 1) {
				if (!_isDeviceDetected) {
					if (_isBuzzerOn) {
						buzzerSensorActivity();
					}
					callSim900();
					_isMasterMode = false;
					//reedRelaySensorActivity(_pin_reedRelay);
				}
			}
			else {
				if (_isBuzzerOn) {
					buzzerSensorActivity();
				}
				callSim900();
				_isMasterMode = false;
			}
		}
	}
}
void reedRelaySensorActivity(uint8_t pin) {
	pinMode(pin, OUTPUT);
	blinkLedHideMode();
}
void internalTemperatureActivity() {
	if (_delay_for_temperature.IsDelayTimeFinished(true)) {
		if ((uint8_t)getTemp() > _tempMax) {
			_what_is_happened[0] = 'T';
			DEBUG_SERIAL_PRINTLN(F("Temperature high."));
			callSim900();
		}
	}
}
void voltageActivity() {
	if (_delay_for_voltage.IsDelayTimeFinished(true)) {
		_voltageValue = (5.10 / 1023.00) * analogRead(A1);
		_voltageMinValue = 3.25;
		if (_voltageValue < _voltageMinValue) {
			_what_is_happened[0] = 'V';
			DEBUG_SERIAL_PRINTLN(F("Voltage low."));
			callSim900();
		}
	}
}
void readIncomingSMS() {
	//Inserita per scaricare buffer e agevolare arrivo sms.
	my_sim900.ReadIncomingChars2();

	my_sim900.ATCommand("AT+CMGL");//=\"REC UNREAD\"");
	//my_sim900.ATCommand("AT+CMGR=1");
	delay(100);
	/**/

	if (my_sim900.IsAvailable() > 0) {
		String response = my_sim900.ReadIncomingChars2();
		delay(500);
		response.trim();
		DEBUG_SERIAL_PRINT(F("####")); DEBUG_SERIAL_PRINT(response); DEBUG_SERIAL_PRINTLN(F("####"));
		//if (response.substring(0, 5) == F("+CMT:"))
		//if (response.indexOf("+CMT:") != -1)
		if (response.indexOf("+CMGL:") != -1) {
			blinkLedHideMode();
			int position = 0;

			position = response.indexOf('"', position);
			position = response.indexOf('"', position + 1);
			position = response.indexOf('"', position + 1);

			/*Serial.println(position);

			Serial.println(response.substring(position + 4, position + 14));

			Serial.println(response.substring(position + 19, position + 29));*/

			if (response.substring(position + 4, position + 14) != _phoneNumber &&
				response.substring(position + 19, position + 29) != _phoneNumber &&
				response.substring(position + 4, position + 14) != _phoneNumberAlternative &&
				response.substring(position + 19, position + 29) != _phoneNumberAlternative) {
				DEBUG_SERIAL_PRINTLN(F("Numero errato"));
				return;
			}
			int index = response.lastIndexOf('"');
			String smsCommand = response.substring(index + 1, index + 7);
			smsCommand.trim();
			//Serial.println(smsCommand);
			delay(1000);
			listOfSmsCommands(smsCommand);
		}
	}
}
void listOfSmsCommands(String command) {

	//Enable incoming call.
	if (command == F("P1")) {
		_phoneNumbers = 1;
		callSim900();
	}

	if (command == F("P2")) {
		_phoneNumbers = 2;
		callSim900();
	}

	//Enable incoming call.
	if (command == F("Rc")) {
		my_sim900.enableIncomingCall(1);
	}

	if (command == F("Rs")) {
		my_sim900.disableIncomingCall();
		callSim900();
	}

	//Disattiva chiamate
	if (command == F("Dc")) {
		_isDisableCall = true;
	}

	//Accende bluetooth
	if (command == F("Ab")) {
		turnOnBlueToothAndSetTurnOffTimer();
		blinkLed(500, 3);
	}

	//Accende led
	if (command == F("Al")) {
		_isBlueLedDisable = false;
		blinkLed(500, 3);
	}

	//Check system
	if (command == F("Ck")) {
		callSim900();
	}

	////Position enable.
	//if (command == F("Pe"))
	//{
	//	_isPositionEnable = true;
	//}

	////Position disable.
	//if (command == F("Pd"))
	//{
	//	_isPositionEnable = false;
	//}

	//Attiva funzione non vedermi
	if (command == F("Nv")) {
		_findOutPhonesMode = 1;
		_isBTSleepON = false;
		_timeToTurnOnAlarm = 0;
		findOutPhonesONAndSetBluetoothInMasterModeActivity();
		blinkLed(500, 3);
	}

	//Attiva External interrupt normalmente aperto
	if (command == F("Eo")) {
		_isBTSleepON = true;
		_isPIRSensorActivated = 0;
		_findOutPhonesMode = 0;
		_isBuzzerOn = 0;
		_isExternalInterruptOn = 1;
		activateFunctionAlarm();
		bluetooth_repository.turnOffBlueTooth();
		_isExtenalInterruptNormalyClosed = false;
	}

	//Disattiva External interrupt
	if (command == F("Ex")) {
		_isExternalInterruptOn = 0;
	}

	//Attiva External interrupt normalmente chiuso
	if (command == F("Ec")) {
		_isBTSleepON = true;
		_isPIRSensorActivated = 0;
		_findOutPhonesMode = 0;
		_isBuzzerOn = 0;
		_isExternalInterruptOn = 1;
		activateFunctionAlarm();
		bluetooth_repository.turnOffBlueTooth();
		_isExtenalInterruptNormalyClosed = true;
	}

	//Attiva motion detect senza bluetooth
	if (command == F("Md")) {
		_isBTSleepON = true;
		_isPIRSensorActivated = 0;
		_findOutPhonesMode = 0;
		_isBuzzerOn = 0;
		activateFunctionAlarm();
		bluetooth_repository.turnOffBlueTooth();
	}

	//Attiva Buzzer
	if (command == F("Bz")) {
		_isBuzzerOn = 1;
		blinkLed(500, 3);
	}

	//Attiva pir sensor senza bluetooth
	if (command == F("Wc")) {
		_isBTSleepON = true;
		_isPIRSensorActivated = 1;
		_findOutPhonesMode = 0;
		/*	_isBuzzerOn = 0;*/
		activateFunctionAlarm();
		bluetooth_repository.turnOffBlueTooth();
	}

	//Find me
	if (command == F("Fm")) {
		_isBTSleepON = false;
		_findOutPhonesMode = 2;
		_isPIRSensorActivated = 0;
		_isBuzzerOn = 0;
		activateFunctionAlarm();
	}
}
void activateFunctionAlarm() {
	_timeToTurnOnAlarm = 0;
	_isDisableCall = false;
	_isAlarmOn = true;
	callSim900();
}
double getTemp(void) {
	unsigned int wADC;
	double t;

	// The internal temperature has to be used
	// with the internal reference of 1.1V.
	// Channel 8 can not be selected with
	// the analogRead function yet.

	// Set the internal reference and mux.
	ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
	ADCSRA |= _BV(ADEN);  // enable the ADC

	delay(20);            // wait for voltages to become stable.

	ADCSRA |= _BV(ADSC);  // Start the ADC

	// Detect end-of-conversion
	while (bit_is_set(ADCSRA, ADSC));

	// Reading register "ADCW" takes care of how to read ADCL and ADCH.
	wADC = ADCW;

	// The offset of 324.31 could be wrong. It is just an indication.
	t = (wADC - _offSetTempValue) / 1.22;

	// The returned temperature is in degrees Celsius.
	return (t);
}
//unsigned int offSetTempValue(double externalTemperature)
//{
//	unsigned int wADC;
//	double t;
//	// The internal temperature has to be used
//	// with the internal reference of 1.1V.
//	// Channel 8 can not be selected with
//	// the analogRead function yet.
//
//	// Set the internal reference and mux.
//	ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
//	ADCSRA |= _BV(ADEN);  // enable the ADC
//
//	delay(20);            // wait for voltages to become stable.
//
//	ADCSRA |= _BV(ADSC);  // Start the ADC
//
//						  // Detect end-of-conversion
//	while (bit_is_set(ADCSRA, ADSC));
//
//	// Reading register "ADCW" takes care of how to read ADCL and ADCH.
//	wADC = ADCW;
//
//	// The offset of 324.31 could be wrong. It is just an indication.
//	//t = (wADC - _offSetTempValue) / 1.22;
//
//	// The returned temperature is in degrees Celsius.
//	return (-(externalTemperature * 1.22) + wADC);
//}
