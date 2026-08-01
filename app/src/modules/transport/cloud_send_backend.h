/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef CLOUD_SEND_BACKEND_H__
#define CLOUD_SEND_BACKEND_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TRANSPORT_COAP_RESPONSE_OK 64
#define TRANSPORT_COAP_RESPONSE_CONTINUE 95
#define TRANSPORT_COAP_RESPONSE_BAD_REQUEST 128
#define TRANSPORT_COAP_RESPONSE_UNAUTHORIZED 129

typedef void (*transport_cloud_response_cb_t)(int16_t result_code, size_t offset,
					      const uint8_t *response, size_t len,
					      bool last_block, void *user_data);

bool transport_cloud_backend_is_connected(void);
int transport_cloud_backend_bytes_send(uint8_t *buffer, size_t len);
int transport_cloud_backend_confirmable_post(uint8_t *buffer, size_t len,
					     transport_cloud_response_cb_t callback,
					     void *user_data);

#endif /* CLOUD_SEND_BACKEND_H__ */
