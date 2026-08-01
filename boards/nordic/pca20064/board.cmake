# Copyright (c) 2026
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause

if(CONFIG_BOARD_PCA20064_NRF9161_NS)
  set(TFM_PUBLIC_KEY_FORMAT "full")
endif()

if(CONFIG_TFM_FLASH_MERGED_BINARY)
  set_property(TARGET runners_yaml_props_target PROPERTY hex_file tfm_merged.hex)
endif()

if(CONFIG_BOARD_PCA20064_NRF9161 OR CONFIG_BOARD_PCA20064_NRF9161_NS)
  board_runner_args(nrfjprog)
  board_runner_args(nrfutil "--nrf-family=NRF91")
  # NCS v2.9.0's official nRF9161 DK uses this J-Link device alias.
  board_runner_args(jlink "--device=nRF9160_xxAA" "--speed=4000")
endif()

include(${ZEPHYR_BASE}/boards/common/nrfutil.board.cmake)
include(${ZEPHYR_BASE}/boards/common/nrfjprog.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
