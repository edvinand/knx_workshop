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

![Selecting the top-level actuator folder using Open an existing application](images/add_existing_app.png)

## Exercise 4: Build the actuator and sensor samples

First, build both participant applications without flashing or starting them. This confirms the initial configurations build before you set up a private Thread network in Exercise 5.

1. In the nRF Connect view, under `actuator`, click **Add build configuration**. Set:
	- **SDK:** nRF Connect SDK v3.4.0
	- **Toolchain:** nRF Connect SDK Toolchain v3.4.0
	- **Board target:** `nrf54l15dk/nrf54l15/cpuapp`

	Leave all other options at their defaults. Scroll to the bottom and click **Generate and Build**.

	![Adding a build configuration for the actuator application](images/add_build_configuration.png)

2. Repeat for `sensor` with the same SDK, toolchain, board target, and default options. Click **Generate and Build** at the bottom.
3. Confirm that both builds complete successfully.

## Exercise 5: Set up your private Thread network

The hardcoded overlay files contain the same starter Thread dataset for everyone. If you leave it unchanged, multiple groups can end up using the same network settings. Choose a channel in the valid range and set a unique PAN ID and network key for your group. The actuator and sensor in your pair must use the same channel, PAN ID, and key. The network key is what prevents other groups from joining your network; a different channel or PAN ID alone does not provide privacy.

1. In the top-level `actuator/prj.conf`, append the complete contents of `common/overlay-knx-hardcoded.conf` and `common/overlay-thread-hardcoded.conf`.
2. Repeat in the top-level `sensor/prj.conf`, using the same two files. Do not edit the files under `solution/` or change the common overlay templates.
3. In both `prj.conf` files, customize the Thread settings copied from `overlay-thread-hardcoded.conf`:
	- Set `CONFIG_OPENTHREAD_CHANNEL` to a channel from **11 through 26**, inclusive. Use the same channel in both applications.
	- Change `CONFIG_OPENTHREAD_PANID` from the template value `51966` to a PAN ID unique to your group, and use it in both applications.
	- Replace `CONFIG_OPENTHREAD_NETWORKKEY` with a unique 16-byte key. Keep the template's colon-separated hexadecimal format and use exactly the same key in both applications. Do not share your key with other groups.
4. Rebuild both applications using their existing build configurations and confirm that both builds succeed before proceeding to any network test.
