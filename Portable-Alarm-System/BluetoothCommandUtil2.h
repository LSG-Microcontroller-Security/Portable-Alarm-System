// BluetoothCommandUtil2.h
#pragma once

#include <stdint.h>

class BluetoothCommandUtil2 {
public:
	enum CommandTypeA : uint8_t { Command, Data, Menu };
	enum CommandTypeB : uint8_t { EndTrasmission };
	enum CommandTypeC : uint8_t { Message, Title, Info };

	static void append_command_type(char* destination, uint8_t capacity, CommandTypeA command_type, uint8_t command_code) {
		append_char(destination, capacity, ';');

		switch (command_type) {
		case Command:
			append_char(destination, capacity, 'C');
			break;
		case Data:
			append_char(destination, capacity, 'D');
			break;
		case Menu:
			append_char(destination, capacity, 'M');
			break;
		}

		append_char(destination, capacity, '0' + (command_code / 100U));
		append_char(destination, capacity, '0' + ((command_code / 10U) % 10U));
		append_char(destination, capacity, '0' + (command_code % 10U));
	}

	static void append_command_type(char* destination, uint8_t capacity, CommandTypeB command_type) {
		if (command_type == EndTrasmission) {
			append_char(destination, capacity, ';');
			append_char(destination, capacity, 'E');
		}
	}

	static void append_command_type(char* destination, uint8_t capacity, CommandTypeC command_type) {
		append_char(destination, capacity, ';');

		switch (command_type) {
		case Message:
			append_char(destination, capacity, 'M');
			append_char(destination, capacity, 'E');
			append_char(destination, capacity, 'S');
			break;
		case Title:
			append_char(destination, capacity, 'T');
			append_char(destination, capacity, 'I');
			append_char(destination, capacity, 'T');
			break;
		case Info:
			append_char(destination, capacity, 'I');
			append_char(destination, capacity, 'N');
			append_char(destination, capacity, 'F');
			break;
		}
	}

private:
	static void append_char(char* destination, uint8_t capacity, char value) {
		uint8_t length = 0;
		while (length < capacity && destination[length] != '\0') {
			length++;
		}

		if (length + 1U < capacity) {
			destination[length] = value;
			destination[length + 1U] = '\0';
		}
	}
};