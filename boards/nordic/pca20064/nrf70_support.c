/*
 * Copyright (c) 2024 Nordic Semiconductor ASA.
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/dt-bindings/regulator/npm6001.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(pca20064_wifi_power, CONFIG_BOARD_LOG_LEVEL);

static const struct device *const wifi_regulator = DEVICE_DT_GET(DT_NODELABEL(reg_wifi));
static const struct gpio_dt_spec rf_frontend_enable =
	GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), rf_fe_enable_gpios);

int nrf_wifi_if_zep_start_board(const struct device *dev)
{
	ARG_UNUSED(dev);

	if (!device_is_ready(wifi_regulator)) {
		LOG_ERR("Wi-Fi regulator is not ready");
		return -ENODEV;
	}

	if (!gpio_is_ready_dt(&rf_frontend_enable)) {
		LOG_ERR("Short-range RF front-end GPIO is not ready");
		return -ENODEV;
	}

	int ret = gpio_pin_configure_dt(&rf_frontend_enable, GPIO_OUTPUT_INACTIVE);

	if (ret != 0) {
		LOG_ERR("Cannot configure short-range RF front-end GPIO (%d)", ret);
		return ret;
	}

	ret = regulator_enable(wifi_regulator);

	if (ret != 0) {
		LOG_ERR("Cannot enable Wi-Fi regulator (%d)", ret);
		return ret;
	}

	ret = regulator_set_mode(wifi_regulator, NPM6001_MODE_PWM);
	if (ret != 0) {
		LOG_ERR("Cannot set Wi-Fi regulator mode (%d)", ret);
		goto disable_regulator;
	}

	ret = gpio_pin_set_dt(&rf_frontend_enable, 1);
	if (ret != 0) {
		LOG_ERR("Cannot enable short-range RF front-end (%d)", ret);
		goto disable_regulator;
	}

	k_usleep(300);
	return 0;

disable_regulator:
	(void)regulator_disable(wifi_regulator);
	return ret;
}

int nrf_wifi_if_zep_stop_board(const struct device *dev)
{
	ARG_UNUSED(dev);
	int gpio_ret = gpio_pin_set_dt(&rf_frontend_enable, 0);
	int regulator_ret = regulator_disable(wifi_regulator);

	if (gpio_ret != 0) {
		LOG_ERR("Cannot disable short-range RF front-end (%d)", gpio_ret);
	}

	if (regulator_ret != 0) {
		LOG_ERR("Cannot disable Wi-Fi regulator (%d)", regulator_ret);
	}

	return gpio_ret != 0 ? gpio_ret : regulator_ret;
}
