#
# Copyright (c) 2024 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

target_sources(app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/location.c
	${CMAKE_CURRENT_SOURCE_DIR}/location_payload.c
)

# Generate the SenML-CBOR encoder for the Geolocation LwM2M object.
set(zcbor_command
	zcbor code
	--cddl ${ZEPHYR_BASE}/subsys/net/lib/lwm2m/lwm2m_senml_cbor.cddl
	--cddl ${CMAKE_CURRENT_SOURCE_DIR}/location_object.cddl
	--encode
	--short-names
	-t location-object
	--output-cmake location_object.cmake
)
execute_process(COMMAND ${zcbor_command}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMAND_ERROR_IS_FATAL ANY)

include(${CMAKE_CURRENT_BINARY_DIR}/location_object.cmake)

# Regenerate the codec whenever its schema changes.
set_property(
	DIRECTORY
	PROPERTY
	CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/location_object.cddl
)

zephyr_link_libraries(location_object)
target_link_libraries(location_object PRIVATE zephyr_interface)
