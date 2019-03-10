/*
 * libmnlxt/xfrm.h		Libmnlxt Xfrm/IPsec
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_XFRM_H_
#define LIBMNLXT_XFRM_H_

#include <libmnlxt/core.h>
#include <libmnlxt/data.h>
#include <libmnlxt/xfrm_policy.h>

/**
 * Connects to xfrm netlink socket and initializes mnlxt handle
 * @param handle pointer at mnlxt handle
 * @param groups xfrm netlink multicast groups to subscribe (XFRMNLGRP_* see linux/xfrm.h)
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_connect(mnlxt_handle_t *handle, int group);
/**
 * Dumps netlink data for given xfrm message type
 * @param data pointer at mnlxt data to store mnlxt messages into
 * @param type netlink message type (XFRM_MSG_GET* see linux/xfrm.h)
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_data_dump(mnlxt_data_t *data, int type);
/**
 * Creates a request from a mnlxt message and sends it via netlink
 * @param message pointer at mnlxt message
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_message_request(mnlxt_message_t *message);

/**
 * Create a mnlxt xfrm message
 * @param type netlink message type (XFRM_MSG_GET* see linux/xfrm.h)
 * @param payload structure of mnlxt_xfrm_*_new
 * @return pointer at dynamic allocated mnlxt_message_t structure
 */
mnlxt_message_t *mnlxt_xfrm_message_new(uint16_t type, void *payload);

#endif /* LIBMNLXT_XFRM_H_ */
