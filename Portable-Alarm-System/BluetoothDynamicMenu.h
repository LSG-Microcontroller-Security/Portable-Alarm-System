#pragma once

#include <stdint.h>

#include "BluetoothFrameWriter.h"

class BluetoothDynamicMenu {
public:
    static void process();
    static void loadMainMenu();
    static void loadConfigurationMenu();
    static void loadSecurityMenu();
private:
    static const uint8_t bluetooth_receive_buffer_size = 64U;

    static bool contains(
        const char* bluetooth_data,
        BluetoothProgramText command);

    static bool read_value(
        const char* bluetooth_data,
        char* value,
        uint8_t capacity);

    static bool is_numeric(const char* value);

    static bool copy_value(
        char* destination,
        uint8_t capacity,
        const char* value);

    static BluetoothProgramText calculate_battery_level(
        float battery_level);

    static void write_eeprom(
        uint8_t address,
        const char* value);
};
