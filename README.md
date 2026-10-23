# Portable Alarm System

Portable Alarm System is a compact, modular alarm and monitoring device designed to protect objects, rooms, vehicles and other small environments. It detects alarm conditions and can call a configured phone number through its GSM module. Bluetooth provides local configuration and, in selected operating modes, detection of an authorised mobile phone.

The firmware is designed for an AVR/Arduino-compatible microcontroller and can work with external sensors and accessories.

> **Documentation status:** This usage guide describes the firmware examined on the `develop` branch on 23 October 2026. Some behaviour depends on the installed hardware, phone, Bluetooth module and firmware version. Verify safety-critical behaviour on the actual device before relying on the alarm.

## Quick start

1. Configure the authorised phone numbers and, if using a Find mode, the Bluetooth address and name of the selected phone. These settings are available through the Bluetooth **Configuration** menu.
2. Choose an operating mode: movement, external contact, PIR, or one of the two phone-detection (Find) modes.
3. Test the GSM call, the selected sensor and the Bluetooth search under the same power and phone conditions that will be used in practice.
4. Remember that the firmware includes an automatic alarm-arming timer of approximately **five minutes**. Access to some Bluetooth menu operations resets this timer. Do not assume that an `Alarm OFF` selection permanently disables future automatic arming.

The Bluetooth home menu reports **Alarm ON/OFF**, firmware version, temperature, an approximate battery level and `WhatzUp` (the code of the most recently recorded event). From the home menu you can open **Configuration** and **Security** or change the alarm state.

## Operating modes

| Mode | How it works | SMS command |
| --- | --- | --- |
| Movement / tilt | Detects internal movement or inclination and calls the selected phone number when armed. Bluetooth phone detection is not required. | `Md` |
| External contact, normally open | Uses the external contact input as an alarm source. | `Eo` |
| External contact, normally closed | Uses the external contact input as an alarm source. | `Ec` |
| PIR | Uses the external PIR sensor as an alarm source; an optional buzzer can sound. | `Wc` |
| **Find Mode 1** (phone-present suppression) | Searches for the authorised phone. When the phone is detected, the internal movement alarm is suppressed (and PIR calls are suppressed if PIR is independently enabled). Activating this mode via SMS resets other sensor modes, including PIR. The external-contact logic is separate. | `N1`–`N9` |
| **Find Mode 2** (phone-absence alarm) | Monitors for the authorised phone; failure to detect it can initiate a GSM call. The firmware's main loop excludes ordinary sensor alarms while this mode is active. | `F1`–`F9` |

For `N1`–`N9` and `F1`–`F9`, the digit selects **the number of phone-detection attempts per search cycle** (1 through 9). The same Bluetooth phone-detection path is used by both modes. The firmware may also make a call on activation of modes that invoke the alarm-activation routine, including `F1`–`F9`.

The code provides configuration entries for **two phone numbers** and **two sets of Bluetooth device identifiers**. The `N.Phone` setting selects the currently used phone (1 or 2); the current search logic checks the corresponding Bluetooth address and name rather than searching both phones at the same time.

### Important: mobile hotspot and Bluetooth Find reliability

> **Attention — potential interference on some phones:** With some mobile phones, enabling their **Wi-Fi hotspot/tethering** may make Bluetooth Find searches intermittent or prevent the alarm unit from detecting the authorised phone reliably. This can affect **both Find Mode 1 (`N1`–`N9`) and Find Mode 2 (`F1`–`F9`)** because both depend on detecting the phone. The observation concerns the **hotspot function**, not merely the mobile data connection or cellular network.
>
> If detection is unreliable, first **switch off the phone's hotspot** and repeat the test under otherwise identical conditions. Compare several searches with the hotspot off and on; also verify Bluetooth state, configured device name/address and stable power to the HC-05 module. The precise cause and the affected handset models have **not yet been conclusively established**, so this is a troubleshooting precaution, not a universal incompatibility.

In **Find Mode 1**, a missed detection can fail to suppress an otherwise valid movement alarm (or a separately enabled PIR alarm); in **Find Mode 2**, a missed detection can be interpreted as phone absence and initiate a call. Do not treat one successful discovery as sufficient proof of reliability.

## SMS control

The firmware accepts SMS commands from one of the two configured authorised phone numbers. Commands below are **case-sensitive** and exactly **two characters long** (except that the second character of `N1`–`N9` and `F1`–`F9` is a digit). Commands configure the **current runtime state**; do not assume they all persist across power cycles.

