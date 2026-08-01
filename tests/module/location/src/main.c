/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <string.h>

#include <unity.h>

#include "location_object_decode.h"
#include "location_payload.h"

#define TEST_TIMESTAMP_MS INT64_C(1785587696123)
#define TEST_TIMESTAMP_S  UINT32_C(1785587696)
#define TEST_LATITUDE	  53.349805
#define TEST_LONGITUDE	  -6.260310
#define TEST_ACCURACY	  24.5f

static struct location_event_data location_event;

static void assert_string_equal(const char *expected, const struct zcbor_string *actual)
{
	TEST_ASSERT_EQUAL_size_t(strlen(expected), actual->len);
	TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual->value, actual->len);
}

static struct location_object encode_and_decode(enum location_method method)
{
	struct location_object decoded = {0};
	struct payload payload = {0};
	int err;

	location_event.method = method;
	err = location_payload_encode(&location_event, TEST_TIMESTAMP_MS, &payload);
	TEST_ASSERT_EQUAL_INT(0, err);
	TEST_ASSERT_GREATER_THAN_size_t(0, payload.buffer_len);

	err = cbor_decode_location_object(payload.buffer, payload.buffer_len, &decoded, NULL);
	TEST_ASSERT_EQUAL_INT(0, err);

	return decoded;
}

void setUp(void)
{
	memset(&location_event, 0, sizeof(location_event));
	location_event.id = LOCATION_EVT_LOCATION;
	location_event.location.latitude = TEST_LATITUDE;
	location_event.location.longitude = TEST_LONGITUDE;
	location_event.location.accuracy = TEST_ACCURACY;
}

void tearDown(void)
{
}

void test_gnss_payload_contains_dashboard_geolocation_fields(void)
{
	struct location_object decoded = encode_and_decode(LOCATION_METHOD_GNSS);

	assert_string_equal("14201/0/", &decoded.latitude_m.bn);
	TEST_ASSERT_EQUAL_DOUBLE(TEST_LATITUDE, decoded.latitude_m.vf);
	TEST_ASSERT_EQUAL_DOUBLE(TEST_LONGITUDE, decoded.longitude_m.vf);
	TEST_ASSERT_EQUAL_DOUBLE(TEST_ACCURACY, decoded.radius_m.vf);
	TEST_ASSERT_EQUAL_UINT32(TEST_TIMESTAMP_S, decoded.latitude_m.bt);
	assert_string_equal("GNSS", &decoded.location_source_m.vs);
}

void test_wifi_payload_uses_ground_fix_instance(void)
{
	struct location_object decoded = encode_and_decode(LOCATION_METHOD_WIFI);

	assert_string_equal("14201/1/", &decoded.latitude_m.bn);
	assert_string_equal("WIFI", &decoded.location_source_m.vs);
}

void test_single_cell_payload_uses_single_cell_instance(void)
{
	struct location_object decoded = encode_and_decode(LOCATION_METHOD_CELLULAR);

	assert_string_equal("14201/2/", &decoded.latitude_m.bn);
	assert_string_equal("SCELL", &decoded.location_source_m.vs);
}

void test_multi_cell_payload_uses_ground_fix_instance(void)
{
	struct location_object decoded;

	location_event.location.details.cellular.ncells_count = 2;
	decoded = encode_and_decode(LOCATION_METHOD_CELLULAR);

	assert_string_equal("14201/1/", &decoded.latitude_m.bn);
	assert_string_equal("MCELL", &decoded.location_source_m.vs);
}

void test_combined_payload_does_not_claim_an_unknown_fulfilment_source(void)
{
	struct location_object decoded = encode_and_decode(LOCATION_METHOD_WIFI_CELLULAR);

	assert_string_equal("14201/1/", &decoded.latitude_m.bn);
	assert_string_equal("WIFI+CELLULAR", &decoded.location_source_m.vs);
}

void test_invalid_location_data_is_rejected(void)
{
	struct payload payload = {0};

	location_event.location.latitude = 91.0;
	TEST_ASSERT_NOT_EQUAL(
		0, location_payload_encode(&location_event, TEST_TIMESTAMP_MS, &payload));

	location_event.location.latitude = TEST_LATITUDE;
	location_event.location.accuracy = -1.0f;
	TEST_ASSERT_NOT_EQUAL(
		0, location_payload_encode(&location_event, TEST_TIMESTAMP_MS, &payload));
}

void test_non_location_event_and_unknown_method_are_rejected(void)
{
	struct payload payload = {0};

	location_event.id = LOCATION_EVT_TIMEOUT;
	TEST_ASSERT_NOT_EQUAL(
		0, location_payload_encode(&location_event, TEST_TIMESTAMP_MS, &payload));

	location_event.id = LOCATION_EVT_LOCATION;
	location_event.method = (enum location_method)99;
	TEST_ASSERT_NOT_EQUAL(
		0, location_payload_encode(&location_event, TEST_TIMESTAMP_MS, &payload));
}

extern int unity_main(void);

int main(void)
{
	(void)unity_main();

	return 0;
}
