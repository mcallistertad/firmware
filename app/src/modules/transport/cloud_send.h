/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef CLOUD_SEND_H__
#define CLOUD_SEND_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

int transport_cloud_bytes_send(uint8_t *buffer, size_t len, bool confirmable);

#endif /* CLOUD_SEND_H__ */
