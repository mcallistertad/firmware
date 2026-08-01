/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef LOCATION_PAYLOAD_H_
#define LOCATION_PAYLOAD_H_

#include <stdint.h>

#include <modem/location.h>

#include "message_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Encode a successful location event as a Geolocation (14201) SenML-CBOR payload. */
int location_payload_encode(const struct location_event_data *event_data, int64_t timestamp_ms,
			    struct payload *payload);

#ifdef __cplusplus
}
#endif

#endif /* LOCATION_PAYLOAD_H_ */
