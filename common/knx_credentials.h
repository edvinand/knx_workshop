/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef KNX_CREDENTIALS_H_
#define KNX_CREDENTIALS_H_

#include <stddef.h>
#include <stdint.h>

#define KNX_CREDENTIALS_DEVICE_ID_SIZE 8U
#define KNX_CREDENTIALS_SERIAL_SIZE 12U
#define KNX_CREDENTIALS_SERIAL_BUFFER_SIZE (KNX_CREDENTIALS_SERIAL_SIZE + 1U)

enum knx_credentials_role {
	KNX_CREDENTIALS_ROLE_LSSB = 7,
	KNX_CREDENTIALS_ROLE_LSAB = 9,
};

/* device_id uses the byte order returned by hwinfo_get_device_id(). */
int knx_credentials_get_serial_number(
	const uint8_t device_id[KNX_CREDENTIALS_DEVICE_ID_SIZE],
	enum knx_credentials_role role, char *serial_number, size_t serial_number_size);

#endif /* KNX_CREDENTIALS_H_ */