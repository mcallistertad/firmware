/*
 * Copyright (c) 2026 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef DEVICE_INFO_H_
#define DEVICE_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Publish the hello.nrfcloud.com Device Information (14204) object. */
int device_info_publish(void);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_INFO_H_ */
