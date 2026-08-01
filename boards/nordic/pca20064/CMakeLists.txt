# Copyright (c) 2026
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause

if(CONFIG_BOARD_PCA20064_NRF9161_NS)
  set(PM_STATIC_YML_FILE
      ${CMAKE_CURRENT_LIST_DIR}/pca20064_nrf9161_pm_static.yml
      CACHE INTERNAL "PCA20064 static partition map")
endif()

if(CONFIG_BOARD_PCA20064_NRF9161_NS)
  zephyr_library()

  # Keep the nRF5340 USB/connectivity bridge out of reset in every NS app.
  # B0 and MCUboot use the secure board target and do not compile this file.
  zephyr_library_sources(pca20064_board.c)

  if(CONFIG_WIFI_NRF70)
    zephyr_library_sources(nrf70_support.c)
  endif()
endif()
