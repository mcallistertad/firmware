/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <string.h>

#include <unity.h>

#include "device_info_object_decode.h"
#include "device_info_payload.h"

#define TEST_TIMESTAMP_MS INT64_C(1785587696123)
#define TEST_TIMESTAMP_S  UINT32_C(1785587696)

static const struct device_info_values values = {
	.imei = "123456789012345",
	.iccid = "89457300000066612345",
	.modem_firmware = "mfw_nrf91x1_2.0.0",
	.application_firmware = "0.0.0-dev",
	.board = "pca20064",
	.battery_model = "LP803448",
};

static void assert_string_equal(const char *expected, const struct zcbor_string *actual)
{
	TEST_ASSERT_EQUAL_size_t(strlen(expected), actual->len);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual->value, actual->len);
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_payload_contains_dashboard_device_information_fields(void)
{
	struct device_info_object decoded = {0};
	struct payload payload = {0};
	int err;

	err = device_info_payload_encode(&values, TEST_TIMESTAMP_MS, &payload);
	TEST_ASSERT_EQUAL_INT(0, err);
	TEST_ASSERT_GREATER_THAN_size_t(0, payload.buffer_len);

	err = cbor_decode_device_info_object(payload.buffer, payload.buffer_len, &decoded, NULL);
	TEST_ASSERT_EQUAL_INT(0, err);

	assert_string_equal("14204/0/", &decoded.imei_m.bn);
	assert_string_equal(values.imei, &decoded.imei_m.vs);
	TEST_ASSERT_EQUAL_UINT32(TEST_TIMESTAMP_S, decoded.imei_m.bt);
	assert_string_equal(values.iccid, &decoded.iccid_m.vs);
	assert_string_equal(values.modem_firmware, &decoded.modem_firmware_m.vs);
	assert_string_equal(values.application_firmware, &decoded.application_firmware_m.vs);
	assert_string_equal(values.board, &decoded.board_m.vs);
	assert_string_equal(values.battery_model, &decoded.battery_model_m.vs);
}

void test_invalid_arguments_and_timestamp_are_rejected(void)
{
	struct device_info_values invalid = values;
	struct payload payload = {0};

	TEST_ASSERT_EQUAL_INT(-EINVAL,
			      device_info_payload_encode(NULL, TEST_TIMESTAMP_MS, &payload));
	TEST_ASSERT_EQUAL_INT(-EINVAL,
			      device_info_payload_encode(&values, TEST_TIMESTAMP_MS, NULL));

	invalid.imei = "";
	TEST_ASSERT_EQUAL_INT(-EINVAL,
			      device_info_payload_encode(&invalid, TEST_TIMESTAMP_MS, &payload));

	invalid = values;
	invalid.iccid = "";
	TEST_ASSERT_EQUAL_INT(-EINVAL,
			      device_info_payload_encode(&invalid, TEST_TIMESTAMP_MS, &payload));

	TEST_ASSERT_EQUAL_INT(-ERANGE, device_info_payload_encode(&values, -1000, &payload));
}

extern int unity_main(void);

int main(void)
{
	(void)unity_main();

	return 0;
}
