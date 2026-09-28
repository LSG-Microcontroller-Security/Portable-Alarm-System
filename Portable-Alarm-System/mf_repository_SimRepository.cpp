// mf_repository_SimRepository.cpp
#include "mf_repository_SimRepository.h"
#include <Arduino.h>
#include <string.h>
#include <stdlib.h>

#ifndef _DEBUG_FOR_SERIAL
#define _DEBUG_FOR_SERIAL 0U
#endif

#define AT_SMS_HEADER                "+CMGR:"
#define AT_SET_SMS_TEXT_MODE         "AT+CMGF=1"
#define AT_SET_SMS_PDU_MODE          "AT+CMGF=0"
#define AT_SET_SMS_CHARSET           "AT+CSCS=\"GSM\""
#define ATD_COMMAND                  "ATD"
#define HANGUP_COMMAND               "ATH"
#define AT_LIST_CURRENT_CALLS         "AT+CLCC"
#define AT_AUTO_ANSWER_PREFIX         "ATS0="
#define AT_SEND_SMS_PREFIX           "AT+CMGS=\""
#define AT_SEND_SMS_HEADER           "+CMGS:"
#define LITERAL_OK                   "OK"
#define LITERAL_ERROR                "ERROR"
#define LITERAL_PROMPT               ">"
#define LITERAL_CTRL_Z               ((char)26)
#define LITERAL_RETURN               "\r\n"
#define AT_READ_SMS_PREFIX           "AT+CMGR="
#define AT_DELETE_ALL_SMS            "AT+CMGD=1,4"
#define AT_SET_SMS_MEMORY            "AT+CPMS=\"SM\",\"SM\",\"SM\""
#define AT_GET_SMS_STORAGE_STATUS    "AT+CPMS?"
#define AT_SMS_STORAGE_HEADER        "+CPMS:"
#define AT_DELETE_SMS_PREFIX         "AT+CMGD="
#define AT_NETWORK_REGISTRATION      "AT+CREG?"
#define AT_SIGNAL_STRENGTH           "AT+CSQ"
#define AT_GPRS_ATTACHED             "AT+CGATT?"
#define AT_SET_CNMI                  "AT+CNMI=2,1,0,0,0"
#define AT_SET_NETLIGHT_OFF          "AT+CNETLIGHT=0"
#define AT_SET_NETLIGHT_ON           "AT+CNETLIGHT=1"
#define AT_ENABLE_DTR_SLEEP          "AT+CSCLK=1"
#define AT_BASIC_COMMAND             "AT"
#define RESPONSE_NETWORK_REGISTRATION_HEADER "+CREG:"
#define RESPONSE_SIGNAL_STRENGTH_HEADER "+CSQ:"
#define RESPONSE_GPRS_ATTACHED_HEADER "+CGATT:"
#define RESPONSE_OK_WITH_CRLF        "\r\nOK\r\n"
#define RESPONSE_ERROR_WITH_CRLF     "\r\nERROR\r\n"
#define RESPONSE_OK_SUFFIX           "OK\r\n"
#define RESPONSE_ERROR_SUFFIX        "ERROR\r\n"
#define DEBUG_SMS_LABEL              "sms"

namespace {
	constexpr uint8_t sim_pin_not_configured = 255U;
	constexpr uint8_t sim_small_response_buffer_size = 20;
	constexpr uint8_t sim_sms_message_buffer_size = 12;
	constexpr unsigned long sim_read_sms_timeout_ms = 900UL;

	inline void send_at_cmd(ISerial& serial, const char* cmd) {
		serial.print(cmd);
		serial.write((uint8_t)'\r');
	}

	inline void write_uint8(ISerial& serial, uint8_t value) {
		if (value >= 100U) {
			serial.write((uint8_t)('0' + (value / 100U)));
			value %= 100U;
			serial.write((uint8_t)('0' + (value / 10U)));
			serial.write((uint8_t)('0' + (value % 10U)));
			return;
		}

		if (value >= 10U) {
			serial.write((uint8_t)('0' + (value / 10U)));
			serial.write((uint8_t)('0' + (value % 10U)));
			return;
		}

		serial.write((uint8_t)('0' + value));
	}
}

SimRepository::SimRepository(ISerial& serial)
	: serial_(serial), sleep_pin_(sim_pin_not_configured), boot_pin_(sim_pin_not_configured), is_sms_receive_initialized_(false), is_call_disabled_(false) {
}

SimRepository::SimRepository(ISerial& serial, unsigned long baud_rate)
	: SimRepository(serial) {
	begin(baud_rate);
}

