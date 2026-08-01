/*
 * PCA20064 board safety initialisation.
 * Copyright (c) 2026
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>

#define USER_NODE DT_PATH(zephyr_user)

static const struct gpio_dt_spec nrf5340_reset =
	GPIO_DT_SPEC_GET(USER_NODE, nrf5340_reset_gpios);

static int pca20064_release_nrf5340(void)
{
	if (!gpio_is_ready_dt(&nrf5340_reset)) {
		return -ENODEV;
	}

	/* Active-low reset: logical inactive drives the physical pin high. */
	return gpio_pin_configure_dt(&nrf5340_reset, GPIO_OUTPUT_INACTIVE);
}

SYS_INIT(pca20064_release_nrf5340, POST_KERNEL, 90);
