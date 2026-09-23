#include "BluetoothFrameWriter.h"

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
    BluetoothProgramText source) {

    if (source == nullptr) {
        return;
    }

    char character = bluetooth_program_read_char(source++);

    while (character != '\0') {
        append_char(destination, capacity, character);
        character = bluetooth_program_read_char(source++);
    }
}

void BluetoothFrameWriter::append_uint(
    char* destination,
    uint8_t capacity,
    unsigned long value) {

    char number[11] = {};
    uint8_t length = 0U;

    do {
        number[length++] =
            static_cast<char>('0' + (value % 10UL));

        value /= 10UL;
    }
    while (value != 0UL && length < sizeof(number));

    while (length > 0U) {
        append_char(
            destination,
            capacity,
            number[--length]);
    }
}

void BluetoothFrameWriter::send_frame(
    const char* message,
    BluetoothCommandUtil::CommandTypeC commandType) {

    char frame[frame_size] = {};

    append_text(frame, sizeof(frame), message);

    BluetoothCommandUtil::append_command_type(
        frame,
        sizeof(frame),
        commandType);

    bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_program_frame(
    BluetoothProgramText message,
    BluetoothCommandUtil::CommandTypeA commandType,
    uint8_t commandCode) {

    char frame[frame_size] = {};

    append_program_text(
        frame,
        sizeof(frame),
        message);

    BluetoothCommandUtil::append_command_type(
        frame,
        sizeof(frame),
        commandType,
        commandCode);

    bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_program_frame(
    BluetoothProgramText message,
    BluetoothCommandUtil::CommandTypeB commandType) {

    char frame[frame_size] = {};

    append_program_text(
        frame,
        sizeof(frame),
        message);
    BluetoothCommandUtil::append_command_type(frame,sizeof(frame),commandType);
    bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_program_frame(
    BluetoothProgramText message,
    BluetoothCommandUtil::CommandTypeC commandType) {

    char frame[frame_size] = {};

    append_program_text(
        frame,
        sizeof(frame),
        message);

    BluetoothCommandUtil::append_command_type(
        frame,
        sizeof(frame),
        commandType);

    bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_value_frame(
    BluetoothProgramText label,
    const char* value,
    BluetoothCommandUtil::CommandTypeA commandType,
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

    BluetoothCommandUtil::append_command_type(
        frame,
        sizeof(frame),
        commandType,
        commandCode);

    bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_program_value_frame(
    BluetoothProgramText label,
    BluetoothProgramText value,
    BluetoothCommandUtil::CommandTypeC commandType) {

    char frame[frame_size] = {};

    append_program_text(
        frame,
        sizeof(frame),
        label);

    append_program_text(
        frame,
        sizeof(frame),
        value);

    BluetoothCommandUtil::append_command_type(
        frame,
        sizeof(frame),
        commandType);

    bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_value_frame(
    BluetoothProgramText label,
    const char* value,
    BluetoothCommandUtil::CommandTypeC commandType) {

    char frame[frame_size] = {};

    append_program_text(
        frame,
        sizeof(frame),
        label);

    append_text(
        frame,
        sizeof(frame),
        value);

    BluetoothCommandUtil::append_command_type(
        frame,
        sizeof(frame),
        commandType);

    bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_uint_frame(
    BluetoothProgramText label,
    unsigned long value,
    BluetoothCommandUtil::CommandTypeA commandType,
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

    BluetoothCommandUtil::append_command_type(
        frame,
        sizeof(frame),
        commandType,
        commandCode);

    bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_uint_frame(
    BluetoothProgramText label,
    unsigned long value,
    BluetoothCommandUtil::CommandTypeC commandType) {

    char frame[frame_size] = {};

    append_program_text(
        frame,
        sizeof(frame),
        label);

    append_uint(
        frame,
        sizeof(frame),
        value);

    BluetoothCommandUtil::append_command_type(
        frame,
        sizeof(frame),
        commandType);

    bluetoothRepository.println(frame);
}

void BluetoothFrameWriter::send_end() {

    send_program_frame(
        BT_TEXT(""),
        BluetoothCommandUtil::EndTrasmission);
}
