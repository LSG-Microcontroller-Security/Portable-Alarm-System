#pragma once

#include <Arduino.h>

#if defined(__AVR__)
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif

#include <mf_repository_BlueToothRepository.h>
#include "BluetoothCommandUtil2.h"

class BluetoothFrameWriter {
public:
	BluetoothFrameWriter(BlueToothRepository& bluetoothRepository);

	void send_frame(
		const char* message,
		BluetoothCommandUtil2::CommandTypeC commandType);

	void send_program_frame(
		PGM_P message,
		BluetoothCommandUtil2::CommandTypeA commandType,
		uint8_t commandCode);

	void send_program_frame(
		PGM_P message,
		BluetoothCommandUtil2::CommandTypeB commandType);

	void send_program_frame(
		PGM_P message,
		BluetoothCommandUtil2::CommandTypeC commandType);

	void send_value_frame(
		PGM_P label,
		const char* value,
		BluetoothCommandUtil2::CommandTypeA commandType,
		uint8_t commandCode);

	void send_program_value_frame(
		PGM_P label,
		PGM_P value,
		BluetoothCommandUtil2::CommandTypeC commandType);

	void send_value_frame(
		PGM_P label,
		const char* value,
		BluetoothCommandUtil2::CommandTypeC commandType);

	void send_uint_frame(
		PGM_P label,
		unsigned long value,
		BluetoothCommandUtil2::CommandTypeA commandType,
		uint8_t commandCode);

	void send_uint_frame(
		PGM_P label,
		unsigned long value,
		BluetoothCommandUtil2::CommandTypeC commandType);

	void send_end();

private:
	static const uint8_t frame_size = 64U;

	BlueToothRepository& bluetoothRepository;

	static void append_char(
		char* destination,
		uint8_t capacity,
		char value);

	static void append_text(
		char* destination,
		uint8_t capacity,
		const char* source);

	static void append_program_text(
		char* destination,
		uint8_t capacity,
		PGM_P source);

	static void append_uint(
		char* destination,
		uint8_t capacity,
		unsigned long value);
};
