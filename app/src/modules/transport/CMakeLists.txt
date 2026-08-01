#
# Copyright (c) 2023 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

target_sources(app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/cloud_send.c
	${CMAKE_CURRENT_SOURCE_DIR}/cloud_send_backend.c
	${CMAKE_CURRENT_SOURCE_DIR}/transport.c
)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
