/*
 * link_data_vlan.h		Libmnlxt Internal Routing Link Data, VLAN interface
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LINK_DATA_VLAN_H_
#define LINK_DATA_VLAN_H_

#include "libmnlxt/rt_link.h"

int mnlxt_rt_link_vlan_cmp(const mnlxt_rt_link_vlan_t *vlan1, uint16_t vlan_flags1, const mnlxt_rt_link_vlan_t *vlan2,
													 uint16_t vlan_flags2, uint16_t filter);
void mnlxt_rt_link_vlan_info_put(struct nlmsghdr *nlh, const mnlxt_rt_link_t *rt_link);
int mnlxt_rt_link_vlan_info_data(const struct nlattr *link_vlan_attr, mnlxt_data_t *data, mnlxt_rt_link_t *rt_link);

#endif /* LINK_DATA_VLAN_H_ */
