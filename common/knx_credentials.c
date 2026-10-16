/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "knx_credentials.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

struct known_device {
	const char *device_id;
	uint8_t suffix;
};

/* J-Link serials are printed on the DK; nRF DK XX is the matching event QR label. */
/* Suffixes are assigned explicitly; keep existing assignments stable. */
static const struct known_device known_devices[] = {
	{"a0200c8bf82225f3", 0x00}, /* J-Link 1057743303; nRF DK 00 */
	{"bb2057113fa05198", 0x01}, /* J-Link 1057769449; nRF DK 01 */
	{"ffc8e431f99653a4", 0x02}, /* nRF54L15 Tag 02 */
	{"f914f9c1026a51e7", 0x23}, /* J-Link 1057745708; nRF DK 23 */
	{"ef7208b9ee72b1d7", 0x24}, /* J-Link 1057799747; nRF DK 24 */
	{"f8c219a07dc2b8c6", 0x25}, /* J-Link 1057716233; nRF DK 25 */
	{"ff23a9e6854bcab9", 0x26}, /* J-Link 1057752918; nRF DK 26 */
	{"dbc4db62d889bc92", 0x27}, /* J-Link 1057789401; nRF DK 27 */
	{"92eb447a09952d2d", 0x28}, /* J-Link 1057794246; nRF DK 28 */
	{"f077c4761b12f2e2", 0x29}, /* J-Link 1057727072; nRF DK 29 */
	{"a8b9ba8a240ed3fa", 0x2a}, /* J-Link 1057733605; nRF DK 2A */
	{"4d36b2119a5b34fb", 0x2b}, /* J-Link 1057768192; nRF DK 2B */
	{"fff962c6a3a08e88", 0x2c}, /* J-Link 1057724213; nRF DK 2C */
	{"810e931751c57eee", 0x2d}, /* J-Link 1057730233; nRF DK 2D */
	{"ffc26c76ea1261a3", 0x2e}, /* J-Link 1057721134; nRF DK 2E */
	{"430dbbf9fa62039e", 0x2f}, /* J-Link 1057792667; nRF DK 2F */
	{"d1092a9ea3d0da09", 0x30}, /* J-Link 1057732989; nRF DK 30 */
	{"bf4cadb6dcdbc337", 0x31}, /* J-Link 1057770749; nRF DK 31 */
	{"b1d6c0e0115b4df6", 0x32}, /* J-Link 1057747763; nRF DK 32 */
	{"38a8be4a22659fa1", 0x33}, /* J-Link 1057773368; nRF DK 33 */
	{"d16ea3b2ea3c2570", 0x34}, /* J-Link 1057720802; nRF DK 34 */
	{"ee9350affb43f404", 0x35}, /* J-Link 1057785613; nRF DK 35 */
	{"ea32141103ad22bd", 0x36}, /* J-Link 1057789473; nRF DK 36 */
	{"0decb18b97dea8ef", 0x37}, /* J-Link 1057762537; nRF DK 37 */
	{"a8c7c1dd332646d6", 0x38}, /* J-Link 1057766988; nRF DK 38 */
	{"bf027fa025e92931", 0x39}, /* J-Link 1057714799; nRF DK 39 */
	{"23136fd72122ce80", 0x3a}, /* J-Link 1057780432; nRF DK 3A */
	{"23e96dcfefe0c71b", 0x3b}, /* J-Link 1057715536; nRF DK 3B */
	{"da76fe9bebc1b586", 0x3c}, /* J-Link 1057711508; nRF DK 3C */
	{"fccd7274e1b96320", 0x3d}, /* J-Link 1057735222; nRF DK 3D */
	{"0cfe160b51d5f539", 0x3e}, /* J-Link 1057780313; nRF DK 3E */
	{"a3294f70aff30b37", 0x3f}, /* J-Link 1057753159; nRF DK 3F */
	{"98b6f41e55d8181e", 0x40}, /* J-Link 1057730551; nRF DK 40 */
	{"ef33678aecd4df9d", 0x41}, /* J-Link 1057747289; nRF DK 41 */
	{"ea1bf4fc743eee40", 0x42}, /* J-Link 1057762487; nRF DK 42 */
	{"8f37571ef952e09d", 0x43}, /* J-Link 1057749565; nRF DK 43 */
	{"fe40d8c57af1b28b", 0x44}, /* J-Link 1057772172; nRF DK 44 */
	{"03e28b8efd909508", 0x45}, /* J-Link 1057711931; nRF DK 45 */
	{"bde61bdde6e1c442", 0x46}, /* J-Link 1057777240; nRF DK 46 */
	{"ea2a6e7146e67960", 0x47}, /* J-Link 1057786881; nRF DK 47 */
	{"ff0ef7c1d02e8e7e", 0x48}, /* J-Link 1057796655; nRF DK 48 */
	{"85698c6370004d44", 0x49}, /* J-Link 1057776028; nRF DK 49 */
	{"665509dc703570ad", 0x4a}, /* J-Link 1057708750; nRF DK 4A */
	{"9e72d8388d6dd456", 0x4b}, /* J-Link 1057753722; nRF DK 4B */
	{"8cbb18108bbf883f", 0x4c}, /* J-Link 1057730473; nRF DK 4C */
	{"8678e5db1aab060c", 0x4d}, /* J-Link 1057744827; nRF DK 4D */
	{"f86249f79a9c310b", 0x4e}, /* J-Link 1057736633; nRF DK 4E */
	{"c1b56b60c6fe3901", 0x4f}, /* J-Link 1057782949; nRF DK 4F */
};

