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
	sim_repository.begin(19200);
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
	sim_repository.setNetlightEnabled(false);
	sim_repository.initSmsReception();
	sim_repository.deleteAllSms();
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
	sim_repository.call(phoneNumber);
}

void motionTiltExternalInterrupt() {
	if ((_isExternalInterruptOn & 0x01U) != 0U /*&& !_isPIRSensorActivated*/) {
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
		bluetooth_repository.find_mode_v3();
		bluetooth_repository.set_to_master_mode();
		_isMasterMode = true;
	}
	_isDeviceDetected = false;
	for (uint8_t i = 0; i < _delayFindMe; i++) {
		if (_phoneNumbers == 1) {
			_isDeviceDetected = bluetooth_repository.is_device_detected(_bufDeviceAddress, _bufDeviceName);
			if (_isDeviceDetected) {
				Serial.println("Find first BT");
				break;
				
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
	findOutPhonesONAndSetBluetoothInMasterModeActivity();
	return;
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
	if ((_isExternalInterruptOn & 0x01U) != 0U && !(_isExtenalInterruptNormalyClosed ^ digitalRead(3))) {
		if ((_isExternalInterruptOn & 0x02U) != 0U) {
			_isExternalInterruptOn &= static_cast<uint8_t>(~0x02U);
			sim_repository.hangUp();
		}
		_isOnExternalMotionDetect = false;
	}
	if ((_isExternalInterruptOn & 0x01U) != 0U && (_isExtenalInterruptNormalyClosed ^ digitalRead(3)) && sim_repository.isCallActive()) {
		delay(250UL);
		return;
	}

	//if ((millis() - _millsStart) > _sensitivityAlarm)
	//{
	//	_millsStart = 0;
	//	_isFirstTilt = true;
	//}

	//if ((_isOnMotionDetect && _isAlarmOn) || (_isAlarmOn && _isExternalInterruptOn && (_isExtenalInterruptNormalyClosed ^ digitalRead(3))))
	//if ((_isOnMotionDetect && _isAlarmOn) || (_isAlarmOn && (_isExtenalInterruptNormalyClosed ^ digitalRead(3))) || _isExternalInterruptOn))	/*if(true)*/

	if (_isAlarmOn && (
		(_isOnMotionDetect && ((_isExternalInterruptOn & 0x01U) == 0U))
		|| ((_isOnExternalMotionDetect || (_isExtenalInterruptNormalyClosed ^ digitalRead(3)))
			&& ((_isExternalInterruptOn & 0x01U) != 0U))
	)) {

		blinkLedHideMode();

		detachInterrupt(0);
		detachInterrupt(1);

		_what_is_happened[0] = 'M';
		DEBUG_SERIAL_PRINTLN(F("Motion detected"));

		if (_findOutPhonesMode == 1) {
			if (!_isDeviceDetected) {
				if ((_isExternalInterruptOn & 0x01U) != 0U) { _isExternalInterruptOn |= 0x02U; }
				callSim900();
				_isMasterMode = false;
			}
		}
		else {
			if ((_isExternalInterruptOn & 0x01U) != 0U) { _isExternalInterruptOn |= 0x02U; }
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
			int smsCount = sim_repository.getSmsCount();
			if (smsCount <= 0) { return; }
			char sender[16];
			char message[12];
			uint8_t maxIndex = static_cast<uint8_t>(smsCount + 10);
			for (uint8_t i = 1; i <= maxIndex; i++) {
				if (!sim_repository.readSms(i, sender, sizeof(sender), message, sizeof(message))) { continue; }
				DEBUG_SERIAL_PRINT(F("SMS ricevuto ["));
				DEBUG_SERIAL_PRINT(i);
				DEBUG_SERIAL_PRINT(F("] da "));
				DEBUG_SERIAL_PRINT(sender);
				DEBUG_SERIAL_PRINT(F(": "));
				DEBUG_SERIAL_PRINTLN(message);
				if (!sim_repository.deleteSmsAt(i)) {
					DEBUG_SERIAL_PRINTLN(F("Cancellazione SMS non riuscita"));
					return;
				}
				blinkLedHideMode();
				const char* number = sender;
				if (strncmp(number, "+39", 3) == 0) { number += 3; }
				if (strcmp(number, _phoneNumber) != 0 && strcmp(number, _phoneNumberAlternative) != 0) {
					DEBUG_SERIAL_PRINTLN(F("Numero errato"));
					continue;
				}
				listOfSmsCommands(message);
				break;
			}
		}

		void deactivateOtherAlarmModes() {
			_isPIRSensorActivated = 0;
			_findOutPhonesMode = 0;
			_isBuzzerOn = 0;
			_isExternalInterruptOn = 0;
			_isOnMotionDetect = false;
			_isOnExternalMotionDetect = false;
		}

		void listOfSmsCommands(const char* command) {
			if (command == nullptr || command[0] == '\0' || command[1] == '\0' || command[2] != '\0') { return; }
			// P1: seleziona il numero di telefono principale per le chiamate.
			if (command[0] == 'P' && command[1] == '1') {
				_phoneNumbers = 1;
				callSim900();
			}
			// P2: seleziona il numero di telefono alternativo per le chiamate.
			if (command[0] == 'P' && command[1] == '2') {
				_phoneNumbers = 2;
				callSim900();
			}
			// Rc: abilita la risposta automatica alle chiamate in ingresso.
			if (command[0] == 'R' && command[1] == 'c') {
				sim_repository.enableIncomingCall(1);
			}
			// Rs: disabilita la risposta automatica e richiama il numero selezionato.
			if (command[0] == 'R' && command[1] == 's') {
				sim_repository.disableIncomingCall();
				callSim900();
			}
			// Dc: disabilita le chiamate di allarme e chiude quella eventualmente in corso.
			if (command[0] == 'D' && command[1] == 'c') {
				_isDisableCall = true;
				_isExternalInterruptOn &= static_cast<uint8_t>(~0x02U);
				sim_repository.hangUp();
			}
			// Ab: accende il Bluetooth e avvia il relativo timer di spegnimento.
			if (command[0] == 'A' && command[1] == 'b') {
				turnOnBlueToothAndSetTurnOffTimer();
				blinkLed(500, 3);
			}
			// Al: abilita i lampeggi del LED di alimentazione.
			if (command[0] == 'A' && command[1] == 'l') {
				_isBlueLedDisable = false;
				blinkLed(500, 3);
			}
			// Ck: richiama il numero selezionato per verificare il sistema.
			if (command[0] == 'C' && command[1] == 'k') {
				callSim900();
			}
			// Nv: attiva la modalità Non vedermi con ricerca Bluetooth del telefono.
			if (command[0] == 'N' && command[1] == 'v') {
				deactivateOtherAlarmModes();
				_findOutPhonesMode = 1;
				_isBTSleepON = false;
				_timeToTurnOnAlarm = 0;
				blinkLed(500, 3);
			}
			// Eo: attiva l'allarme con contatto esterno normalmente aperto.
			if (command[0] == 'E' && command[1] == 'o') {
				deactivateOtherAlarmModes();
				_isBTSleepON = true;
				_isExternalInterruptOn = 1;
				activateFunctionAlarm();
				bluetooth_repository.turnOffBlueTooth();
				_isExtenalInterruptNormalyClosed = false;
			}
			// Ex: disabilita l'allarme del contatto esterno.
			if (command[0] == 'E' && command[1] == 'x') {
				_isExternalInterruptOn = 0;
			}
			// Ec: attiva l'allarme con contatto esterno normalmente chiuso.
			if (command[0] == 'E' && command[1] == 'c') {
				deactivateOtherAlarmModes();
				_isBTSleepON = true;
				_isExternalInterruptOn = 1;
				_timeToTurnOnAlarm = 0;
				_isDisableCall = false;
				_isAlarmOn = true;
				bluetooth_repository.turnOffBlueTooth();
				_isExtenalInterruptNormalyClosed = true;
			}
			// Md: attiva il rilevamento movimento o inclinazione senza Bluetooth.
			if (command[0] == 'M' && command[1] == 'd') {
				deactivateOtherAlarmModes();
				_isBTSleepON = true;
				activateFunctionAlarm();
				bluetooth_repository.turnOffBlueTooth();
			}
			// Bz: abilita il buzzer, se presente nell'hardware.
			if (command[0] == 'B' && command[1] == 'z') {
				_isBuzzerOn = 1;
				blinkLed(500, 3);
			}
			// Wc: attiva il sensore PIR senza Bluetooth.
			if (command[0] == 'W' && command[1] == 'c') {
				deactivateOtherAlarmModes();
				_isBTSleepON = true;
				_isPIRSensorActivated = 1;
				activateFunctionAlarm();
				bluetooth_repository.turnOffBlueTooth();
			}
			// Fm: attiva la modalità Trova il dispositivo.
			if (command[0] == 'F' && command[1] == 'm') {
				deactivateOtherAlarmModes();
				_isBTSleepON = false;
				_findOutPhonesMode = 2;
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
