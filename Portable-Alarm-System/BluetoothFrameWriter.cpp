#include "BluetoothFrameWriter.h"

#include <stdlib.h>
#include <string.h>

BluetoothFrameWriter::BluetoothFrameWriter(
	BlueToothRepository& bluetoothRepository)
	: bluetoothRepository(bluetoothRepository) {
}

void BluetoothFrameWriter::append_char(
	char* destination,
	uint8_t capacity,
	char value) {

	if (destination == nullptr || capacity == 0U) {
		return;
	}

	uint8_t length = 0U;

	while (
		length < capacity &&
		destination[length] != '\0') {

		length++;
	}

	if (length + 1U < capacity) {
		destination[length] = value;
		destination[length + 1U] = '\0';
	}
}

void BluetoothFrameWriter::append_text(
	char* destination,
	uint8_t capacity,
	const char* source) {

	if (source == nullptr) {
		return;
	}

	while (*source != '\0') {
		append_char(destination, capacity, *source++);
	}
}

void BluetoothFrameWriter::append_program_text(
	char* destination,
	uint8_t capacity,
	PGM_P source) {

	if (source == nullptr) {
		return;
	}

	char character = pgm_read_byte(source++);

	while (character != '\0') {
		append_char(destination, capacity, character);
		character = pgm_read_byte(source++);
	}
}

void BluetoothFrameWriter::append_uint(
	char* destination,
	uint8_t capacity,
	unsigned long value) {

	char number[11] = {};
	ultoa(value, number, 10);
	append_text(destination, capacity, number);
}

void BluetoothFrameWriter::send_frame(
	const char* message,
	BluetoothCommandUtil2::CommandTypeC commandType) {

	char frame[frame_size] = {};

	append_text(frame, sizeof(frame), message);

	BluetoothCommandUtil2::append_command_type(
		frame,
		sizeof(frame),
		commandType);

	bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_program_frame(
	PGM_P message,
	BluetoothCommandUtil2::CommandTypeA commandType,
	uint8_t commandCode) {

	char frame[frame_size] = {};

	append_program_text(
		frame,
		sizeof(frame),
		message);

	BluetoothCommandUtil2::append_command_type(
		frame,
		sizeof(frame),
		commandType,
		commandCode);

	bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_program_frame(
	PGM_P message,
	BluetoothCommandUtil2::CommandTypeB commandType) {

	char frame[frame_size] = {};

	append_program_text(
		frame,
		sizeof(frame),
		message);

	BluetoothCommandUtil2::append_command_type(
		frame,
		sizeof(frame),
		commandType);

	bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_program_frame(
	PGM_P message,
	BluetoothCommandUtil2::CommandTypeC commandType) {

	char frame[frame_size] = {};

	append_program_text(
		frame,
		sizeof(frame),
		message);

	BluetoothCommandUtil2::append_command_type(
		frame,
		sizeof(frame),
		commandType);

	bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_value_frame(
	PGM_P label,
	const char* value,
	BluetoothCommandUtil2::CommandTypeA commandType,
	uint8_t commandCode) {

	char frame[frame_size] = {};

	append_program_text(
		frame,
		sizeof(frame),
		label);

	append_text(
		frame,
		sizeof(frame),
		value);

	BluetoothCommandUtil2::append_command_type(
		frame,
		sizeof(frame),
		commandType,
		commandCode);

	bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_program_value_frame(
	PGM_P label,
	PGM_P value,
	BluetoothCommandUtil2::CommandTypeC commandType) {

	char frame[frame_size] = {};

	append_program_text(
		frame,
		sizeof(frame),
		label);

	append_program_text(
		frame,
		sizeof(frame),
		value);

	BluetoothCommandUtil2::append_command_type(
		frame,
		sizeof(frame),
		commandType);

	bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_value_frame(
	PGM_P label,
	const char* value,
	BluetoothCommandUtil2::CommandTypeC commandType) {

	char frame[frame_size] = {};

	append_program_text(
		frame,
		sizeof(frame),
		label);

	append_text(
		frame,
		sizeof(frame),
		value);

	BluetoothCommandUtil2::append_command_type(
		frame,
		sizeof(frame),
		commandType);

	bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_uint_frame(
	PGM_P label,
	unsigned long value,
	BluetoothCommandUtil2::CommandTypeA commandType,
	uint8_t commandCode) {

	char frame[frame_size] = {};

	append_program_text(
		frame,
		sizeof(frame),
		label);

	append_uint(
		frame,
		sizeof(frame),
		value);

	BluetoothCommandUtil2::append_command_type(
		frame,
		sizeof(frame),
		commandType,
		commandCode);

	bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_uint_frame(
	PGM_P label,
	unsigned long value,
	BluetoothCommandUtil2::CommandTypeC commandType) {

	char frame[frame_size] = {};

	append_program_text(
		frame,
		sizeof(frame),
		label);

	append_uint(
		frame,
		sizeof(frame),
		value);

	BluetoothCommandUtil2::append_command_type(
		frame,
		sizeof(frame),
		commandType);

	bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_end() {

	send_program_frame(
		PSTR(""),
		BluetoothCommandUtil2::EndTrasmission);
}
