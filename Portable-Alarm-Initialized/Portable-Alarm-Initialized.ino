// Portable-Alarm-Initialized.ino

#include <EEPROM.h>
#include <mf_commons_commonsLayer.h>
#include <mf_adapter_HardwareSerialAdapter.h>
#include <mf_repository_AvrMicroRepository.h>
#include <mf_repository_BlueToothRepository.h>

#ifndef EEPROM_INITIALIZATION_ENABLED
#define EEPROM_INITIALIZATION_ENABLED 0U
#endif

const uint8_t configuration_version_address = 0U;
const uint8_t configuration_version = 1U;
const uint8_t address_phone_number = 1U;
const uint8_t address_precision_number = 12U;
const uint8_t address_temperature_enabled = 14U;
const uint8_t address_temperature_max = 16U;
const uint8_t address_pir_enabled = 19U;
const uint8_t address_device_1 = 21U;
const uint8_t address_device_name_1 = 36U;
const uint8_t address_find_phones_enabled = 51U;
const uint8_t address_bluetooth_sleep_enabled = 53U;
const uint8_t address_selected_phone = 55U;
const uint8_t address_alternative_phone_number = 57U;
const uint8_t address_find_mode = 68U;
const uint8_t address_apn = 70U;
const uint8_t address_temperature_offset = 95U;
const uint8_t address_find_delay = 100U;
const uint8_t address_external_interrupt_enabled = 102U;
const uint8_t address_device_2 = 104U;
const uint8_t address_device_name_2 = 119U;
const uint8_t address_buzzer_enabled = 134U;

const uint8_t bluetooth_key_pin = 10U;
const uint8_t bluetooth_power_pin = 6U;

const unsigned long bluetooth_program_baud_rate = 38400UL;
const unsigned long bluetooth_receive_baud_rate = 9600UL;
const unsigned long serial_monitor_baud_rate = 38400UL;

const size_t bluetooth_response_buffer_size = 64U;
const unsigned long bluetooth_response_end_delay_ms = 100UL;

#if EEPROM_INITIALIZATION_ENABLED

void clear_eeprom() {
	const int eeprom_length = EEPROM.length();

	for (int address = 0; address < eeprom_length; address++) {
		EEPROM.update(address, 0U);
	}
}

void write_eeprom_text(uint8_t address, const char* value) {
	while (*value != '\0') {
		EEPROM.update(address, static_cast<uint8_t>(*value));
		address++;
		value++;
	}

	EEPROM.update(address, 0U);
}

void initialize_configuration() {
	write_eeprom_text(address_phone_number, "3202445649");
	write_eeprom_text(address_precision_number, "0");
	write_eeprom_text(address_temperature_enabled, "0");
	write_eeprom_text(address_temperature_max, "60");
	write_eeprom_text(address_pir_enabled, "0");
	write_eeprom_text(address_device_1, "6045,CB,3122BD");
	write_eeprom_text(address_device_name_1, "ASUS_Z017D");
	write_eeprom_text(address_find_phones_enabled, "0");
	write_eeprom_text(address_bluetooth_sleep_enabled, "1");
	write_eeprom_text(address_selected_phone, "1");
	write_eeprom_text(address_alternative_phone_number, "");
	write_eeprom_text(address_find_mode, "0");
	write_eeprom_text(address_apn, "");
	write_eeprom_text(address_temperature_offset, "324");
	write_eeprom_text(address_find_delay, "1");
	write_eeprom_text(address_external_interrupt_enabled, "0");
	write_eeprom_text(address_device_2, "");
	write_eeprom_text(address_device_name_2, "");
	write_eeprom_text(address_buzzer_enabled, "0");

	EEPROM.update(configuration_version_address, configuration_version);
}

#endif

bool is_bluetooth_response_space(char value) {
	return value == ' ' || value == '\r' || value == '\n' || value == '\t';
}

void trim_bluetooth_response(char* response) {
	char* first = response;

	while (*first != '\0' && is_bluetooth_response_space(*first)) {
		first++;
	}

	char* last = first;

	while (*last != '\0') {
		last++;
	}

	while (last > first && is_bluetooth_response_space(*(last - 1))) {
		last--;
	}

	char* destination = response;

	while (first < last) {
		*destination = *first;
		destination++;
		first++;
	}

	*destination = '\0';
}

