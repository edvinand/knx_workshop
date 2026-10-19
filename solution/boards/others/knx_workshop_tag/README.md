# KNX Workshop Tag Solution

Corrected board definition for the physical nRF54L15 Tag, based on NCS v3.4.0. The board target remains `knx_workshop_tag/nrf54l15/cpuapp` so it can replace the participant starter by changing `BOARD_ROOT`.

## Corrected GPIOs

| Function | GPIO | Configuration |
| --- | --- | --- |
| Button | P0.00 | Active-low, internal pull-up |
| Red LED | P2.08 | Active-low |
| Green LED | P2.10 | Active-low |
| Blue LED | P2.09 | Active-low |

Compared with the participant starter, the board DTS changes only these four GPIO properties and the model description. All three LED channels require both the correct pin and active-low polarity.

The BME688, ADXL367, BMI270, and their unused I2C/SPI buses remain disabled. This prevents normal driver initialization; it does not guarantee that the physical sensors are powered off. The Tag's clock, radio, antenna-switch, and memory configuration is retained.

## Selecting the Solution

Use the solution directory as the board root: `-DBOARD_ROOT=C:/nordic/knx_workshop/solution`, with board target `knx_workshop_tag/nrf54l15/cpuapp`. Use the equivalent repository path on another computer. Keep NCS and its toolchain at v3.4.0.

The starter and solution intentionally define the same board name. Select only the intended custom board root, not both the workshop root and the solution root. Use a pristine build or a separate build directory when switching between them.

Use the dedicated [Tag sensor application](../../../../sensor_tag/README.md) with this corrected board root for the one-button/RGB UI and RTT diagnostics. This directory contains the board solution only; the existing DK-oriented sensor application remains unchanged.