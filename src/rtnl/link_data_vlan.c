/*
 * link_data_vlan.c		Libmnlxt Routing Link Data, VLAN interface
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>
#include <linux/if_link.h>

#include "libmnlxt/rt.h"

int mnlxt_rt_link_vlan_cmp(const mnlxt_rt_link_vlan_t *vlan1, uint16_t vlan_flags1, const mnlxt_rt_link_vlan_t *vlan2,
													 uint16_t vlan_flags2, uint16_t filter) {
	int i = -1;
	if (NULL == vlan1 || NULL == vlan2) {
		errno = EINVAL;
	} else {
		uint16_t flag = 1;
		for (i = 0; i < MNLXT_RT_LINK_VLAN_MAX; ++i, flag <<= 1) {
			if (0 == (flag & filter)) {
				continue;
			}
			if (0 == (vlan_flags1 & flag)) {
				if (0 == (vlan_flags2 & flag)) {
					/* both not set */
					continue;
				}
				goto failed;
			} else if (0 == (vlan_flags2 & flag)) {
				goto failed;
			} else {
				switch (i) {
				case MNLXT_RT_LINK_VLAN_ID:
					if (vlan1->id != vlan2->id) {
						goto failed;
					}
					break;
				}
			}
		}
		return 0;
	failed:
		++i;
	}
	return i;
}

void mnlxt_rt_link_vlan_info_put(struct nlmsghdr *nlh, const mnlxt_rt_link_t *rt_link) {
	uint16_t id = 0;
	if (0 == mnlxt_rt_link_get_vlan_id(rt_link, &id)) {
		mnl_attr_put_u16(nlh, IFLA_VLAN_ID, id);
	}
}

int mnlxt_rt_link_vlan_info_data(const struct nlattr *link_vlan_attr, mnlxt_data_t *data, mnlxt_rt_link_t *link) {
	int rc = -1;
	const struct nlattr *attr;

	mnl_attr_for_each_nested(attr, link_vlan_attr) {
		int type = mnl_attr_get_type(attr);
		switch (type) {
		case IFLA_VLAN_ID:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U16)) {
				data->error_str = "IFLA_VLAN_ID validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_vlan_id(link, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_link_set_vlan_id failed";
				goto end;
			}
			break;
			/*
			 * IFLA_VLAN_FLAGS,
			 * IFLA_VLAN_EGRESS_QOS,
			 * IFLA_VLAN_INGRESS_QOS,
			 * IFLA_VLAN_PROTOCOL
			 */
		}
	}
	rc = 0;
end:
	return rc;
}
