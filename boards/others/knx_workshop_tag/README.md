# KNX Workshop Tag

Participant starter board for the physical nRF54L15 Tag, based on the NCS v3.4.0 Tag definition. The supplied application-core target is `knx_workshop_tag/nrf54l15/cpuapp`. No non-secure or FLPR board files are supplied.

**Do not flash this starter unchanged.** The button and RGB LED GPIO assignments are deliberately incorrect. Participants must correct the pins and LED polarity to match the Tag before flashing.

## Exercise Starting Point

The devicetree keeps one button and three RGB LED channels. The button uses the DK's button 0 assignment. Red, green, and blue use the DK's LED 0, LED 2, and LED 3 assignments respectively. These avoid the Tag's antenna-control GPIOs. They are exercise placeholders, not Tag wiring.

Locate the `gpios` properties under `buttons` and `leds` in the board DTS. Use the Tag schematic to correct them, either directly in the board DTS or through an application overlay. An overlay should target the existing node labels rather than add duplicate buttons or LEDs. A corrected reference is available under `solution/boards/others/knx_workshop_tag`.

For [Exercise 8](../../../README.md#correct-the-board-gpios), leave this DTS unchanged and copy its LED/button sections into the application's `boards/knx_workshop_tag_nrf54l15_cpuapp.overlay`, preserving their node paths and labels. Correct the GPIOs in that overlay instead. This filename matches the custom board target and is discovered automatically; no `EXTRA_DTC_OVERLAY_FILE` argument is needed.

The BME688, ADXL367, BMI270, and their unused I2C/SPI buses are explicitly disabled. This prevents normal driver initialization; it does not guarantee that the physical sensors are powered off. The Tag's clock, radio, antenna-switch, and memory configuration is retained. No UART console is configured.

## Board Files

- `board.yml`: board identity and SoC.
- `Kconfig.knx_workshop_tag`: application-core SoC selection.
- `Kconfig.defconfig`: board-level defaults.
- `knx_workshop_tag_nrf54l15_cpuapp_defconfig`: GPIO and MPU defaults.
- `knx_workshop_tag_nrf54l15_cpuapp.dts`: hardware description and exercise GPIO assignments.
- `board.cmake`: nrfutil and J-Link runner integration.

Use the dedicated [Tag sensor application](../../../sensor_tag/README.md) after correcting the board GPIOs. It provides the one-button/RGB UI and RTT diagnostics without the add-on's DK UI. The existing DK sensor application remains unchanged.

## Create a Build Configuration

1. Add the workshop's `sensor_tag` folder as an existing application in the nRF Connect view.
2. Make the board discoverable **before selecting a board**. In VS Code workspace settings, find **nRF Connect: Board Roots** (`nrf-connect.boardRoots`) and add your absolute workshop repository path, for example `C:/nordic/knx_workshop`. Adding that repository root as a workspace folder is an alternative, since the extension also scans workspace folders. The root must contain `boards/`; do not enter the board directory itself.
3. Under `sensor_tag`, select **Add Build Configuration**. Choose NCS **v3.4.0** and its matching toolchain.
4. In the board picker's **Custom** list, select **`knx_workshop_tag/nrf54l15/cpuapp`**. If board and qualifiers are separate fields, select board `knx_workshop_tag` and qualifiers `nrf54l15/cpuapp`. Do not select the DK or the stock `nrf54l15tag` board for this exercise.
5. Use a separate build directory from any existing stock-Tag or DK build, for example `sensor_tag/_build_tag`. Do not reuse another board's cached build directory.
6. In **Extra CMake arguments**, ensure the board root points to your repository:

	 ```text
	 -DBOARD_ROOT=C:/nordic/knx_workshop
	 ```

7. Create and build the configuration. Before flashing, check that the configuration log identifies board `knx_workshop_tag` with qualifiers `nrf54l15/cpuapp`, and that the board DTS path points into this repository rather than the SDK's stock Tag directory. Open RTT for application logs; this custom board has no UART console.

Replace the example checkout path with your own. If the board is missing from the picker, check the workspace Board Roots setting and reopen **Add Build Configuration**. A CMake argument alone does not populate the extension's board picker.

### Starter and Solution Roots

Both copies have the same board name, so expose only the intended copy as a board root for a configuration:

| Application | Board root | Board target |
| --- | --- | --- |
| `sensor_tag` | `C:/nordic/knx_workshop` | `knx_workshop_tag/nrf54l15/cpuapp` |
| `solution/sensor_tag` | `C:/nordic/knx_workshop/solution` | `knx_workshop_tag/nrf54l15/cpuapp` |

For the solution, use the solution root in Board Roots and `-DBOARD_ROOT=C:/nordic/knx_workshop/solution`. Avoid registering both copies for the same build: they define the same target, and the wrong GPIO definition could be selected. The solution already contains the corrected GPIO assignments.

## Board Target Naming

NCS v3.4.0 uses Zephyr's hardware model v2. The general target format is:

```text
<board_name>[@revision][/<soc>[/<cpu_cluster>][/<variant>]]
```

Square brackets mark optional parts and are not typed into the target. Only use qualifiers actually supplied by the board. For this board:

| Part | Value | Meaning |
| --- | --- | --- |
| Board name | `knx_workshop_tag` | Unique identity in `board.yml`; conventionally lowercase with underscores. |
| Revision | Not specified | A supported hardware revision would follow `@`. |
| SoC | `nrf54l15` | The chip fitted to the Tag. |
| CPU cluster | `cpuapp` | The application-core target, inherited from the SoC definition. |
| Variant | Not specified | Some boards provide variants such as `ns`; this custom board does not. |

The full target is therefore **`knx_workshop_tag/nrf54l15/cpuapp`**. `KNX Workshop Tag`, the `full_name` in `board.yml`, is a display name, not a build target. The stock Tag target `nrf54l15tag/nrf54l15/cpuapp` names the same SoC and CPU cluster but selects a different board definition.

Qualified filenames replace the target's slashes with underscores: [knx_workshop_tag_nrf54l15_cpuapp.dts](knx_workshop_tag_nrf54l15_cpuapp.dts) and [knx_workshop_tag_nrf54l15_cpuapp_defconfig](knx_workshop_tag_nrf54l15_cpuapp_defconfig). An application-specific overlay for this custom target can similarly be named `boards/knx_workshop_tag_nrf54l15_cpuapp.overlay`. The existing stock-Tag overlay is named for a different board and does not automatically apply to this target.

## How This Board Was Created

The starting point was the NCS v3.4.0 board directory `zephyr/boards/nordic/nrf54l15tag/`. The workshop adapts that definition outside the SDK, under `boards/others/knx_workshop_tag/`. It reuses the existing nRF54L15 SoC support; it is not a new SoC port. The SDK files were not modified.

### Source Files and Renames

This was not a verbatim copy of one DTS file. The stock board splits its hardware description across three files, which were combined into the workshop DTS:

| Stock file in `zephyr/boards/nordic/nrf54l15tag/` | Workshop destination or adaptation |
| --- | --- |
| `nrf54l15tag_nrf54l15_cpuapp.dts` | Base for [knx_workshop_tag_nrf54l15_cpuapp.dts](knx_workshop_tag_nrf54l15_cpuapp.dts): SoC/partition includes, model, chosen memory and NFC-pin configuration. |
| `nrf54l15tag_cpuapp_common.dtsi` | Merge its application-core clock, radio, regulator, boot-mode and antenna configuration into that DTS. |
| `nrf54l15tag_common.dtsi` | Merge its GPIO, button, RGB LED, sensor and bus definitions into that DTS, retaining only the UI nodes used here. |
| `board.yml` | Adapt [board.yml](board.yml) to the new board identity and only the supported application-core target. |
| `Kconfig.nrf54l15tag` | Rename/adapt as [Kconfig.knx_workshop_tag](Kconfig.knx_workshop_tag), selecting only the application-core SoC. |
| `nrf54l15tag_nrf54l15_cpuapp_defconfig` | Copy its GPIO/MPU defaults into [knx_workshop_tag_nrf54l15_cpuapp_defconfig](knx_workshop_tag_nrf54l15_cpuapp_defconfig). |
| `Kconfig.defconfig` | Simplify [Kconfig.defconfig](Kconfig.defconfig) to the stack-protection default, guarded by the new board symbol. |
| `board.cmake` | Simplify [board.cmake](board.cmake) to the application-core J-Link device and the nrfutil/J-Link runner includes. |

The merged DTS keeps the SDK includes `<arm/nordic/nrf54l15_cpuapp.dtsi>` and `<vendor/nordic/nrf54l15_cpuapp_partition.dtsi>`. It no longer includes the two board-local `nrf54l15tag_*common.dtsi` files because their relevant content is now in the custom DTS. Thus the workshop board does not need private copies of those `.dtsi` files.

### Names Changed

In `board.yml`, change the board identity, not the SoC identity:

```yaml
board:
  name: knx_workshop_tag
  full_name: KNX Workshop Tag
  vendor: others
  socs:
    - name: nrf54l15
```

The directory uses `others` to match this vendor grouping. It is not part of the build target. The stock board's `ns` and FLPR/XIP variants and multi-target runner coordination are not carried over; no files for those targets are supplied here.

In the renamed `Kconfig.knx_workshop_tag`, change the board symbols from `BOARD_NRF54L15TAG` and `BOARD_NRF54L15TAG_NRF54L15_CPUAPP` to:

```kconfig
config BOARD_KNX_WORKSHOP_TAG
	select SOC_NRF54L15_CPUAPP if BOARD_KNX_WORKSHOP_TAG_NRF54L15_CPUAPP
```

`SOC_NRF54L15_CPUAPP` remains unchanged because the hardware is still an nRF54L15 application core. The board identity and qualifiers generate the `BOARD_KNX_WORKSHOP_TAG_NRF54L15_CPUAPP` target symbol; it does not need a separate manual definition. `Kconfig.defconfig` uses `if BOARD_KNX_WORKSHOP_TAG` to scope its defaults to this board.

The DTS `model` becomes `KNX Workshop Tag - GPIO exercise starter` (or `... solution` in the reference copy). The `compatible = "nordic,nrf54l15tag_nrf54l15_cpuapp"` string is intentionally retained because this is still the same physical Nordic Tag. Do not blindly replace every occurrence of `nrf54l15tag` or `nrf54l15`.

### Hardware and Runner Changes

Keep the clock, radio, memory, antenna switch and GPIO-controller setup, together with the `button0`, `led1_red`, `led1_green`, `led1_blue`, `sw0` and `led0` names used by the application. Omit the unused second button/RGB group and keep the external BME688, ADXL367, BMI270 and their I2C/SPI buses disabled. The starter intentionally replaces the first button/RGB GPIOs with incorrect DK-style values; correcting those is the participant exercise. The [solution DTS](../../../solution/boards/others/knx_workshop_tag/knx_workshop_tag_nrf54l15_cpuapp.dts) contains the actual Tag pin assignments.

The target defconfig retains `CONFIG_GPIO=y` and `CONFIG_ARM_MPU=y`. KNX, Thread and RTT application settings remain in the application configuration rather than being tied to the board identity.

The runner keeps J-Link device `nRF54L15_M33` at speed `4000`, plus the SDK's `nrfutil.board.cmake` and `jlink.board.cmake` includes. The FLPR runner branch and TF-M merged-image handling are omitted because those targets are not supplied. Finally, registering the repository as a board root makes this new identity available without editing the SDK or creating a new SoC definition.

For background, see Nordic's [custom board lesson](https://academy.nordicsemi.com/courses/nrf-connect-sdk-intermediate/lessons/lesson-3-adding-custom-board-support/topic/creating-board-files/) and Zephyr's [board porting guide](https://nrfconnectdocs.nordicsemi.com/ncs/latest/zephyr/hardware/porting/board_porting.html). These general references may track newer releases; this workshop uses the supplied board files with NCS v3.4.0.