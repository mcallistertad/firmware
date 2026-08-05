#
# Copyright (c) 2023 Nordic Semiconductor
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

menu "Trigger"

config APP_TRIGGER_TIMEOUT_SECONDS
	int "Trigger timer timeout"
	default 3600
	range 1 86400

config APP_TRIGGER_MIN_TIMEOUT_SECONDS
	int "Minimum accepted reporting interval"
	default 1
	range 1 86400
	help
	  Clamp shorter update intervals received from the cloud shadow. Boards may
	  raise this value to enforce a battery-life policy while retaining remote
	  control over longer intervals.

config FREQUENT_POLL_DURATION_INTERVAL_SEC
	int "Poll mode duration"
	default 600
	range 1 86400

config APP_TRIGGER_LOCATION_BLOCK_TIMEOUT_SECONDS
	int "Maximum time reporting may be blocked by a location search"
	default 180
	range 1 3600
	help
	  Resume cloud polling if the location library does not publish a terminal
	  event. This timeout should be longer than the configured location request
	  timeout.

module = APP_TRIGGER
module-str = Trigger
source "subsys/logging/Kconfig.template.log_config"

endmenu # Trigger
