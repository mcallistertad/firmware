#
# Copyright (c) 2026 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(device_info_module_test)

test_runner_generate(src/main.c)

target_sources(app PRIVATE
	src/main.c
	../../../app/src/modules/device_info/device_info_payload.c
)

zephyr_include_directories(
	../../../app/src/common
	../../../app/src/modules/device_info
)

target_compile_definitions(app PRIVATE
	-DCONFIG_APP_PAYLOAD_CHANNEL_BUFFER_MAX_SIZE=192
)

set(zcbor_command
	zcbor code
	--cddl ${ZEPHYR_BASE}/subsys/net/lib/lwm2m/lwm2m_senml_cbor.cddl
	--cddl ${APPLICATION_SOURCE_DIR}/../../../app/src/modules/device_info/device_info_object.cddl
	--encode
	--decode
	--short-names
	-t device-info-object
	--output-cmake device_info_object.cmake
)
execute_process(COMMAND ${zcbor_command}
				WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
				COMMAND_ERROR_IS_FATAL ANY)

include(${CMAKE_CURRENT_BINARY_DIR}/device_info_object.cmake)

zephyr_link_libraries(device_info_object)
target_link_libraries(device_info_object PRIVATE zephyr_interface)
