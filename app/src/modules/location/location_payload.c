/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "location_object_encode.h"
#include "location_payload.h"

#define GEOLOCATION_DEVICE_INSTANCE	 "14201/0/"
#define GEOLOCATION_GROUND_FIX_INSTANCE	 "14201/1/"
#define GEOLOCATION_SINGLE_CELL_INSTANCE "14201/2/"

struct location_payload_mapping {
	const char *base_name;
	const char *source;
};

static struct location_payload_mapping
location_mapping_get(const struct location_event_data *event_data)
{
	switch (event_data->method) {
	case LOCATION_METHOD_GNSS:
		return (struct location_payload_mapping){
			.base_name = GEOLOCATION_DEVICE_INSTANCE,
			.source = "GNSS",
		};
	case LOCATION_METHOD_WIFI:
		return (struct location_payload_mapping){
			.base_name = GEOLOCATION_GROUND_FIX_INSTANCE,
			.source = "WIFI",
		};
	case LOCATION_METHOD_CELLULAR:
#if defined(CONFIG_LOCATION_DATA_DETAILS)
		if (event_data->location.details.cellular.ncells_count == 0 &&
		    event_data->location.details.cellular.gci_cells_count == 0) {
			return (struct location_payload_mapping){
				.base_name = GEOLOCATION_SINGLE_CELL_INSTANCE,
				.source = "SCELL",
			};
		}

		return (struct location_payload_mapping){
			.base_name = GEOLOCATION_GROUND_FIX_INSTANCE,
			.source = "MCELL",
		};
#else
		return (struct location_payload_mapping){
			.base_name = GEOLOCATION_GROUND_FIX_INSTANCE,
			.source = "CELLULAR",
		};
#endif
	case LOCATION_METHOD_WIFI_CELLULAR:
		/* NCS 2.9 does not expose which input fulfilled a combined cloud fix. */
		return (struct location_payload_mapping){
			.base_name = GEOLOCATION_GROUND_FIX_INSTANCE,
			.source = "WIFI+CELLULAR",
		};
	default:
		return (struct location_payload_mapping){0};
	}
}

int location_payload_encode(const struct location_event_data *event_data, int64_t timestamp_ms,
			    struct payload *payload)
{
	struct location_payload_mapping mapping;
	struct location_object location_object = {0};
	int64_t timestamp_s;

	if (event_data == NULL || payload == NULL || event_data->id != LOCATION_EVT_LOCATION) {
		return -EINVAL;
	}

	if (!isfinite(event_data->location.latitude) || event_data->location.latitude < -90.0 ||
	    event_data->location.latitude > 90.0 || !isfinite(event_data->location.longitude) ||
	    event_data->location.longitude < -180.0 || event_data->location.longitude > 180.0 ||
	    !isfinite(event_data->location.accuracy) || event_data->location.accuracy < 0.0f) {
		return -ERANGE;
	}

	timestamp_s = timestamp_ms / 1000;
	if (timestamp_s < 0 || timestamp_s > UINT32_MAX) {
		return -ERANGE;
	}

	mapping = location_mapping_get(event_data);
	if (mapping.base_name == NULL || mapping.source == NULL) {
		return -ENOTSUP;
	}

	location_object.latitude_m.bn.value = (const uint8_t *)mapping.base_name;
	location_object.latitude_m.bn.len = strlen(mapping.base_name);
	location_object.latitude_m.vf = event_data->location.latitude;
	location_object.latitude_m.bt = (uint32_t)timestamp_s;
	location_object.longitude_m.vf = event_data->location.longitude;
	location_object.radius_m.vf = event_data->location.accuracy;
	location_object.location_source_m.vs.value = (const uint8_t *)mapping.source;
	location_object.location_source_m.vs.len = strlen(mapping.source);

	return cbor_encode_location_object(payload->buffer, sizeof(payload->buffer),
					   &location_object, &payload->buffer_len);
}
