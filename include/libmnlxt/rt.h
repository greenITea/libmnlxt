/*
 * libmnlxt/rt.h		Libmnlxt Routing
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_RT_H_
#define LIBMNLXT_RT_H_

#include <linux/rtnetlink.h>

#include <libmnlxt/core.h>
#include <libmnlxt/data.h>
#include <libmnlxt/rt_addr.h>
#include <libmnlxt/rt_link.h>
#include <libmnlxt/rt_link_tun.h>
#include <libmnlxt/rt_route.h>
#include <libmnlxt/rt_rule.h>

/**
 * Connects to rtnetlink socket and initializes mnlxt handle
 * @param handle pointer to mnlxt handle
 * @param groups rtnetlink multicast groups to subscribe (RTMGRP_* see linux/rtnetlink.h)
 * @return 0 on success, else -1
 */
int mnlxt_rt_connect(mnlxt_handle_t *handle, int groups);
/**
 * Dumps netlink data for given rtnetlink message type
 * @param data pointer to mnlxt data to store mnlxt messages into
 * @param type rtnetlink message type (RTM_GET* see linux/rtnetlink.h)
 * @param family address family (AF_* see bits/socket.h)
 * @return 0 on success, else -1
 */
int mnlxt_rt_data_dump(mnlxt_data_t *data, int type, unsigned char family);
/**
 * Creates a request from a mnlxt message and sends it via netlink
 * @param message pointer to mnlxt message
 * @return 0 on success, else -1
 */
int mnlxt_rt_message_request(mnlxt_message_t *message);

/**
 * Create a mnlxt routing message
 * @param type rtnetlink message type (RTM_GET* see linux/rtnetlink.h)
 * @param flags request flags (NLM_F_* see linux/netlink.h). Set to 0 for default.
 * @param payload structure of mnlxt_rt_*_new
 * @return pointer to dynamic allocated mnlxt_message_t structure
 */
mnlxt_message_t *mnlxt_rt_message_new(uint16_t type, uint16_t flags, void *payload);

#endif /* LIBMNLXT_RT_H_ */
