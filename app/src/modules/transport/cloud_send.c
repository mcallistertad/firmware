/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <errno.h>

#include "cloud_send.h"
#include "cloud_send_backend.h"

struct confirmed_send_result {
	int result_code;
};

static void confirmed_send_callback(int16_t result_code, size_t offset, const uint8_t *response,
				    size_t len, bool last_block, void *user_data)
{
	struct confirmed_send_result *result = user_data;

	(void)offset;
	(void)response;
	(void)len;
	(void)last_block;

	result->result_code = result_code;
}

int transport_cloud_bytes_send(uint8_t *buffer, size_t len, bool confirmable)
{
	struct confirmed_send_result result = {
		.result_code = -EIO,
	};
	int err;

	if (!confirmable) {
		return transport_cloud_backend_bytes_send(buffer, len);
	}

	if (!transport_cloud_backend_is_connected()) {
		return -EACCES;
	}

	/* NCS 2.9 waits for a confirmable response but does not propagate the callback
	 * result. Capture it directly so a timeout or cloud rejection is not reported
	 * as successful delivery.
	 */
	err = transport_cloud_backend_confirmable_post(buffer, len, confirmed_send_callback,
						       &result);
	if (err) {
		return err;
	}

	if (result.result_code == TRANSPORT_COAP_RESPONSE_UNAUTHORIZED) {
		return -EACCES;
	}

	if (result.result_code < 0) {
		return result.result_code;
	}

	if (result.result_code >= TRANSPORT_COAP_RESPONSE_OK &&
	    result.result_code <= TRANSPORT_COAP_RESPONSE_CONTINUE) {
		return 0;
	}

	return result.result_code == 0 ? -EPROTO : result.result_code;
}
