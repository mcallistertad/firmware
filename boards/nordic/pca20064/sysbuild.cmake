# Copyright (c) 2026
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause

if(SB_CONFIG_BOARD_PCA20064_NRF9161_NS)
  set(PM_STATIC_YML_FILE
      ${CMAKE_CURRENT_LIST_DIR}/pca20064_nrf9161_pm_static.yml
      CACHE INTERNAL "PCA20064 factory-compatible partition map")
endif()
