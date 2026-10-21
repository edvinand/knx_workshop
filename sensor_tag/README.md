# KNX Tag Sensor

Tag version of the participant sensor, using the workshop's pinned KNX add-on and NCS v3.4.0. All Tag UI implementation lives in this application. The SDK and DK sensor are unchanged. The original two-channel ETS device profile is retained, but the single physical button operates channel 0 only.

## Hardware Configuration

Select `knx_workshop_tag/nrf54l15/cpuapp` and add the intended custom board root to the build's CMake options:

- Participant board: `-DBOARD_ROOT=C:/nordic/knx_workshop`, after correcting its exercise GPIOs.
- Corrected reference board: `-DBOARD_ROOT=C:/nordic/knx_workshop/solution`.

Use the matching repository path on another computer. Select only one custom board root and use a pristine or separate build directory when changing it. Do not flash the participant board's deliberately incorrect GPIO assignments.

The application uses the `sw0` button alias and `led1_red`, `led1_green`, and `led1_blue` node labels. GPIO numbers and polarity belong to the board DTS or an application overlay, not C source. `CONFIG_DK_LIBRARY=n` excludes the add-on's four-button/four-LED UI; `tag_ui.c` provides the replacement using Zephyr GPIO APIs.

## Button

Actions happen on release, after debounce:

| Gesture | Action |
| --- | --- |
| Short press, Thread not attached | Start or retry joining; an existing dataset is reused |
| Short press, attached but KNX not commissioned | Toggle KNX programming mode |
| Short press, commissioned and not in programming mode | Toggle switching channel 0 |
| Hold 3 to under 6 seconds, then release | Toggle KNX programming mode while attached |
| Hold at least 6 seconds, then release | Reset KNX configuration; retain Thread credentials |

Red blinks after the 3-second threshold and stays on after 6 seconds. Release before 3 seconds to avoid a long-press action. Programming and KNX reset gestures apply to ETS mode, not hardcoded commissioning. Switching is suppressed while programming mode is active.

Thread factory reset is not assigned to this button. Use the OpenThread RTT shell when that maintenance operation is needed; it clears saved Thread credentials.

## RGB LED

The physical LED can show combined colors when multiple channels are on:

| Channel | Indication |
| --- | --- |
| Blue | Blinking while Thread attaches, solid while attached, off while down |
| Green | Blinking in KNX programming mode, solid in KNX runtime, otherwise off |
| Red | Brief pulse on button press; long-hold feedback as described above |

This keeps status indications close to the DK sample. LED channels and button debounce are controlled in `tag_ui.c`; device state and actions are handled in `knx_sensor.c`. KNX calls run on the KNX worker thread, not in the GPIO interrupt or system workqueue.

The default leaves blue and green on during normal operation, and refreshes the UI every 100 ms. This is a workshop configuration, not an optimized battery product. An optional follow-up exercise is to turn off steady status LEDs and remove unnecessary periodic wakeups, retaining brief button feedback.

## Optional release configuration

Apply [prj_release.conf](prj_release.conf) as an extra Kconfig fragment on top of [prj.conf](prj.conf). In nRF Connect for VS Code, add it under **Extra Kconfig fragments**, or add `-DEXTRA_CONF_FILE=prj_release.conf` to the build's extra CMake arguments. Use a separate build directory or perform a pristine rebuild when changing configurations.

This profile is copied from the DK sensor's [release.conf](../sensor/release.conf). It disables logging, the shell, RTT, and console output, enables RAM power-down and device power management, disables unused SPI support, and enables link-time optimization. It does not change Thread credentials, the 500 ms poll period, LED behavior, or the periodic UI refresh, so it is a starting point rather than a fully power-optimized application.

Use it as an **extra** fragment, not as a replacement for `prj.conf`; the filename alone does not enable it in a normal build. To restore diagnostics, remove the extra fragment and rebuild. With this profile enabled, the RTT shell is unavailable, including its Thread factory-reset command.

## Commissioning and Logs

The default configuration enables the local hardcoded KNX profile and a fixed Thread dataset. Its initial Thread values match `common/overlay-thread-hardcoded.conf`; change them to exactly match the actuator project's dataset before testing the pair. The hardcoded KNX preset binds channel 0, which is the channel operated by the Tag button. Diagnostics and the shell use RTT, not VCOM/UART. Open the nRF Connect RTT terminal for the selected Tag to inspect logs.

For ETS commissioning instead, disable hardcoded KNX commissioning and the fixed Thread dataset, then restore the Joiner settings as described in Exercise 7. The DK and Tag versions use separate applications.

Like the participant DK sensor before Exercise 7, this app initially uses the sample serial `00fa10020700`. It does not yet integrate the DEVICEID credential helper. Assign a unique serial before commissioning multiple sensors together; do not use the same serial for a DK sensor and Tag in one KNX installation. Link channel 0 in ETS for the physical button test.