bool read_bluetooth_characteristic(BlueToothRepository& bluetooth, AvrMicroRepository& avr_repository, const char* command, char* response, size_t response_size) {
	if (response == nullptr || response_size == 0U) {
		return false;
	}

	response[0] = '\0';

	bluetooth.set_to_program_mode();

	delay(bluetooth_response_end_delay_ms);

	avr_repository.clearBuffer();

	bluetooth.println(command);

	const size_t received = bluetooth.readString(response, response_size);

	trim_bluetooth_response(response);

	return received > 0U && response[0] != '\0';
}

void log_bluetooth_characteristic(BlueToothRepository& bluetooth, AvrMicroRepository& avr_repository, const __FlashStringHelper* label, const char* command) {
	char response[bluetooth_response_buffer_size];

	Serial.print(F("INTERROGAZIONE BLUETOOTH - "));
	Serial.println(label);
	Serial.flush();

	const bool has_response = read_bluetooth_characteristic(bluetooth, avr_repository, command, response, sizeof(response));

	Serial.print(label);
	Serial.print(F(": "));

	if (!has_response) {
		Serial.println(F("nessuna risposta"));
		return;
	}

	Serial.println(response);
}

void log_bluetooth_connection(BlueToothRepository& bluetooth, AvrMicroRepository& avr_repository) {
	log_bluetooth_characteristic(bluetooth, avr_repository, F("Connessione"), "AT");
}

void log_bluetooth_version(BlueToothRepository& bluetooth, AvrMicroRepository& avr_repository) {
	log_bluetooth_characteristic(bluetooth, avr_repository, F("Versione"), "AT+VERSION?");
}

void log_bluetooth_name(BlueToothRepository& bluetooth, AvrMicroRepository& avr_repository) {
	log_bluetooth_characteristic(bluetooth, avr_repository, F("Nome"), "AT+NAME?");
}

void log_bluetooth_address(BlueToothRepository& bluetooth, AvrMicroRepository& avr_repository) {
	log_bluetooth_characteristic(bluetooth, avr_repository, F("Indirizzo"), "AT+ADDR?");
}

void log_bluetooth_password(BlueToothRepository& bluetooth, AvrMicroRepository& avr_repository) {
	log_bluetooth_characteristic(bluetooth, avr_repository, F("Password"), "AT+PSWD?");
}

void log_bluetooth_role(BlueToothRepository& bluetooth, AvrMicroRepository& avr_repository) {
	log_bluetooth_characteristic(bluetooth, avr_repository, F("Ruolo"), "AT+ROLE?");
}

void log_bluetooth_uart(BlueToothRepository& bluetooth, AvrMicroRepository& avr_repository) {
	log_bluetooth_characteristic(bluetooth, avr_repository, F("UART"), "AT+UART?");
}

void log_bluetooth_characteristics(BlueToothRepository& bluetooth, AvrMicroRepository& avr_repository) {
	Serial.println(F("--- ATTENZIONE!!! USARE 5Volts pieni Diagnostica Bluetooth ---"));

	log_bluetooth_connection(bluetooth, avr_repository);
	log_bluetooth_version(bluetooth, avr_repository);
	log_bluetooth_name(bluetooth, avr_repository);
	log_bluetooth_address(bluetooth, avr_repository);
	log_bluetooth_password(bluetooth, avr_repository);
	log_bluetooth_role(bluetooth, avr_repository);
	log_bluetooth_uart(bluetooth, avr_repository);

	Serial.println(F("--- Fine diagnostica Bluetooth ---"));
}

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	Serial.begin(serial_monitor_baud_rate);

	static HardwareSerialAdapter bluetooth_serial_adapter(Serial);

	static AvrMicroRepository bluetooth_avr_repository(
		bluetooth_serial_adapter,
		mf::commons::commonsLayer::AnalogRefMode::DEFAULT_m,
		5.0f);

	static BlueToothRepository bluetooth_repository(
		bluetooth_avr_repository,
		bluetooth_key_pin,
		bluetooth_power_pin,
		bluetooth_program_baud_rate,
		bluetooth_receive_baud_rate);

#if EEPROM_INITIALIZATION_ENABLED
	Serial.println(F("ATTENZIONE: SCRITTURA EEPROM ABILITATA"));
	Serial.println(F("CANCELLAZIONE E INIZIALIZZAZIONE EEPROM IN CORSO"));

	clear_eeprom();
	initialize_configuration();

	Serial.println(F("SCRITTURA EEPROM COMPLETATA"));
#else
	Serial.println(F("SCRITTURA EEPROM DISABILITATA"));
#endif

	log_bluetooth_characteristics(bluetooth_repository, bluetooth_avr_repository);

	digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
}