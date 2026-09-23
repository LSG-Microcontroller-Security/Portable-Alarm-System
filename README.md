# Portable Alarm System

Portable Alarm System is a compact, modular alarm and monitoring device designed to protect objects, rooms, vehicles and other small environments.

The base system can detect alarm conditions and notify the user by phone through a GSM module. Its design is intentionally expandable: external sensors and accessories can be connected so the same device can be adapted to different use cases.

## What it can do

The current project includes support for:

- motion and vibration/alarm detection;
- PIR and external sensor inputs;
- telephone notification through GSM;
- Bluetooth configuration from a mobile phone;
- Bluetooth-based detection of a known nearby device;
- buzzer signalling;
- temperature and supply-voltage monitoring;
- configurable operating modes stored in EEPROM.

The hardware also provides three USB connection ports that can be used to connect external modules and sensors.

The original project documentation also describes the system as a modular platform that can be extended with modules for functions such as gas or smoke detection, flame detection, fingerprint readers, NFC and other sensors.

## Bluetooth evolution

Bluetooth is used both to configure the alarm and, in some operating modes, to detect the presence of an authorised device.

The Bluetooth software is currently being reorganised into reusable components with a dynamic menu system. The goal is to keep the user interaction simple while making the Bluetooth layer easier to reuse in other embedded security projects.

## Project philosophy

The system was conceived as a small modular platform rather than as a single-purpose alarm.

The main controller is based on an AVR/Arduino-compatible microcontroller, while external modules can be added according to the required application. The firmware is designed with particular attention to limited Flash and SRAM resources.

## Documentation

[Download the Portable Alarm System leaflet (DOCX)](Documents/Portable%20volantino.docx)

## Video

Project demonstration:

https://www.youtube.com/watch?v=Jp0SQHU51y8

## Project images

[![Portable Alarm System](SitoPortable/photo%20usate/portable1.png)](SitoPortable/photo%20usate/portable1.png)

[![Portable Alarm System prototype](SitoPortable/photo%20usate/IMG_20190405_110031.jpg)](SitoPortable/photo%20usate/IMG_20190405_110031.jpg)

[![Portable Alarm System detail](SitoPortable/photo%20usate/terza.png)](SitoPortable/photo%20usate/terza.png)

## Repository contents

The repository contains the firmware, hardware design material, historical documentation, images and development notes for the Portable Alarm System.

The project is under active refactoring, especially around the Bluetooth configuration and menu architecture.