int knx_credentials_get_serial_number(
	const uint8_t device_id[KNX_CREDENTIALS_DEVICE_ID_SIZE],
	enum knx_credentials_role role, char *serial_number, size_t serial_number_size)
{
	char device_id_hex[KNX_CREDENTIALS_DEVICE_ID_SIZE * 2U + 1U];
	char role_digit;
	const struct known_device *matched_device = NULL;

	if (device_id == NULL || serial_number == NULL) {
		return -EINVAL;
	}
	if (serial_number_size < KNX_CREDENTIALS_SERIAL_BUFFER_SIZE) {
		return -ENOSPC;
	}
	if (role == KNX_CREDENTIALS_ROLE_LSSB) {
		role_digit = '7';
	} else if (role == KNX_CREDENTIALS_ROLE_LSAB) {
		role_digit = '9';
	} else {
		return -EINVAL;
	}

	serial_number[0] = '\0';
	for (size_t byte_index = 0; byte_index < KNX_CREDENTIALS_DEVICE_ID_SIZE; byte_index++) {
		int written = snprintf(&device_id_hex[byte_index * 2U],
				       sizeof(device_id_hex) - byte_index * 2U, "%02x",
				       (unsigned int)device_id[byte_index]);

		if (written != 2) {
			return -EIO;
		}
	}

	for (size_t device_index = 0;
	     device_index < sizeof(known_devices) / sizeof(known_devices[0]); device_index++) {
		if (strcmp(device_id_hex, known_devices[device_index].device_id) == 0) {
			matched_device = &known_devices[device_index];
			break;
		}
	}
	if (matched_device == NULL) {
		return -ENOENT;
	}

	int written = snprintf(serial_number, serial_number_size, "00fa22222%c%02x", role_digit,
			matched_device->suffix);

	if (written != KNX_CREDENTIALS_SERIAL_SIZE) {
		serial_number[0] = '\0';
		return -EIO;
	}

	return 0;
}

int knx_credentials_get_thread_eui64(
	const uint8_t device_id[KNX_CREDENTIALS_DEVICE_ID_SIZE], char *eui64, size_t eui64_size)
{
	if (device_id == NULL || eui64 == NULL) {
		return -EINVAL;
	}
	if (eui64_size < KNX_CREDENTIALS_EUI64_BUFFER_SIZE) {
		return -ENOSPC;
	}

	int written = snprintf(eui64, eui64_size, "f4ce36%02x%02x%02x%02x%02x",
			device_id[3], device_id[2], device_id[1], device_id[0], device_id[7]);

	if (written != KNX_CREDENTIALS_EUI64_BUFFER_SIZE - 1U) {
		eui64[0] = '\0';
		return -EIO;
	}

	return 0;
}