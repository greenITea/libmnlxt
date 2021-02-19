/*
 * link_data_tun.h		Libmnlxt Internal Routing Link Data, TUN/TAP devices
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LINK_DATA_TUN_H_
#define LINK_DATA_TUN_H_

#include "libmnlxt/rt_link.h"

int mnlxt_rt_link_tun_cmp(const mnlxt_rt_link_tun_t *tun1, uint16_t tun_flags1, const mnlxt_rt_link_tun_t *tun2,
													uint16_t tun_flags2, uint16_t filter);
#if 0
void mnlxt_rt_link_tun_info_put(struct nlmsghdr *nlh, const mnlxt_rt_link_t *rt_link);
#endif
int mnlxt_rt_link_tun_info_data(const struct nlattr *link_tun_attr, mnlxt_data_t *data, mnlxt_rt_link_t *rt_link);

#endif /* LINK_DATA_TUN_H_ */