SimRepository::SimRepository(ISerial& serial, uint8_t sleep_pin, unsigned long baud_rate, uint8_t boot_pin)
	: serial_(serial), sleep_pin_(sleep_pin), boot_pin_(boot_pin), is_sms_receive_initialized_(false), is_call_disabled_(false) {
	begin(baud_rate);
	if (sleep_pin_ != sim_pin_not_configured) {
		pinMode(sleep_pin_, OUTPUT);
		digitalWrite(sleep_pin_, LOW);
	}
	if (boot_pin_ != sim_pin_not_configured) {
		digitalWrite(boot_pin_, LOW);
		pinMode(boot_pin_, INPUT);
	}
}

void SimRepository::begin(unsigned long baud_rate) {
	serial_.begin(baud_rate);
}

void SimRepository::call(const char* number) {
	if (is_call_disabled_) {
		return;
	}
	clear_receive_buffer();
	serial_.print(ATD_COMMAND);
	serial_.print(number);
	serial_.write((uint8_t)';');
	serial_.write((uint8_t)'\r');
	wait_for_pattern(LITERAL_OK, 2000UL);
}

void SimRepository::hangUp() {
	clear_receive_buffer();
	send_at_cmd(serial_, HANGUP_COMMAND);
	wait_for_pattern(LITERAL_OK, 2000UL);
}

bool SimRepository::isCallActive() {
	clear_receive_buffer();
	send_at_cmd(serial_, AT_LIST_CURRENT_CALLS);
	const char* call_header = "+CLCC:";
	uint8_t call_header_pos = 0U;
	uint8_t ok_pos = 0U;
	unsigned long start = millis();
	while (static_cast<unsigned long>(millis() - start) < 2000UL) {
		if (serial_.available() <= 0) { continue; }
		int read_value = serial_.read();
		if (read_value < 0) { continue; }
		char c = static_cast<char>(read_value);
		if (c == call_header[call_header_pos]) {
			++call_header_pos;
			if (call_header[call_header_pos] == '\0') { return true; }
		}
		else {
			call_header_pos = (c == call_header[0]) ? 1U : 0U;
		}
		if (c == LITERAL_OK[ok_pos]) {
			++ok_pos;
			if (LITERAL_OK[ok_pos] == '\0') { return false; }
		}
		else {
			ok_pos = (c == LITERAL_OK[0]) ? 1U : 0U;
		}
	}
	return false;
}

void SimRepository::enableIncomingCall(uint8_t number_of_rings) {
	is_call_disabled_ = true;
	clear_receive_buffer();
	serial_.print(AT_AUTO_ANSWER_PREFIX);
	write_uint8(serial_, number_of_rings);
	serial_.write((uint8_t)'\r');
}

void SimRepository::disableIncomingCall() {
	clear_receive_buffer();
	serial_.print(AT_AUTO_ANSWER_PREFIX);
	serial_.write((uint8_t)'0');
	serial_.write((uint8_t)'\r');
	delay(100UL);
	send_at_cmd(serial_, HANGUP_COMMAND);
	is_call_disabled_ = false;
}

void SimRepository::setSmsTextMode() {
	clear_receive_buffer();
	send_at_cmd(serial_, AT_SET_SMS_TEXT_MODE);
	wait_for_pattern(LITERAL_OK, 2000UL);
}

void SimRepository::setSmsPduMode() {
	clear_receive_buffer();
	send_at_cmd(serial_, AT_SET_SMS_PDU_MODE);
	wait_for_pattern(LITERAL_OK, 2000UL);
}

void SimRepository::sendSms(const char* number, const char* message) {
	serial_.listen();
	clear_receive_buffer();
	send_at_cmd(serial_, AT_SET_SMS_TEXT_MODE);
	wait_for_pattern(LITERAL_OK, 2000UL);
	clear_receive_buffer();
	serial_.print(AT_SEND_SMS_PREFIX);
	serial_.print(number);
#if _DEBUG_FOR_SERIAL && !_ON_MOCKING_TESTS
	Serial.print(F(DEBUG_SMS_LABEL)); Serial.println(number);
#endif
	serial_.write((uint8_t)'"');
	serial_.write((uint8_t)'\r');
	wait_for_pattern(LITERAL_PROMPT, 3000UL);
	serial_.print(message);
	serial_.write((uint8_t)LITERAL_CTRL_Z);
	wait_for_pattern(AT_SEND_SMS_HEADER, 12000UL) || wait_for_pattern(LITERAL_OK, 12000UL);
}

