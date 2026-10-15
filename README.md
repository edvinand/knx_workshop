# knx-hands-on

## Prerequisites

Before starting the exercises, make sure you have:

- [Visual Studio Code](https://code.visualstudio.com/download) installed.
- The [nRF Connect for VS Code extension pack](https://marketplace.visualstudio.com/items?itemName=nordic-semiconductor.nrf-connect-extension-pack) installed.
- Standalone [`nrfutil`](https://www.nordicsemi.com/Products/Development-tools/nRF-Util/Download) installed, with its location added to your computer's `PATH`. Verify it by running `nrfutil --version` from any directory.

## Exercise 1: Install the KNX IoT add-on

The KNX IoT add-on requires nRF Connect SDK v3.4.0 and its toolchain. See the [software requirements](https://nrfconnectdocs.nordicsemi.com/addons/ncs-knx-iot/latest/setup.html#software-requirements) before starting.

To add the add-on to an existing NCS workspace using the CLI:

1. Open a terminal at the NCS workspace root, the directory containing `.west`.
2. If the `ncs-knx-iot` directory is not already there, clone the add-on repository:

	```powershell
	git clone https://github.com/nrfconnect/ncs-knx-iot
	```

3. Launch a terminal with the NCS v3.4.0 toolchain:

	```powershell
	nrfutil sdk-manager toolchain launch --ncs-version v3.4.0 --terminal
	```

	In the terminal that opens, run these commands:

	```powershell
	west config manifest.path ncs-knx-iot
	west update
	```

## Exercise 2: Include PR #6

We found compatibility issues with some KNX routers in Zephyr, so we need to apply this patch: [PR #6](https://github.com/nrfconnect/sdk-knx-iot-point-api-stack/pull/6).

From the NCS workspace root (folder called v3.4.0), check that the stack has no local changes:

```powershell
git -C modules/lib/sdk-knx-iot-stack status --short
```

If it prints any paths, stop and preserve those changes before continuing. If the output is empty, fetch and check out the PR:

```powershell
git -C modules/lib/sdk-knx-iot-stack fetch https://github.com/nrfconnect/sdk-knx-iot-point-api-stack.git pull/6/head
git -C modules/lib/sdk-knx-iot-stack checkout --detach FETCH_HEAD
```

This checks out the PR in your local workspace. Running `west update` later may replace it with the revision specified by the manifest.

## Exercise 3: Add the applications to nRF Connect for VS Code

Add the participant applications to the nRF Connect view one at a time. Do not add the applications under `solution/`.

1. In the nRF Connect Welcome view, select **Open an existing application**.
2. Browse to the workshop repository, select its top-level `actuator` folder, and click **Open**.
3. When prompted, select **Add**.
4. Repeat for the top-level `sensor` folder.

![Selecting the top-level actuator folder using Open an existing application](images/Screenshot%202026-10-16%20012208.png)

## Exercise 4: Build the actuator and sensor samples

We want to test the actuator (acting as a light bulb) and the sensor (acting as the light switch) locally, without having to add them to an actual KNX network.
Use the workshop's hardcoded KNX and Thread settings for these initial builds. Make these changes only to the participant applications at the top level of the workshop repository; leave the applications under `solution/` unchanged.

1. Open `actuator/prj.conf`. Copy the complete contents of `common/overlay-knx-hardcoded.conf` and `common/overlay-thread-hardcoded.conf` to the bottom of the file.
2. Repeat for `sensor/prj.conf`, appending the contents of those same two files.
3. In nRF Connect for VS Code, create a build configuration for each application. Use the same board target for both, matching the development kit. Both samples support `nrf54l15dk/nrf54l15/cpuapp`, `nrf54lm20dk/nrf54lm20a/cpuapp`, and `nrf54lm20dk/nrf54lm20b/cpuapp`.
4. Build both applications and confirm that each build completes successfully.
