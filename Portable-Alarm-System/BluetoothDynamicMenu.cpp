#include "BluetoothDynamicMenu.h"
#include <stdlib.h>
#include <string.h>
#if defined(__AVR__)
#include <LSGEEpromRW.h>
#endif
#include <mf_repository_BlueToothRepository.h>
#include "BluetoothCommandUtil.h"
#include "BluetoothFrameWriter.h"

// Objects already instantiated by Portable-Alarm-System.ino
extern BlueToothRepository bluetooth_repository;
extern BluetoothFrameWriter bluetooth_frame_writer;
// Project state already owned by Portable-Alarm-System.ino
extern char version[15];
extern char _old_password[5];
extern char _new_password[5];
extern bool _isAlarmOn;
extern bool _isOnMotionDetect;
extern bool _isDisableCall;
extern uint8_t _isBTSleepON;
extern uint8_t _isPIRSensorActivated;
extern uint8_t _is_external_Interrupt_activated;
extern uint8_t _isBuzzerOn;
extern uint8_t _phoneNumbers;
extern uint8_t _findOutPhonesMode;
extern uint8_t _tempMax;
extern uint8_t _delayFindMe;
extern unsigned int _offSetTempValue;
extern unsigned long _timeToTurnOnAlarm;
extern float _voltageValue;
extern char _phoneNumber[11];
extern char _phoneNumberAlternative[11];
extern char _what_is_happened[2];
extern char _bufPirSensorIsON[2];
extern char _bufFindOutPhonesON[2];
extern char _bufDbPhoneON[2];
extern char _bufTemperatureMax[3];
extern char _bufDeviceAddress[15];
extern char _bufDeviceAddress2[15];
extern char _bufDeviceName[15];
extern char _bufDeviceName2[15];
extern char _bufOffSetTemperature[5];
extern char _bufDelayFindMe[2];
extern char _bufExternalInterruptIsON[2];
extern char _bufBuzzerIsON[2];
extern double getTemp(void);

namespace {
    enum EepromAddress : uint8_t {
        AddressPhoneNumber            = 1U,
        AddressTemperatureMax         = 16U,
        AddressPirSensorIsOn          = 19U,
        AddressDeviceAddress          = 21U,
        AddressDeviceName             = 36U,
        AddressFindOutPhonesOn        = 51U,
        AddressDbPhoneIsOn            = 55U,
        AddressPhoneNumberAlternative = 57U,
        AddressOffsetTemperature      = 95U,
        AddressDelayFindMe            = 100U,
        AddressExternalInterruptIsOn  = 102U,
        AddressDeviceAddress2         = 104U,
        AddressDeviceName2            = 119U,
        AddressBuzzerIsOn             = 134U
    };
}

