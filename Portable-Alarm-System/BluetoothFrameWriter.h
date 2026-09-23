#pragma once
#include <stdint.h>
#include <string.h>
#if defined(__AVR__)
#include <Arduino.h>
#include <avr/pgmspace.h>
typedef PGM_P BluetoothProgramText;
#define BT_TEXT(value) PSTR(value)
inline char bluetooth_program_read_char(BluetoothProgramText source) {
    return static_cast<char>(pgm_read_byte(source));
}
inline const char* bluetooth_find_program_text( const char* text,BluetoothProgramText pattern) {
    return strstr_P(text, pattern);
}
inline unsigned long bluetooth_millis() {
    return millis();
}
inline void bluetooth_delay(unsigned long milliseconds) {
    delay(milliseconds);
}
#else
typedef const char* BluetoothProgramText;
#define BT_TEXT(value) (value)
inline char bluetooth_program_read_char(BluetoothProgramText source) {
    return *source;
}
inline const char* bluetooth_find_program_text(const char* text,BluetoothProgramText pattern) {
    return strstr(text, pattern);
}
inline unsigned long bluetooth_millis() {
    return 0UL;
}

inline void bluetooth_delay(unsigned long milliseconds) {
    (void)milliseconds;
}

#endif

#include <mf_repository_BlueToothRepository.h>

#include "BluetoothCommandUtil.h"

class BluetoothFrameWriter {
public:
    BluetoothFrameWriter(BlueToothRepository& bluetoothRepository);

    void send_frame(
        const char* message,
        BluetoothCommandUtil::CommandTypeC commandType);

    void send_program_frame(
        BluetoothProgramText message,
        BluetoothCommandUtil::CommandTypeA commandType,
        uint8_t commandCode);

    void send_program_frame(
        BluetoothProgramText message,
        BluetoothCommandUtil::CommandTypeB commandType);

    void send_program_frame(
        BluetoothProgramText message,
        BluetoothCommandUtil::CommandTypeC commandType);

    void send_value_frame(
        BluetoothProgramText label,
        const char* value,
        BluetoothCommandUtil::CommandTypeA commandType,
        uint8_t commandCode);

    void send_program_value_frame(
        BluetoothProgramText label,
        BluetoothProgramText value,
        BluetoothCommandUtil::CommandTypeC commandType);

    void send_value_frame(
        BluetoothProgramText label,
        const char* value,
        BluetoothCommandUtil::CommandTypeC commandType);

    void send_uint_frame(
        BluetoothProgramText label,
        unsigned long value,
        BluetoothCommandUtil::CommandTypeA commandType,
        uint8_t commandCode);

    void send_uint_frame(
        BluetoothProgramText label,
        unsigned long value,
        BluetoothCommandUtil::CommandTypeC commandType);

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
        BluetoothProgramText source);

    static void append_uint(
        char* destination,
        uint8_t capacity,
        unsigned long value);
};
