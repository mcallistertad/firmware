/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "device_info_object_encode.h"
#include "device_info_payload.h"

#define DEVICE_INFO_INSTANCE "14204/0/"

static bool string_is_valid(const char *value)
{
	return value != NULL && value[0] != '\0';
}

static void zcbor_string_set(struct zcbor_string *target, const char *value)
{
	target->value = (const uint8_t *)value;
	target->len = strlen(value);
}

int device_info_payload_encode(const struct device_info_values *values, int64_t timestamp_ms,
			       struct payload *payload)
{
	struct device_info_object object = {0};
	int64_t timestamp_s;

	if (values == NULL || payload == NULL || !string_is_valid(values->imei) ||
	    !string_is_valid(values->iccid) || !string_is_valid(values->modem_firmware) ||
	    !string_is_valid(values->application_firmware) || !string_is_valid(values->board) ||
	    !string_is_valid(values->battery_model)) {
		return -EINVAL;
	}

	timestamp_s = timestamp_ms / 1000;
	if (timestamp_s < 0 || timestamp_s > UINT32_MAX) {
		return -ERANGE;
	}

	zcbor_string_set(&object.imei_m.bn, DEVICE_INFO_INSTANCE);
	zcbor_string_set(&object.imei_m.vs, values->imei);
	object.imei_m.bt = (uint32_t)timestamp_s;
	zcbor_string_set(&object.iccid_m.vs, values->iccid);
	zcbor_string_set(&object.modem_firmware_m.vs, values->modem_firmware);
	zcbor_string_set(&object.application_firmware_m.vs, values->application_firmware);
	zcbor_string_set(&object.board_m.vs, values->board);
	zcbor_string_set(&object.battery_model_m.vs, values->battery_model);

	return cbor_encode_device_info_object(payload->buffer, sizeof(payload->buffer), &object,
					      &payload->buffer_len);
}
