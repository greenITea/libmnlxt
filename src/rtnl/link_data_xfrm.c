/*
 * link_data_xfrm.c		Libmnlxt Routing Link Data, XFRM interface
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>
#include <linux/if_link.h>

#include "config.h"
#include "libmnlxt/rt.h"

int mnlxt_rt_link_xfrm_cmp(const mnlxt_rt_link_xfrm_t *xfrm1, uint16_t xfrm_flags1, const mnlxt_rt_link_xfrm_t *xfrm2,
													 uint16_t xfrm_flags2, uint16_t filter) {
	int i = -1;
	if (NULL == xfrm1 || NULL == xfrm2) {
		errno = EINVAL;
	} else {
		uint16_t flag = 1;
		for (i = 0; i < MNLXT_RT_LINK_XFRM_MAX; ++i, flag <<= 1) {
			if (0 == (flag & filter)) {
				continue;
			}
			if (0 == (xfrm_flags1 & flag)) {
				if (0 == (xfrm_flags2 & flag)) {
					/* both not set */
					continue;
				}
				goto failed;
			} else if (0 == (xfrm_flags2 & flag)) {
				goto failed;
			} else {
				switch (i) {
				case MNLXT_RT_LINK_XFRM_IFINDEX:
					if (xfrm1->if_index != xfrm2->if_index) {
						goto failed;
					}
					break;
				case MNLXT_RT_LINK_XFRM_ID:
					if (xfrm1->id != xfrm2->id) {
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

#ifdef HAVE_IFLA_XFRM
void mnlxt_rt_link_xfrm_info_put(struct nlmsghdr *nlh, const mnlxt_rt_link_t *rt_link) {
	uint32_t u32 = 0;
	if (0 == mnlxt_rt_link_get_xfrm_id(rt_link, &u32)) {
		mnl_attr_put_u32(nlh, IFLA_XFRM_IF_ID, u32);
	}
	if (0 == mnlxt_rt_link_get_xfrm_ifindex(rt_link, &u32)) {
		mnl_attr_put_u32(nlh, IFLA_XFRM_LINK, u32);
	}
}

int mnlxt_rt_link_xfrm_info_data(const struct nlattr *link_xfrm_attr, mnlxt_data_t *data, mnlxt_rt_link_t *rt_link) {
	int rc = -1;
	const struct nlattr *attr;

	mnl_attr_for_each_nested(attr, link_xfrm_attr) {
		int type = mnl_attr_get_type(attr);
		switch (type) {
		case IFLA_XFRM_LINK:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "IFLA_XFRM_LINK validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_xfrm_ifindex(rt_link, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_link_set_xfrm_ifindex failed";
				goto end;
			}
			break;
		case IFLA_XFRM_IF_ID:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "IFLA_XFRM_IF_ID validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_xfrm_id(rt_link, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_link_set_xfrm_id failed";
				goto end;
			}
			break;
		}
	}
	rc = 0;
end:
	return rc;
}
#endif
