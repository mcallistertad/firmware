/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <net/nrf_cloud_coap.h>
#include <nrf_cloud_coap_transport.h>
#include <zephyr/net/coap.h>
#include <zephyr/sys/util.h>

#include "cloud_send_backend.h"

#define NRF_CLOUD_COAP_D2C_RAW_RESOURCE "msg/d2c/raw"

BUILD_ASSERT(TRANSPORT_COAP_RESPONSE_OK == COAP_RESPONSE_CODE_OK);
BUILD_ASSERT(TRANSPORT_COAP_RESPONSE_CONTINUE == COAP_RESPONSE_CODE_CONTINUE);
BUILD_ASSERT(TRANSPORT_COAP_RESPONSE_BAD_REQUEST == COAP_RESPONSE_CODE_BAD_REQUEST);
BUILD_ASSERT(TRANSPORT_COAP_RESPONSE_UNAUTHORIZED == COAP_RESPONSE_CODE_UNAUTHORIZED);

bool transport_cloud_backend_is_connected(void)
{
	return nrf_cloud_coap_is_connected();
}

int transport_cloud_backend_bytes_send(uint8_t *buffer, size_t len)
{
	return nrf_cloud_coap_bytes_send(buffer, len, false);
}

int transport_cloud_backend_confirmable_post(uint8_t *buffer, size_t len,
					     transport_cloud_response_cb_t callback,
					     void *user_data)
{
	return nrf_cloud_coap_post(NRF_CLOUD_COAP_D2C_RAW_RESOURCE, NULL, buffer, len,
				   COAP_CONTENT_FORMAT_APP_OCTET_STREAM, true, callback, user_data);
}
