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

The KNX IoT applications use the `sdk-knx-iot-stack` west project. Pull request [#6](https://github.com/nrfconnect/sdk-knx-iot-point-api-stack/pull/6) fixes how the stack handles Echo options in CoAP messages, which can prevent a KNX IoT Router from synchronizing with a device. Include the pull request in your local NCS workspace before continuing with the applications.

### Steps

1. Open the NCS workspace root, the directory containing `.west`. In the workshop setup, this is `C:\nordic\SDKs\ncs\v3.4.0`.
2. Check that the stack checkout has no local changes. From the NCS workspace root, run:

	```powershell
	git -C modules/lib/sdk-knx-iot-stack status --short
	```

	If this prints any paths, stop and preserve those changes before updating the project.

3. Open `ncs-knx-iot/west.yml`. In the `sdk-knx-iot-stack` project entry, change its revision from `main` to `pull/6/head`:

	```yaml
	- name: sdk-knx-iot-stack
	  revision: pull/6/head
	  remote: ncs
	  repo-path: sdk-knx-iot-point-api-stack
	  path: modules/lib/sdk-knx-iot-stack
	```

	This selects the pull request's head from the `nrfconnect/sdk-knx-iot-point-api-stack` repository; it does not merge the pull request.

4. Save the manifest. In a toolchain-enabled terminal, run the following from the NCS workspace root:

	```powershell
	nrfutil sdk-manager toolchain launch --ncs-version v3.4.0 -- west update sdk-knx-iot-stack
	```

5. Verify the stack checkout is at the pull request commit:

	```powershell
	git -C modules/lib/sdk-knx-iot-stack log -1 --oneline
	```

	The PR head at the time this guide was written is `15eb575` (`Stop dropping messages with outer echo challenges`).

This updates the stack in your local NCS workspace. It does not change the participant `actuator` or `sensor` applications, or write anything to GitHub.
