/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Return true when an operation error means the cloud session is no longer usable. */
bool transport_cloud_error_requires_reconnect(int err);

/** Request a coordinated disconnect and reconnect of the nRF Cloud transport. */
void transport_cloud_reconnect_request(int reason);

#ifdef __cplusplus
}
#endif

#endif /* TRANSPORT_H_ */
