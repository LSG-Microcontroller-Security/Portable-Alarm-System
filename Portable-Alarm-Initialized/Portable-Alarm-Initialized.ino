// Portable-Alarm-Initialized.ino
#include <EEPROM.h>
#include <MyBlueTooth.h>
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
const unsigned long bluetooth_response_timeout_ms = 1500UL;
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
void clear_bluetooth_serial_input() {
	while (Serial.available() > 0) {
		Serial.read();
	}
}
bool read_bluetooth_response(char* response, size_t response_size) {
	if (response == nullptr || response_size == 0U) {
		return false;
	}

	size_t index = 0U;
	bool received = false;
	const unsigned long start_time = millis();
	unsigned long last_byte_time = start_time;

	while ((millis() - start_time) < bluetooth_response_timeout_ms) {
		while (Serial.available() > 0) {
			const int value = Serial.read();
			if (value < 0) {
				continue;
			}

			if (index < (response_size - 1U)) {
				response[index] = static_cast<char>(value);
				index++;
			}

			received = true;
			last_byte_time = millis();
		}

		if (received && (millis() - last_byte_time) >= bluetooth_response_end_delay_ms) {
			break;
		}
	}

	response[index] = '\0';
	trim_bluetooth_response(response);
	return response[0] != '\0';
}
void log_bluetooth_connection(MyBlueTooth& bluetooth) {
	log_bluetooth_characteristic(bluetooth, F("Connessione"), F("AT"));
}
void log_bluetooth_version(MyBlueTooth& bluetooth) {
	log_bluetooth_characteristic(bluetooth, F("Versione"), F("AT+VERSION?"));
}
void log_bluetooth_name(MyBlueTooth& bluetooth) {
	log_bluetooth_characteristic(bluetooth, F("Nome"), F("AT+NAME?"));
}
void log_bluetooth_address(MyBlueTooth& bluetooth) {
	log_bluetooth_characteristic(bluetooth, F("Indirizzo"), F("AT+ADDR?"));
}
void log_bluetooth_password(MyBlueTooth& bluetooth) {
	log_bluetooth_characteristic(bluetooth, F("Password"), F("AT+PSWD?"));
}
void log_bluetooth_role(MyBlueTooth& bluetooth) {
	log_bluetooth_characteristic(bluetooth, F("Ruolo"), F("AT+ROLE?"));
}
void log_bluetooth_uart(MyBlueTooth& bluetooth) {
	log_bluetooth_characteristic(bluetooth, F("UART"), F("AT+UART?"));
}
void log_bluetooth_characteristic(MyBlueTooth& bluetooth, const __FlashStringHelper* label, const __FlashStringHelper* command) {
	char response[bluetooth_response_buffer_size];
	Serial.print(F("INTERROGAZIONE BLUETOOTH - "));
	Serial.println(label);
	Serial.flush();
	bluetooth.clearBuffer();
	bluetooth.ProgramMode();
	clear_bluetooth_serial_input();
	Serial.println(command);
	const bool has_response = read_bluetooth_response(response, sizeof(response));
	// La risposta viene stampata soltanto dopo avere spento il modulo.
	delay(100);
	Serial.print(label);
	Serial.print(F(": "));
	if (!has_response) {
		Serial.println(F("nessuna risposta"));
		return;
	}
	Serial.println(response);
}
void log_bluetooth_characteristics(MyBlueTooth& bluetooth) {
	Serial.begin(serial_monitor_baud_rate);
	Serial.println(F("--- ATTENZIONE!!!USARE 5Volts pieni Diagnostica Bluetooth ---"));
	log_bluetooth_connection(bluetooth);
	log_bluetooth_version(bluetooth);
	log_bluetooth_name(bluetooth);
	log_bluetooth_address(bluetooth);
	log_bluetooth_password(bluetooth);
	log_bluetooth_role(bluetooth);
	log_bluetooth_uart(bluetooth);
	Serial.println(F("--- Fine diagnostica Bluetooth ---"));
}
void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
	static MyBlueTooth bluetooth(&Serial, bluetooth_key_pin, bluetooth_power_pin, bluetooth_program_baud_rate, bluetooth_receive_baud_rate);

#if EEPROM_INITIALIZATION_ENABLED
	Serial.println(F("ATTENZIONE: SCRITTURA EEPROM ABILITATA"));
	Serial.println(F("CANCELLAZIONE E INIZIALIZZAZIONE EEPROM IN CORSO"));
	clear_eeprom();
	initialize_configuration();
	Serial.println(F("SCRITTURA EEPROM COMPLETATA"));
#else
	Serial.println(F("SCRITTURA EEPROM DISABILITATA"));
#endif
	log_bluetooth_characteristics(bluetooth);
	digitalWrite(LED_BUILTIN, HIGH);
}
void loop() {
}