void SimRepository::initSmsReception() {
	if (is_sms_receive_initialized_) {
		return;
	}

	bool ok = true;
	clear_receive_buffer();
	send_at_cmd(serial_, AT_SET_SMS_MEMORY);
	ok = ok && wait_for_pattern(LITERAL_OK, 5000UL);

	clear_receive_buffer();
	send_at_cmd(serial_, AT_SET_SMS_TEXT_MODE);
	ok = ok && wait_for_pattern(LITERAL_OK, 5000UL);

	clear_receive_buffer();
	send_at_cmd(serial_, AT_SET_SMS_CHARSET);
	ok = ok && wait_for_pattern(LITERAL_OK, 5000UL);

	clear_receive_buffer();
	send_at_cmd(serial_, AT_SET_CNMI);
	ok = ok && wait_for_pattern(LITERAL_OK, 5000UL);

	is_sms_receive_initialized_ = ok;
}

bool SimRepository::setNetlightEnabled(bool enabled) {
	clear_receive_buffer();
	delay(5000);
	send_at_cmd(serial_, enabled ? AT_SET_NETLIGHT_ON : AT_SET_NETLIGHT_OFF);
	return wait_for_pattern(LITERAL_OK, 35000UL);
}

bool SimRepository::readSms(uint8_t index, char* message) {
	return readSms(index, nullptr, 0U, message, sim_sms_message_buffer_size);
}

bool SimRepository::readSms(uint8_t index, char* sender, uint8_t sender_capacity, char* message, uint8_t message_capacity) {
	if (message == nullptr || message_capacity == 0U) {
		return false;
	}

	message[0] = '\0';
	if (sender != nullptr && sender_capacity > 0U) {
		sender[0] = '\0';
	}

	if (!is_sms_receive_initialized_) {
		initSmsReception();
	}

	clear_receive_buffer();

	serial_.print(AT_READ_SMS_PREFIX);
	write_uint8(serial_, index);
	serial_.write((uint8_t)'\r');

	uint8_t state = 0U;
	uint8_t header_pos = 0U;
	uint8_t sender_pos = 0U;
	uint8_t quote_count = 0U;
	uint8_t message_pos = 0U;
	unsigned long start = millis();

	char trailer[8] = {};
	uint8_t trailer_pos = 0U;

	while (static_cast<unsigned long>(millis() - start) < sim_read_sms_timeout_ms && message_pos < static_cast<uint8_t>(message_capacity - 1U)) {
		if (serial_.available() <= 0) {
			continue;
		}

		int read_value = serial_.read();

		if (read_value < 0) {
			continue;
		}

		char c = static_cast<char>(read_value);

		if (trailer_pos < static_cast<uint8_t>(sizeof(trailer) - 1U)) {
			trailer[trailer_pos++] = c;
		}
		else {
			for (uint8_t i = 0U; i < static_cast<uint8_t>(sizeof(trailer) - 2U); ++i) {
				trailer[i] = trailer[i + 1U];
			}
			trailer[static_cast<uint8_t>(sizeof(trailer) - 2U)] = c;
		}
		trailer[trailer_pos < static_cast<uint8_t>(sizeof(trailer)) ? trailer_pos : static_cast<uint8_t>(sizeof(trailer) - 1U)] = '\0';

		if (strstr(trailer, LITERAL_ERROR) != nullptr) {
			break;
		}
		if (state >= 2U && strstr(trailer, LITERAL_OK) != nullptr) {
			break;
		}

		if (state == 0U) {
			if (c == AT_SMS_HEADER[header_pos]) {
				++header_pos;

				if (AT_SMS_HEADER[header_pos] == '\0') {
					state = 1U;
				}
			}
			else {
				header_pos = (c == AT_SMS_HEADER[0]) ? 1U : 0U;
			}

			continue;
		}

		if (state == 1U) {
			if (c == '\n') {
				if (sender != nullptr && sender_capacity > 0U) {
					sender[sender_pos] = '\0';
				}
				state = 2U;
				continue;
			}

			if (c == '"') {
				++quote_count;
				continue;
			}

			if (quote_count == 3U && sender != nullptr && sender_capacity > 0U && sender_pos < static_cast<uint8_t>(sender_capacity - 1U)) {
				sender[sender_pos++] = c;
			}

			continue;
		}

		if (state == 2U) {
			if (c == '\r' || c == '\n') {
				continue;
			}

			message[message_pos++] = c;
			state = 3U;
			continue;
		}

		if (c == '\r' || c == '\n') {
			break;
		}

		message[message_pos++] = c;
	}

	message[message_pos] = '\0';
	if (sender != nullptr && sender_capacity > 0U) {
		sender[sender_pos] = '\0';
	}

	return message_pos > 0U;
}

