/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <errno.h>

#include <date_time.h>
#include <modem/modem_info.h>
#include <zephyr/app_version.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "device_info.h"
#include "device_info_payload.h"
#include "message_channel.h"

LOG_MODULE_REGISTER(device_info, CONFIG_APP_DEVICE_INFO_LOG_LEVEL);

static bool published;

int device_info_publish(void)
{
	char imei[MODEM_INFO_MAX_RESPONSE_SIZE] = {0};
	char iccid[MODEM_INFO_MAX_RESPONSE_SIZE] = {0};
	char modem_firmware[MODEM_INFO_FWVER_SIZE] = {0};
	struct device_info_values values = {
		.imei = imei,
		.iccid = iccid,
		.modem_firmware = modem_firmware,
		.application_firmware = APP_VERSION_STRING,
		.board = CONFIG_BOARD,
		.battery_model = CONFIG_APP_DEVICE_INFO_BATTERY_MODEL,
	};
	struct payload payload = {0};
	int64_t timestamp_ms;
	int err;

	if (published) {
		return 0;
	}

	err = modem_info_string_get(MODEM_INFO_IMEI, imei, sizeof(imei));
	if (err <= 0) {
		return err < 0 ? err : -ENODATA;
	}

	err = modem_info_string_get(MODEM_INFO_ICCID, iccid, sizeof(iccid));
	if (err <= 0) {
		return err < 0 ? err : -ENODATA;
	}

	err = modem_info_string_get(MODEM_INFO_FW_VERSION, modem_firmware, sizeof(modem_firmware));
	if (err <= 0) {
		return err < 0 ? err : -ENODATA;
	}

	err = date_time_now(&timestamp_ms);
	if (err) {
		return err;
	}

	err = device_info_payload_encode(&values, timestamp_ms, &payload);
	if (err) {
		return err;
	}
	payload.object_id = DEVICE_INFO_OBJECT_ID;

	err = zbus_chan_pub(&PAYLOAD_CHAN, &payload, K_SECONDS(1));
	if (err) {
		return err;
	}

	LOG_INF("Device information payload queued");

	return 0;
}

void device_info_delivery_status(int err)
{
	if (published) {
		return;
	}

	if (err) {
		LOG_WRN("Device information delivery failed: %d", err);
		return;
	}

	published = true;
	LOG_INF("Device information published");
}
