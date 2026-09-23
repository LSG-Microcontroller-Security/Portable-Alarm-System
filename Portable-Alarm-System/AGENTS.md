# Portable Alarm System - Development Rules

## General
- Preserve existing behavior before architectural refactoring.
- Minimize Flash and SRAM usage.
- Avoid unnecessary abstractions, objects, buffers, or helper layers.
- Do not rename existing files, classes, methods, or public interfaces unless explicitly requested.

## Memory
- Avoid Arduino String in new code.
- Avoid dynamic allocation unless strictly necessary.
- Prefer fixed-size char buffers.
- Keep constant strings in Flash on AVR.
- Use PSTR() / PROGMEM for constant Bluetooth and menu strings.
- Do not move constant strings into SRAM just to simplify testing.
- Avoid increasing buffer sizes unless required.

## AVR / desktop compatibility
- Higher-level Bluetooth code must remain compilable as normal desktop C++ for future mock tests.
- Use __AVR__ to isolate AVR-specific code.
- AVR-specific headers must only be included inside AVR guards.
- Desktop/mock builds must not require Arduino headers.

Example:

    #if defined(__AVR__)
    // AVR implementation
    #else
    // desktop/mock implementation
    #endif

## PGM_P
- Keep PGM_P in existing public interfaces.
- Do not replace PGM_P with new custom type names unless explicitly requested.

Preferred pattern:

    #if defined(__AVR__)
    #include <avr/pgmspace.h>
    #define BT_TEXT(x) PSTR(x)
    #else
    typedef const char* PGM_P;
    #define BT_TEXT(x) (x)
    #endif

- On AVR, BT_TEXT() must keep constant strings in Flash.
- On desktop/mock builds, it must behave as normal const char*.

## Mock testing
- BlueToothRepository is the main Bluetooth component to mock.
- BluetoothCommandUtil2, BluetoothFrameWriter and BluetoothDynamicMenu must remain testable without Arduino hardware.
- Prefer mocking BlueToothRepository rather than mocking higher-level Bluetooth classes.
- Use _ON_MOCKING_TESTS only when test behavior itself must change.
- Use __AVR__ for platform detection.

## BluetoothCommandUtil2
- Keep it pure C++.
- No Arduino dependency.
- It may remain header-only.
- Keep it lightweight.
- No dynamic allocation.

## BluetoothFrameWriter
- Responsible for building and sending Bluetooth protocol frames.
- May depend on BlueToothRepository.
- Must remain desktop/mock compatible.
- Keep AVR/desktop compatibility helpers inside BluetoothFrameWriter.h.
- Keep PGM_P in the public API.
- Do not use Arduino String.
- Keep the frame buffer small.
- Current target frame size: 64U.

## BluetoothDynamicMenu
- Keep .h and .cpp separate.
- Static methods are acceptable.
- Main application access point should be:

    BluetoothDynamicMenu::process();

- process() handles:
  - Bluetooth input.
  - Command parsing.
  - Menu/action dispatch.
  - Responses.
  - Configuration updates.

- Constant menu strings must remain in Flash on AVR.

## Repository boundary
- BlueToothRepository is the hardware-facing Bluetooth boundary.
- Serial and hardware Bluetooth responsibilities belong in the repository or below it.
- Higher-level Bluetooth classes must not directly manipulate serial hardware.

## Find / detect
- Bluetooth device discovery must use BlueToothRepository.
- BluetoothFrameWriter is not required for actual detection.
- BluetoothFrameWriter may only be used to notify the phone/UI, for example "Find activated".

## Portable-Alarm-System.ino
- Keep Bluetooth menu parsing out of the .ino file.
- Use:

    BluetoothDynamicMenu::process();

- Keep unrelated GSM, alarm, sensor and device-discovery logic outside the menu class.

## Coding style
- Prefer fixed-size integer types where useful.
- Prefer const where appropriate.
- Prefer fixed-size buffers.
- Avoid heavyweight STL structures in AVR code.
- Optimize primarily for AVR Flash and SRAM usage.
- Keep code simple and explicit.
- Keep function declarations, definitions and calls on a single line whenever possible.
- When a function has multiple parameters, keep the parameters on the same line.
- Do not place each function parameter on a separate line unless a single-line declaration would become impractically long.

## Refactoring order
1. Preserve behavior.
2. Preserve Bluetooth protocol compatibility.
3. Preserve Flash and SRAM efficiency.
4. Keep code mock-test compatible.
5. Only then perform further architectural cleanup.