void SimRepository::deleteAllSms() {
	clear_receive_buffer();
	send_at_cmd(serial_, AT_DELETE_ALL_SMS);
	wait_for_pattern(LITERAL_OK, 3000UL);
}

int SimRepository::getSmsCount() {
	if (!is_sms_receive_initialized_) {
		initSmsReception();

		if (!is_sms_receive_initialized_) {
			return -1;
		}
	}

	clear_receive_buffer();
	send_at_cmd(serial_, AT_GET_SMS_STORAGE_STATUS);

	char buffer[32];
	read_response(buffer, static_cast<uint8_t>(sizeof(buffer)), 3000UL);

	char* header = strstr(buffer, AT_SMS_STORAGE_HEADER);
	if (header == nullptr) {
		return -1;
	}

	char* first_comma = strchr(header, ',');
	if (first_comma == nullptr) {
		return -1;
	}

	int used = atoi(first_comma + 1);
	if (used < 0) {
		return -1;
	}

	return used;
}

bool SimRepository::deleteSmsAt(uint8_t index) {
	clear_receive_buffer();
	serial_.print(AT_DELETE_SMS_PREFIX);
	write_uint8(serial_, index);
	serial_.write((uint8_t)'\r');
	return wait_for_pattern(LITERAL_OK, 2000UL);
}

uint8_t SimRepository::findSmsWithContent(const char* pattern) {
	if (pattern == nullptr || pattern[0] == '\0') {
		return 0;
	}

	if (!is_sms_receive_initialized_) {
		initSmsReception();

		if (!is_sms_receive_initialized_) {
			return 0;
		}
	}

	int sms_count = getSmsCount();
	if (sms_count <= 0) {
		return 0;
	}

	uint8_t max_index_to_scan = static_cast<uint8_t>(sms_count + 10);
	if (max_index_to_scan < static_cast<uint8_t>(sms_count)) {
		max_index_to_scan = 255;
	}

	char buffer[sim_sms_message_buffer_size];

	for (uint8_t i = 1; i <= max_index_to_scan; ++i) {
		if (!readSms(i, buffer)) {
			continue;
		}

		if (strcmp(buffer, pattern) == 0) {
			return i;
		}
	}

	return 0;
}

bool SimRepository::isRegisteredToNetwork() {
	serial_.listen();
	clear_receive_buffer();
	send_at_cmd(serial_, AT_NETWORK_REGISTRATION);

	const char* response_header = RESPONSE_NETWORK_REGISTRATION_HEADER;
	uint8_t header_index = 0U;
	bool is_header_found = false;
	bool is_comma_found = false;
	char registration_status = '\0';
	unsigned long start = millis();

	while (static_cast<unsigned long>(millis() - start) < 2000UL) {
		if (serial_.available() <= 0) {
			continue;
		}

		int read_value = serial_.read();
		if (read_value < 0) {
			continue;
		}

		char c = static_cast<char>(read_value);
		if (!is_header_found) {
			if (c == response_header[header_index]) {
				++header_index;
				if (response_header[header_index] == '\0') {
					is_header_found = true;
					is_comma_found = false;
				}
			}
			else {
				header_index = (c == response_header[0]) ? 1U : 0U;
			}
			continue;
		}

		if (!is_comma_found) {
			if (c == ',') {
				is_comma_found = true;
			}
			else if (c == '\r' || c == '\n') {
				is_header_found = false;
				header_index = 0U;
			}
			continue;
		}

		if (c >= '0' && c <= '9') {
			registration_status = c;
			break;
		}

		if (c == '\r' || c == '\n') {
			is_header_found = false;
			is_comma_found = false;
			header_index = 0U;
		}
	}

	return registration_status == '1' || registration_status == '5';
}

int SimRepository::getSignalStrength() {
	clear_receive_buffer();
	send_at_cmd(serial_, AT_SIGNAL_STRENGTH);

	char buffer[sim_small_response_buffer_size];
	read_response(buffer, sim_small_response_buffer_size, 2000UL);

	char* csq = strstr(buffer, RESPONSE_SIGNAL_STRENGTH_HEADER);
	if (csq == nullptr) {
		return -1;
	}

	int rssi = atoi(csq + 5);

	if (rssi >= 0 && rssi <= 31) {
		return rssi;
	}

	return -1;
}