| Command | Operation |
| --- | --- |
| `N1`–`N9` | Activate Find Mode 1; digit = attempts per search cycle |
| `F1`–`F9` | Activate Find Mode 2; digit = attempts per search cycle |
| `Md` | Activate movement/tilt alarm without Bluetooth phone detection |
| `Eo` | Activate normally-open external-contact alarm |
| `Ec` | Activate normally-closed external-contact alarm |
| `Ex` | Disable external-contact alarm |
| `Wc` | Activate PIR alarm without Bluetooth phone detection |
| `Bz` | Enable optional buzzer (used for PIR alarm handling in the current firmware) |
| `P1`, `P2` | Select primary/alternative phone and initiate a call |
| `Ck` | Test by initiating a call to the selected number |
| `Rc` | Enable automatic answering of incoming calls |
| `Rs` | Disable automatic answering and initiate a call |
| `Dc` | Inhibit alarm calls and hang up the current call, if any |
| `Ab` | Turn on Bluetooth and open another configuration/arming-timer window |
| `Al` | Enable power-LED blinking |

These are the commands implemented by the inspected `listOfSmsCommands()` routine; this is **not** a list of every possible hardware or future firmware capability. Some mode-activation commands initiate an immediate GSM call as part of activation; perform setup with that behaviour in mind.

## Bluetooth menus

### Main menu

The main menu shows the current alarm state and offers **Alarm On / Alarm OFF**, **Configuration**, **Security**, temperature, battery level and the last event indicator (`WhatzUp`). In the current firmware the observed event letters include `M` (internal movement), `E` (external contact), `P` (PIR), `T` (high temperature), `V` (low voltage) and `X` (initial state).

### Configuration menu

- **Phone / Ph.Altern.** — main and alternative authorised telephone numbers.
- **N.Phone** — select phone 1 or phone 2 for calls and the associated Bluetooth detection entry.
- **TempMax / OffSetTemp** — temperature alarm threshold and sensor-calibration offset.
- **PIR status** — activate/deactivate the PIR input where the selected mode allows it.
- **Addr / Name / Addr2 / Name2** — configured Bluetooth identifiers for the selected phone entries (shown when a Find mode is active).
- **FindLoop / FindMode** — number of Bluetooth search attempts and operating mode (0 = no Find, 1 = Find Mode 1, 2 = Find Mode 2).
- **Ext.Int / Buzz.** — enable external interrupt and buzzer functions.

Changes made through many of these fields are saved to EEPROM and restored during firmware initialisation. By contrast, most SMS mode changes act on the current running state.

### Security menu

The security menu can change the Bluetooth **password** (after checking the existing password) and **device name**. Reconnect the phone after changing these settings, if necessary.

## Sensors, monitoring and notifications

The current firmware includes a movement/tilt interrupt, an external-contact interrupt, a PIR input, optional buzzer, an internal microcontroller temperature reading and a supply-voltage reading. The low-voltage threshold is set to approximately **3.25 V** in the examined firmware. High temperature and low voltage can trigger GSM calls independently of the ordinary motion-alarm path.

**Notifications are GSM calls**, not general-purpose outbound SMS messages. SMS is used mainly for remote command input. Make sure the SIM and GSM module can place calls and that the phone number selection is correct.

## Initial configuration and diagnostics

The repository contains a separate `Portable-Alarm-Initialized` sketch for EEPROM initialisation and Bluetooth diagnostics. Its `EEPROM_INITIALIZATION_ENABLED` option is **disabled by default** in the inspected branch. Enabling it causes EEPROM clearing and re-initialisation: back up required settings first and use the tool deliberately.

The main firmware prints `Firmware Git` and `Libraries Git` at startup, allowing hardware tests to be associated with their firmware and library revisions. The serial debug macros are controlled separately from the general Bluetooth menu and may be disabled in a normal build.

## Troubleshooting

**The phone is not reliably detected in Find mode:** Test with the phone's Wi-Fi hotspot switched off; repeat multiple times under identical conditions. Check that the saved Bluetooth address and name match the intended phone and that `N.Phone` selects the intended entry. Verify that the HC-05 is powered reliably, particularly when the alarm unit is battery-powered or uses voltage-conversion hardware. In the inspected source, Bluetooth discovery defaults to the `V2` implementation path; do not assume alternate implementation paths have been validated on your device.

**The alarm calls the wrong phone or does not call:** Check `Phone`, `Ph.Altern.`, `N.Phone`, the SIM/GSM module and any active call-inhibition command (`Dc`). `P1`, `P2` and `Ck` are useful checks, bearing in mind that they place a real call.

**Settings disappear after restarting:** Check whether the setting was made with an SMS runtime command or saved through the Bluetooth configuration menu. Inspect the EEPROM initialisation status only when necessary.

**Automatic arming behaves unexpectedly:** Be aware of the five-minute timer, including resets when the Bluetooth menus are opened. Validate the effective alarm state with a controlled test.

## Hardware and modular expansion

The project is intended as a modular platform: the hardware includes three USB connection ports for compatible external modules and sensors. The historical project material also explores possible extensions such as gas or smoke sensing, flame sensing, fingerprint readers and NFC. **These are expansion possibilities, not evidence that such modules are implemented by the current firmware.**

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

The repository contains the main firmware, a separate EEPROM/Bluetooth initialisation sketch, hardware design files, historical documentation, demonstration images and development notes. Some Bluetooth and library components are actively being refactored; this guide describes observed source behaviour rather than guaranteeing that every feature is supported on every hardware revision.