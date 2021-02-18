/*
 * link_data_tun.c		Libmnlxt Routing Link Data, TUN/TAP devices
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>
#include <linux/if_link.h>
#include <linux/if_tun.h>

#include "libmnlxt/rt.h"

int mnlxt_rt_link_tun_cmp(const mnlxt_rt_link_tun_t *tun1, uint16_t tun_flags1, const mnlxt_rt_link_tun_t *tun2,
													uint16_t tun_flags2, uint16_t filter) {
	int i = -1;
	if (NULL == tun1 || NULL == tun2) {
		errno = EINVAL;
	} else {
		uint16_t flag = 1;
		for (i = 0; i < MNLXT_RT_LINK_TUN_MAX; ++i, flag <<= 1) {
			if (0 == (flag & filter)) {
				continue;
			}
			if (0 == (tun_flags1 & flag)) {
				if (0 == (tun_flags2 & flag)) {
					/* both not set */
					continue;
				}
				goto failed;
			} else if (0 == (tun_flags2 & flag)) {
				goto failed;
			} else {
				switch (i) {
				case MNLXT_RT_LINK_TUN_TYPE:
					if (tun1->type != tun2->type) {
						goto failed;
					}
					break;
				case MNLXT_RT_LINK_TUN_FLAGS:
					if (tun1->flags != tun2->flags) {
						goto failed;
					}
					break;
				case MNLXT_RT_LINK_TUN_UID:
					if (tun1->uid != tun2->uid) {
						goto failed;
					}
					break;
				case MNLXT_RT_LINK_TUN_GID:
					if (tun1->uid != tun2->uid) {
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

#if 0
void mnlxt_rt_link_tun_info_put(struct nlmsghdr *nlh, const mnlxt_rt_link_t *rt_link) {
	uint32_t u8 = 0;
	uint32_t u16 = 0;
	uint32_t u32 = 0;
	if (0 == mnlxt_rt_link_get_tun_uid(rt_link, (uid_t *)&u32)) {
		mnl_attr_put_u32(nlh, IFLA_TUN_OWNER, u32);
	}
	if (0 == mnlxt_rt_link_get_tun_gid(rt_link, (gid_t *)&u32)) {
		mnl_attr_put_u32(nlh, IFLA_TUN_GROUP, u32);
	}
	if (0 == mnlxt_rt_link_get_tun_type(rt_link, &u8)) {
		mnl_attr_put_u8(nlh, IFLA_TUN_TYPE, u8);
	}
	if (0 == mnlxt_rt_link_get_tun_flags(rt_link, &u16)) {
		if ((u16 & MNLXT_RT_LINK_TUN_FLAG_PI)) {
			mnl_attr_put_u8(nlh, IFLA_TUN_PI, 1);
		}
		if (u16 & MNLXT_RT_LINK_TUN_FLAG_VNET_HDR) {
			mnl_attr_put_u8(nlh, IFLA_TUN_VNET_HDR, 1);
		}
		if (u16 & MNLXT_RT_LINK_TUN_FLAG_PERSIST) {
			mnl_attr_put_u8(nlh, IFLA_TUN_PERSIST, 1);
		}
		if (u16 & MNLXT_RT_LINK_TUN_FLAG_MULTI_QUEUE) {
			mnl_attr_put_u8(nlh, IFLA_TUN_MULTI_QUEUE, 1);
		}
	}
}
#endif

int mnlxt_rt_link_info_data_tun(const struct nlattr *link_tun_attr, mnlxt_data_t *data, mnlxt_rt_link_t *rt_link) {
	int rc = -1;
	const struct nlattr *attr;
	uint32_t flags = 0;

	mnl_attr_for_each_nested(attr, link_tun_attr) {
		int type = mnl_attr_get_type(attr);
		switch (type) {
		case IFLA_TUN_OWNER:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "IFLA_TUN_OWNER validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_tun_uid(rt_link, (uid_t)mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_link_set_tun_uid failed";
				goto end;
			}
			break;
		case IFLA_TUN_GROUP:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "IFLA_TUN_GROUP validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_tun_gid(rt_link, (gid_t)mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_link_set_tun_gid failed";
				goto end;
			}
			break;
		case IFLA_TUN_TYPE:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U8)) {
				data->error_str = "IFLA_TUN_TYPE validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_tun_type(rt_link, mnl_attr_get_u8(attr))) {
				data->error_str = "mnlxt_rt_link_set_tun_type failed";
				goto end;
			}
			break;
		case IFLA_TUN_PI:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U8)) {
				data->error_str = "IFLA_TUN_PI validation failed";
				goto end;
			}
			if (mnl_attr_get_u8(attr)) {
				flags |= MNLXT_RT_LINK_TUN_FLAG_PI;
			}
			break;
		case IFLA_TUN_VNET_HDR:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U8)) {
				data->error_str = "IFLA_TUN_VNET_HDR validation failed";
				goto end;
			}
			if (mnl_attr_get_u8(attr)) {
				flags |= MNLXT_RT_LINK_TUN_FLAG_VNET_HDR;
			}
			break;
		case IFLA_TUN_PERSIST:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U8)) {
				data->error_str = "IFLA_TUN_PERSIST validation failed";
				goto end;
			}
			if (mnl_attr_get_u8(attr)) {
				flags |= MNLXT_RT_LINK_TUN_FLAG_PERSIST;
			}
			break;
		case IFLA_TUN_MULTI_QUEUE:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U8)) {
				data->error_str = "IFLA_TUN_MULTI_QUEUE validation failed";
				goto end;
			}
			if (mnl_attr_get_u8(attr)) {
				flags |= MNLXT_RT_LINK_TUN_FLAG_MULTI_QUEUE;
			}
			break;
		}
	}
	if (-1 == mnlxt_rt_link_set_tun_flags(rt_link, flags)) {
		data->error_str = "mnlxt_rt_link_set_tun_flags failed";
		goto end;
	}
	rc = 0;
end:
	return rc;
}
