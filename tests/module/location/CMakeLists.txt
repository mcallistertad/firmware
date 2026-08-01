#
# Copyright (c) 2026 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(location_module_test)

test_runner_generate(src/main.c)

target_sources(app PRIVATE
	src/main.c
	../../../app/src/modules/location/location_payload.c
)

zephyr_include_directories(
	../../../app/src/common
	../../../app/src/modules/location
	${ZEPHYR_BASE}/../nrfxlib/nrf_modem/include
)

target_compile_definitions(app PRIVATE
	-DCONFIG_APP_PAYLOAD_CHANNEL_BUFFER_MAX_SIZE=128
	-DCONFIG_LOCATION_DATA_DETAILS=1
	-DCONFIG_LOCATION_METHOD_CELLULAR=1
	-DCONFIG_LOCATION_METHOD_GNSS=1
	-DCONFIG_LOCATION_METHODS_LIST_SIZE=3
	-DCONFIG_LOCATION_METHOD_WIFI=1
)

set(zcbor_command
	zcbor code
	--cddl ${ZEPHYR_BASE}/subsys/net/lib/lwm2m/lwm2m_senml_cbor.cddl
	--cddl ${APPLICATION_SOURCE_DIR}/../../../app/src/modules/location/location_object.cddl
	--encode
	--decode
	--short-names
	-t location-object
	--output-cmake location_object.cmake
)
execute_process(COMMAND ${zcbor_command}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMAND_ERROR_IS_FATAL ANY)

include(${CMAKE_CURRENT_BINARY_DIR}/location_object.cmake)

zephyr_link_libraries(location_object)
target_link_libraries(location_object PRIVATE zephyr_interface)
