# knx-hands-on
Repository for KNX hands on guide

## Exercise 1: Include PR #6

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