void BluetoothDynamicMenu::write_eeprom(uint8_t address, const char* value) {
#if defined(__AVR__)
    LSG_EEpromRW eeprom_rw;
    eeprom_rw.eeprom_write_string(address, value);
#else
    (void)address;
    (void)value;
#endif
}
bool BluetoothDynamicMenu::contains(const char* bluetooth_data, PGM_P command) {
    return bluetooth_find_program_text(bluetooth_data, command) != nullptr;
}
bool BluetoothDynamicMenu::read_value(const char* bluetooth_data, char* value, uint8_t capacity) {
    const char* value_start = strchr(bluetooth_data, ';');
    if (value_start == nullptr || capacity == 0U) {
        return false;
    }
    value_start++;
    const char* value_end = strchr(value_start, ';');
    if (value_end == nullptr) {
        value_end = value_start + strlen(value_start);
    }
    uint8_t value_length = static_cast<uint8_t>(value_end - value_start);
    if (value_length >= capacity) {
        return false;
    }
    memcpy(value, value_start, value_length);
    value[value_length] = '\0';
    return true;
}
bool BluetoothDynamicMenu::is_numeric(const char* value) {
    while (*value != '\0') {
        if (*value >= '0' && *value <= '9') {
            return true;
        }
        value++;
    }
    return false;
}
bool BluetoothDynamicMenu::copy_value(char* destination, uint8_t capacity, const char* value) {
    uint8_t value_length = static_cast<uint8_t>(strlen(value));
    if (value_length == 0U || value_length >= capacity) {
        return false;
    }
    memcpy(destination, value, value_length + 1U);
    return true;
}
PGM_P BluetoothDynamicMenu::calculate_battery_level(float battery_level) {
    if (battery_level <= 3.25f) return BT_TEXT("[    ]+");
    if (battery_level <= 3.30f) return BT_TEXT("[|   ]+");
    if (battery_level <= 3.40f) return BT_TEXT("[||  ]+");
    if (battery_level <= 3.60f) return BT_TEXT("[||| ]+");
    return BT_TEXT("[||||]+");
}
void BluetoothDynamicMenu::loadMainMenu() {
    bluetooth_frame_writer.send_value_frame(_isAlarmOn ? BT_TEXT("Alarm ON") : BT_TEXT("Alarm OFF"), version, BluetoothCommandUtil::Title);
    bluetooth_frame_writer.send_program_frame(BT_TEXT("Configuration"), BluetoothCommandUtil::Menu, 1U);
    bluetooth_frame_writer.send_program_frame(BT_TEXT("Security"), BluetoothCommandUtil::Menu, 4U);
    bluetooth_frame_writer.send_program_frame(_isAlarmOn ? BT_TEXT("Alarm OFF") : BT_TEXT("Alarm On"), BluetoothCommandUtil::Command, _isAlarmOn ? 3U : 2U);
    bluetooth_frame_writer.send_uint_frame(BT_TEXT("Temp.:"), getTemp(), BluetoothCommandUtil::Info);
    bluetooth_frame_writer.send_program_value_frame(BT_TEXT("Batt.level:"), calculate_battery_level(_voltageValue), BluetoothCommandUtil::Info);
    bluetooth_frame_writer.send_value_frame(BT_TEXT("WhatzUp:"), _what_is_happened, BluetoothCommandUtil::Info);
    bluetooth_frame_writer.send_end();
    bluetooth_repository.flush();
}
void BluetoothDynamicMenu::loadConfigurationMenu() {
    bluetooth_frame_writer.send_program_frame(BT_TEXT("Configuration"), BluetoothCommandUtil::Title);
    bluetooth_frame_writer.send_value_frame(BT_TEXT("Phone:"), _phoneNumber, BluetoothCommandUtil::Data, 1U);
    bluetooth_frame_writer.send_value_frame(BT_TEXT("Ph.Altern.:"), _phoneNumberAlternative, BluetoothCommandUtil::Data, 99U);
    bluetooth_frame_writer.send_uint_frame(BT_TEXT("N.Phone:"), _phoneNumbers, BluetoothCommandUtil::Data, 98U);
    bluetooth_frame_writer.send_uint_frame(BT_TEXT("TempMax:"), _tempMax, BluetoothCommandUtil::Data, 4U);
    bluetooth_frame_writer.send_uint_frame(BT_TEXT("OffSetTemp:"), _offSetTempValue, BluetoothCommandUtil::Data, 95U);
    if (_findOutPhonesMode != 2U) {
        bluetooth_frame_writer.send_uint_frame(BT_TEXT("PIR status:"), _isPIRSensorActivated, BluetoothCommandUtil::Data, 5U);
    }
    if (_findOutPhonesMode != 0U) {
        bluetooth_frame_writer.send_value_frame(BT_TEXT("Addr:"), _bufDeviceAddress, BluetoothCommandUtil::Data, 10U);
        bluetooth_frame_writer.send_value_frame(BT_TEXT("Name:"), _bufDeviceName, BluetoothCommandUtil::Data, 11U);
        bluetooth_frame_writer.send_value_frame(BT_TEXT("Addr2:"), _bufDeviceAddress2, BluetoothCommandUtil::Data, 15U);
        bluetooth_frame_writer.send_value_frame(BT_TEXT("Name2:"), _bufDeviceName2, BluetoothCommandUtil::Data, 16U);
        bluetooth_frame_writer.send_uint_frame(BT_TEXT("FindLoop:"), _delayFindMe, BluetoothCommandUtil::Data, 94U);
    }
    bluetooth_frame_writer.send_uint_frame(BT_TEXT("FindMode:"), _findOutPhonesMode, BluetoothCommandUtil::Data, 12U);
    bluetooth_frame_writer.send_uint_frame(BT_TEXT("Ext.Int:"), _is_external_Interrupt_activated, BluetoothCommandUtil::Data, 13U);
    bluetooth_frame_writer.send_uint_frame(BT_TEXT("Buzz.:"), _isBuzzerOn, BluetoothCommandUtil::Data, 14U);
    bluetooth_frame_writer.send_end();
}
void BluetoothDynamicMenu::loadSecurityMenu() {
    bluetooth_frame_writer.send_program_frame(BT_TEXT("Security"), BluetoothCommandUtil::Title);
    bluetooth_frame_writer.send_program_frame(BT_TEXT("Change passw.:"), BluetoothCommandUtil::Menu, 5U);
    bluetooth_frame_writer.send_program_frame(BT_TEXT("Change name:"), BluetoothCommandUtil::Menu, 6U);
    bluetooth_frame_writer.send_end();
}
void BluetoothDynamicMenu::process() {
    if (!bluetooth_repository.available()) {
        return;
    }
    char bluetooth_data[bluetooth_receive_buffer_size] = {};
    char value[15] = {};
    if (bluetooth_repository.readString(bluetooth_data, sizeof(bluetooth_data)) == 0U) {
        return;
    }
    if (contains(bluetooth_data, BT_TEXT("#0"))) {
        _timeToTurnOnAlarm = bluetooth_millis() + 300000UL;
        loadMainMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("C002"))) {
        _isAlarmOn = true;
        _isOnMotionDetect = false;
        _timeToTurnOnAlarm = 0;
        _isDisableCall = false;
        loadMainMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("C003"))) {
        _isAlarmOn = false;
        loadMainMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("M001"))) {
        _timeToTurnOnAlarm = bluetooth_millis() + 300000UL;
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D001"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && is_numeric(value) && copy_value(_phoneNumber, 11U, value)) {
            write_eeprom(AddressPhoneNumber, _phoneNumber);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D094"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && is_numeric(value) && copy_value(_bufDelayFindMe, 2U, value)) {
            write_eeprom(AddressDelayFindMe, _bufDelayFindMe);
            _delayFindMe = atoi(_bufDelayFindMe);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D095"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && is_numeric(value) && copy_value(_bufOffSetTemperature, 5U, value)) {
            write_eeprom(AddressOffsetTemperature, _bufOffSetTemperature);
            _offSetTempValue = atoi(_bufOffSetTemperature);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D098"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && is_numeric(value) && copy_value(_bufDbPhoneON, 2U, value)) {
            write_eeprom(AddressDbPhoneIsOn, _bufDbPhoneON);
            _phoneNumbers = atoi(_bufDbPhoneON);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D099"))) {
        if (read_value(bluetooth_data, value, sizeof(value))) {
            if (strcmp(value, "#") == 0) {
                _phoneNumberAlternative[0] = '\0';
            }
            else if (is_numeric(value)) {
                copy_value(_phoneNumberAlternative, 11U, value);
            }
            write_eeprom(AddressPhoneNumberAlternative, _phoneNumberAlternative);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D004"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && is_numeric(value) && copy_value(_bufTemperatureMax, 3U, value)) {
            write_eeprom(AddressTemperatureMax, _bufTemperatureMax);
            _tempMax = atoi(_bufTemperatureMax);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D005"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && is_numeric(value) && copy_value(_bufPirSensorIsON, 2U, value)) {
            write_eeprom(AddressPirSensorIsOn, _bufPirSensorIsON);
            _isPIRSensorActivated = atoi(_bufPirSensorIsON);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D010"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && copy_value(_bufDeviceAddress, 15U, value)) {
            write_eeprom(AddressDeviceAddress, _bufDeviceAddress);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D011"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && copy_value(_bufDeviceName, 15U, value)) {
            write_eeprom(AddressDeviceName, _bufDeviceName);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D015"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && copy_value(_bufDeviceAddress2, 15U, value)) {
            write_eeprom(AddressDeviceAddress2, _bufDeviceAddress2);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D016"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && copy_value(_bufDeviceName2, 15U, value)) {
            write_eeprom(AddressDeviceName2, _bufDeviceName2);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D012"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && is_numeric(value) && copy_value(_bufFindOutPhonesON, 2U, value)) {
            write_eeprom(AddressFindOutPhonesOn, _bufFindOutPhonesON);
            _findOutPhonesMode = atoi(_bufFindOutPhonesON);
            _isBTSleepON = _findOutPhonesMode == 0U ? 1U : 0U;
            if (_findOutPhonesMode == 2U) {
                _isPIRSensorActivated = 0;
            }
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D013"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && is_numeric(value) && copy_value(_bufExternalInterruptIsON, 2U, value)) {
            write_eeprom(AddressExternalInterruptIsOn, _bufExternalInterruptIsON);
            _is_external_Interrupt_activated = atoi(_bufExternalInterruptIsON);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("D014"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && is_numeric(value) && copy_value(_bufBuzzerIsON, 2U, value)) {
            write_eeprom(AddressBuzzerIsOn, _bufBuzzerIsON);
            _isBuzzerOn = atoi(_bufBuzzerIsON);
        }
        loadConfigurationMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("M004"))) {
        loadSecurityMenu();
    }
    else if (contains(bluetooth_data, BT_TEXT("M005"))) {
        bluetooth_frame_writer.send_program_frame(BT_TEXT("Change passw."), BluetoothCommandUtil::Title);
        bluetooth_frame_writer.send_program_frame(BT_TEXT("Insert old passw.:"), BluetoothCommandUtil::Data, 6U);
        bluetooth_frame_writer.send_end();
    }
    else if (contains(bluetooth_data, BT_TEXT("M006"))) {
        bluetooth_frame_writer.send_program_frame(BT_TEXT("Change passw."), BluetoothCommandUtil::Title);
        bluetooth_frame_writer.send_program_frame(BT_TEXT("Insert name:"), BluetoothCommandUtil::Data, 7U);
        bluetooth_frame_writer.send_end();
    }
    else if (contains(bluetooth_data, BT_TEXT("D006"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && strcmp(_old_password, value) == 0) {
            bluetooth_frame_writer.send_program_frame(BT_TEXT("Change passw."), BluetoothCommandUtil::Title);
            bluetooth_frame_writer.send_program_frame(BT_TEXT("Insert new passw:"), BluetoothCommandUtil::Data, 8U);
            bluetooth_frame_writer.send_end();
        }
        else {
            bluetooth_frame_writer.send_program_frame(BT_TEXT("Change passw."), BluetoothCommandUtil::Title);
            bluetooth_frame_writer.send_program_frame(BT_TEXT("Wrong passw:"), BluetoothCommandUtil::Message);
            bluetooth_frame_writer.send_end();
        }
    }
    else if (contains(bluetooth_data, BT_TEXT("D008"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && copy_value(_new_password, 5U, value)) {
            bluetooth_frame_writer.send_program_frame(BT_TEXT("Change passw."), BluetoothCommandUtil::Title);
            bluetooth_frame_writer.send_program_frame(BT_TEXT("Confirm pass:"), BluetoothCommandUtil::Data, 9U);
            bluetooth_frame_writer.send_end();
        }
    }
    else if (contains(bluetooth_data, BT_TEXT("D009"))) {
        if (read_value(bluetooth_data, value, sizeof(value)) && strcmp(_new_password, value) == 0) {
            bluetooth_frame_writer.send_program_frame(BT_TEXT("Change passw."), BluetoothCommandUtil::Title);
            bluetooth_frame_writer.send_program_frame(BT_TEXT("changed:"), BluetoothCommandUtil::Message);
            bluetooth_frame_writer.send_end();
            bluetooth_delay(2000);
            bluetooth_repository.set_password(_new_password);
            bluetooth_repository.set_to_slave_mode();
            memcpy(_old_password, _new_password, sizeof(_old_password));
        }
        else {
            bluetooth_frame_writer.send_program_frame(BT_TEXT("Change passw."), BluetoothCommandUtil::Title);
            bluetooth_frame_writer.send_program_frame(BT_TEXT("passw. doesn't match"), BluetoothCommandUtil::Message);
            bluetooth_frame_writer.send_end();
            bluetooth_repository.println("D006");
        }
    }
    else if (contains(bluetooth_data, BT_TEXT("D007"))) {
        if (read_value(bluetooth_data, value, sizeof(value))) {
            bluetooth_frame_writer.send_program_frame(BT_TEXT("Change passw."), BluetoothCommandUtil::Title);
            bluetooth_frame_writer.send_program_frame(BT_TEXT("changed:"), BluetoothCommandUtil::Message);
            bluetooth_frame_writer.send_end();
            bluetooth_delay(2000);
            bluetooth_repository.set_name(value);
            bluetooth_repository.set_to_slave_mode();
        }
    }
    bluetooth_delay(100);
}