bool SimRepository::isGprsAttached() {
	clear_receive_buffer();
	send_at_cmd(serial_, AT_GPRS_ATTACHED);

	char buffer[sim_small_response_buffer_size];
	read_response(buffer, sim_small_response_buffer_size, 2000UL);

	char* ptr = strstr(buffer, RESPONSE_GPRS_ATTACHED_HEADER);
	if (ptr == nullptr) {
		return false;
	}

	int status = atoi(ptr + 7);

	return status == 1;
}

bool SimRepository::enterSleepMode() {
	if (sleep_pin_ == sim_pin_not_configured) {
		return false;
	}
	serial_.listen();
	digitalWrite(sleep_pin_, LOW);
	delay(5000);
	clear_receive_buffer();
	send_at_cmd(serial_, AT_ENABLE_DTR_SLEEP);
	if (!wait_for_pattern(LITERAL_OK, 3000UL)) {
		return false;
	}
	digitalWrite(sleep_pin_, HIGH);
	return true;
}

bool SimRepository::exitSleepMode() {
	if (sleep_pin_ == sim_pin_not_configured) {
		return false;
	}
	serial_.listen();
	digitalWrite(sleep_pin_, LOW);
	delay(200UL);
	clear_receive_buffer();
	send_at_cmd(serial_, AT_BASIC_COMMAND);
	return wait_for_pattern(LITERAL_OK, 3000UL);
}

bool SimRepository::turn_on_module() {
	if (boot_pin_ == sim_pin_not_configured) {
		return false;
	}

	if (is_module_on()) {
		return true;
	}

	digitalWrite(boot_pin_, LOW);
	pinMode(boot_pin_, OUTPUT);
	delay(1100UL);
	pinMode(boot_pin_, INPUT);
	delay(5000UL);
	return is_module_on();
}

bool SimRepository::turn_off_module() {
	if (boot_pin_ == sim_pin_not_configured) {
		return false;
	}

	if (!is_module_on()) {
		return true;
	}

	digitalWrite(boot_pin_, LOW);
	pinMode(boot_pin_, OUTPUT);
	delay(1600UL);
	pinMode(boot_pin_, INPUT);
	delay(3000UL);
	return !is_module_on();
}

bool SimRepository::is_module_on() {
	serial_.listen();
	clear_receive_buffer();
	send_at_cmd(serial_, AT_BASIC_COMMAND);
	return wait_for_pattern(LITERAL_OK, 1500UL);
}

void SimRepository::clear_receive_buffer() {
	while (serial_.available() > 0) {
		serial_.read();
	}
}

bool SimRepository::wait_for_pattern(const char* pattern, unsigned long timeout_ms) {
	if (pattern == nullptr || pattern[0] == '\0') {
		return false;
	}

	const char* match = pattern;
	unsigned long start = millis();

	while (static_cast<unsigned long>(millis() - start) < timeout_ms) {
		if (serial_.available() <= 0) {
			continue;
		}

		int read_value = serial_.read();

		if (read_value < 0) {
			continue;
		}

		char c = static_cast<char>(read_value);

		if (c == *match) {
			++match;

			if (*match == '\0') {
				return true;
			}
		}
		else {
			match = (c == pattern[0]) ? pattern + 1 : pattern;

			if (*match == '\0') {
				return true;
			}
		}
	}

	return false;
}

uint8_t SimRepository::read_response(char* buffer, uint8_t buffer_size, unsigned long timeout_ms) {
	if (buffer == nullptr || buffer_size == 0U) {
		return 0;
	}

	uint8_t pos = 0U;
	unsigned long start = millis();

	while (static_cast<unsigned long>(millis() - start) < timeout_ms && pos < static_cast<uint8_t>(buffer_size - 1U)) {
		if (serial_.available() <= 0) {
			continue;
		}

		int read_value = serial_.read();

		if (read_value < 0) {
			continue;
		}

		buffer[pos++] = static_cast<char>(read_value);
		buffer[pos] = '\0';

		if (strstr(buffer, RESPONSE_OK_WITH_CRLF) != nullptr || strstr(buffer, RESPONSE_ERROR_WITH_CRLF) != nullptr) {
			break;
		}

		if (strstr(buffer, RESPONSE_OK_SUFFIX) != nullptr || strstr(buffer, RESPONSE_ERROR_SUFFIX) != nullptr) {
			break;
		}
	}

	buffer[pos] = '\0';

	return pos;
}

bool SimRepository::listen() {
	return serial_.listen();
}

bool SimRepository::is_listening() {
	return serial_.isListening();
}

bool SimRepository::stop_listening() {
	return serial_.stopListening();
}

void SimRepository::delay(unsigned long ms) {
	::delay(ms);
}
