/*
 * rt.h
 *
 *  Created on: Nov 13, 2018
 *      Author: Andrey Alexandrenko
 */

#ifndef INCLUDE_LIBMNLXT_RT_H_
#define INCLUDE_LIBMNLXT_RT_H_

#include "core.h"
#include "data.h"
#include "rt_addr.h"
#include "rt_link.h"
#include "rt_route.h"
#include "rt_rule.h"

/**
 * Connects to rtnetlink socket and initializes mnlxt handle
 * @param handle pointer at mnlxt handle
 * @param groups rtnetlink multicast groups to subscribe (RTMGRP_* see linux/rtnetlink.h)
 * @return 0 on success, else -1
 */
int mnlxt_rt_connect(mnlxt_handle_t *handle, int groups);
/**
 * Dumps netlink data for given rtnetlink message type
 * @param data pointer at mnlxt data to store mnlxt messages into
 * @param type rtnetlink message type (RTM_GET* see linux/rtnetlink.h)
 * @param family address family (AF_* see bits/socket.h)
 * @return 0 on success, else -1
 */
int mnlxt_rt_data_dump(mnlxt_data_t *data, int type, unsigned char family);
/**
 * Creates a request from a mnlxt message and sends it via netlink
 * @param message pointer at mnlxt message
 * @return 0 on success, else -1
 */
int mnlxt_rt_message_request(mnlxt_message_t *message);

/**
 * Create a mnlxt routing message
 * @param type rtnetlink message type (RTM_GET* see linux/rtnetlink.h)
 * @param payload structure of mnlxt_rt_*_new
 * @return pointer at dynamic allocated mnlxt_message_t structure
 */
mnlxt_message_t *mnlxt_rt_message_new(uint16_t type, void *payload);

#endif /* INCLUDE_LIBMNLXT_RT_H_ */
