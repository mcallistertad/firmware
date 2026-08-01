/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <errno.h>

#include <unity.h>
#include <zephyr/fff.h>
#include <zephyr/sys/util.h>

#include "cloud_send.h"
#include "cloud_send_backend.h"

DEFINE_FFF_GLOBALS;

#define COAP_RESPONSE_CHANGED 68

FAKE_VALUE_FUNC(bool, transport_cloud_backend_is_connected);
FAKE_VALUE_FUNC(int, transport_cloud_backend_bytes_send, uint8_t *, size_t);
FAKE_VALUE_FUNC(int, transport_cloud_backend_confirmable_post, uint8_t *, size_t,
		transport_cloud_response_cb_t, void *);

static int16_t callback_result;

static int confirmable_post_fake(uint8_t *buffer, size_t len,
				 transport_cloud_response_cb_t callback, void *user_data)
{
	ARG_UNUSED(buffer);
	ARG_UNUSED(len);

	if (transport_cloud_backend_confirmable_post_fake.return_val == 0 && callback != NULL) {
		callback(callback_result, 0, NULL, 0, true, user_data);
	}

	return transport_cloud_backend_confirmable_post_fake.return_val;
}

void setUp(void)
{
	RESET_FAKE(transport_cloud_backend_is_connected);
	RESET_FAKE(transport_cloud_backend_bytes_send);
	RESET_FAKE(transport_cloud_backend_confirmable_post);

	transport_cloud_backend_is_connected_fake.return_val = true;
	transport_cloud_backend_confirmable_post_fake.custom_fake = confirmable_post_fake;
	callback_result = COAP_RESPONSE_CHANGED;
}

void test_non_confirmable_delegates_to_bytes_send(void)
{
	uint8_t payload[] = "payload";

	TEST_ASSERT_EQUAL(0, transport_cloud_bytes_send(payload, sizeof(payload), false));
	TEST_ASSERT_EQUAL(1, transport_cloud_backend_bytes_send_fake.call_count);
	TEST_ASSERT_EQUAL(0, transport_cloud_backend_confirmable_post_fake.call_count);
}

void test_confirmable_success_uses_callback_result(void)
{
	uint8_t payload[] = "payload";

	TEST_ASSERT_EQUAL(0, transport_cloud_bytes_send(payload, sizeof(payload), true));
	TEST_ASSERT_EQUAL(1, transport_cloud_backend_confirmable_post_fake.call_count);
}

void test_confirmable_timeout_is_returned(void)
{
	uint8_t payload[] = "payload";

	callback_result = -ETIMEDOUT;
	TEST_ASSERT_EQUAL(-ETIMEDOUT,
			  transport_cloud_bytes_send(payload, sizeof(payload), true));
}

void test_cloud_rejection_is_returned(void)
{
	uint8_t payload[] = "payload";

	callback_result = TRANSPORT_COAP_RESPONSE_BAD_REQUEST;
	TEST_ASSERT_EQUAL(TRANSPORT_COAP_RESPONSE_BAD_REQUEST,
			  transport_cloud_bytes_send(payload, sizeof(payload), true));
}

void test_unauthorized_maps_to_reconnect_error(void)
{
	uint8_t payload[] = "payload";

	callback_result = TRANSPORT_COAP_RESPONSE_UNAUTHORIZED;
	TEST_ASSERT_EQUAL(-EACCES, transport_cloud_bytes_send(payload, sizeof(payload), true));
}

void test_empty_response_is_not_delivery_success(void)
{
	uint8_t payload[] = "payload";

	callback_result = 0;
	TEST_ASSERT_EQUAL(-EPROTO, transport_cloud_bytes_send(payload, sizeof(payload), true));
}

void test_synchronous_post_error_is_returned(void)
{
	uint8_t payload[] = "payload";

	transport_cloud_backend_confirmable_post_fake.return_val = -EIO;
	TEST_ASSERT_EQUAL(-EIO, transport_cloud_bytes_send(payload, sizeof(payload), true));
}

void test_disconnected_confirmable_send_is_rejected(void)
{
	uint8_t payload[] = "payload";

	transport_cloud_backend_is_connected_fake.return_val = false;
	TEST_ASSERT_EQUAL(-EACCES, transport_cloud_bytes_send(payload, sizeof(payload), true));
	TEST_ASSERT_EQUAL(0, transport_cloud_backend_confirmable_post_fake.call_count);
}

extern int unity_main(void);

int main(void)
{
	(void)unity_main();
	return 0;
}
