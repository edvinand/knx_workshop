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

/* Suffixes follow images/IDs.txt order. Keep existing assignments stable. */
static const struct known_device known_devices[] = {
	{"a0200c8bf82225f3", 0x00}, /* J-Link 1057743303 */
	{"bb2057113fa05198", 0x01}, /* J-Link 1057769449 */
	{"ffc8e431f99653a4", 0x02}, /* Tag */
	{"f914f9c1026a51e7", 0x03}, /* J-Link 1057745708 */
	{"ef7208b9ee72b1d7", 0x04}, /* J-Link 1057799747 */
	{"f8c219a07dc2b8c6", 0x05}, /* J-Link 1057716233 */
	{"ff23a9e6854bcab9", 0x06}, /* J-Link 1057752918 */
	{"dbc4db62d889bc92", 0x07}, /* J-Link 1057789401 */
	{"92eb447a09952d2d", 0x08}, /* J-Link 1057794246 */
	{"f077c4761b12f2e2", 0x09}, /* J-Link 1057727072 */
	{"a8b9ba8a240ed3fa", 0x0a}, /* J-Link 1057733605 */
	{"4d36b2119a5b34fb", 0x0b}, /* J-Link 1057768192 */
	{"fff962c6a3a08e88", 0x0c}, /* J-Link 1057724213 */
	{"810e931751c57eee", 0x0d}, /* J-Link 1057730233 */
	{"ffc26c76ea1261a3", 0x0e}, /* J-Link 1057721134 */
	{"430dbbf9fa62039e", 0x0f}, /* J-Link 1057792667 */
	{"d1092a9ea3d0da09", 0x10}, /* J-Link 1057732989 */
	{"bf4cadb6dcdbc337", 0x11}, /* J-Link 1057770749 */
	{"b1d6c0e0115b4df6", 0x12}, /* J-Link 1057747763 */
	{"38a8be4a22659fa1", 0x13}, /* J-Link 1057773368 */
	{"d16ea3b2ea3c2570", 0x14}, /* J-Link 1057720802 */
	{"ee9350affb43f404", 0x15}, /* J-Link 1057785613 */
	{"ea32141103ad22bd", 0x16}, /* J-Link 1057789473 */
	{"0decb18b97dea8ef", 0x17}, /* J-Link 1057762537 */
	{"a8c7c1dd332646d6", 0x18}, /* J-Link 1057766988 */
	{"bf027fa025e92931", 0x19}, /* J-Link 1057714799 */
	{"23136fd72122ce80", 0x1a}, /* J-Link 1057780432 */
	{"23e96dcfefe0c71b", 0x1b}, /* J-Link 1057715536 */
	{"da76fe9bebc1b586", 0x1c}, /* J-Link 1057711508 */
	{"fccd7274e1b96320", 0x1d}, /* J-Link 1057735222 */
	{"0cfe160b51d5f539", 0x1e}, /* J-Link 1057780313 */
	{"a3294f70aff30b37", 0x1f}, /* J-Link 1057753159 */
	{"98b6f41e55d8181e", 0x20}, /* J-Link 1057730551 */
	{"ef33678aecd4df9d", 0x21}, /* J-Link 1057747289 */
	{"ea1bf4fc743eee40", 0x22}, /* J-Link 1057762487 */
	{"8f37571ef952e09d", 0x23}, /* J-Link 1057749565 */
	{"fe40d8c57af1b28b", 0x24}, /* J-Link 1057772172 */
	{"03e28b8efd909508", 0x25}, /* J-Link 1057711931 */
	{"bde61bdde6e1c442", 0x26}, /* J-Link 1057777240 */
	{"ea2a6e7146e67960", 0x27}, /* J-Link 1057786881 */
	{"ff0ef7c1d02e8e7e", 0x28}, /* J-Link 1057796655 */
	{"85698c6370004d44", 0x29}, /* J-Link 1057776028 */
	{"665509dc703570ad", 0x2a}, /* J-Link 1057708750 */
	{"9e72d8388d6dd456", 0x2b}, /* J-Link 1057753722 */
	{"8cbb18108bbf883f", 0x2c}, /* J-Link 1057730473 */
	{"8678e5db1aab060c", 0x2d}, /* J-Link 1057744827 */
	{"f86249f79a9c310b", 0x2e}, /* J-Link 1057736633 */
	{"c1b56b60c6fe3901", 0x2f}, /* J-Link 1057782949 */
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