/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef DEVICE_INFO_PAYLOAD_H_
#define DEVICE_INFO_PAYLOAD_H_

#include <stdint.h>

#include "message_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

struct device_info_values {
	const char *imei;
	const char *iccid;
	const char *modem_firmware;
	const char *application_firmware;
	const char *board;
	const char *battery_model;
};

/** Encode Device Information (14204) as a SenML-CBOR payload. */
int device_info_payload_encode(const struct device_info_values *values, int64_t timestamp_ms,
			       struct payload *payload);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_INFO_PAYLOAD_H_ */
