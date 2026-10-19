# KNX Workshop Tag

Participant starter board for the physical nRF54L15 Tag, based on the NCS v3.4.0 Tag definition. The supplied application-core target is `knx_workshop_tag/nrf54l15/cpuapp`. No non-secure or FLPR board files are supplied.

**Do not flash this starter unchanged.** The button and RGB LED GPIO assignments are deliberately incorrect. Participants must correct the pins and LED polarity to match the Tag before flashing.

## Exercise Starting Point

The devicetree keeps one button and three RGB LED channels. The button uses the DK's button 0 assignment. Red, green, and blue use the DK's LED 0, LED 2, and LED 3 assignments respectively. These avoid the Tag's antenna-control GPIOs. They are exercise placeholders, not Tag wiring.

Locate the `gpios` properties under `buttons` and `leds` in the board DTS. Use the Tag schematic to correct them, either directly in the board DTS or through an application overlay. An overlay should target the existing node labels rather than add duplicate buttons or LEDs. A corrected reference is available under `solution/boards/others/knx_workshop_tag`.

The BME688, ADXL367, BMI270, and their unused I2C/SPI buses are explicitly disabled. This prevents normal driver initialization; it does not guarantee that the physical sensors are powered off. The Tag's clock, radio, antenna-switch, and memory configuration is retained. No UART console is configured.

## Board Files

- `board.yml`: board identity and SoC.
- `Kconfig.knx_workshop_tag`: application-core SoC selection.
- `Kconfig.defconfig`: board-level defaults.
- `knx_workshop_tag_nrf54l15_cpuapp_defconfig`: GPIO and MPU defaults.
- `knx_workshop_tag_nrf54l15_cpuapp.dts`: hardware description and exercise GPIO assignments.
- `board.cmake`: nrfutil and J-Link runner integration.

## Discovery

Add the workshop repository root, not this board directory, to the build's board roots. For this checkout, set the CMake option `-DBOARD_ROOT=C:/nordic/knx_workshop` and select `knx_workshop_tag/nrf54l15/cpuapp`. Use the equivalent repository path on another computer. Keep NCS and its toolchain at v3.4.0.

Use the dedicated [Tag sensor application](../../../sensor_tag/README.md) after correcting the board GPIOs. It provides the one-button/RGB UI and RTT diagnostics without the add-on's DK UI. The existing DK sensor application remains unchanged.