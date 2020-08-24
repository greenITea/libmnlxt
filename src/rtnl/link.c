/*
 * link.c		Libmnlxt Routing Link
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <libmnlxt/rt_link.h>

#include "internal.h"

#define link_ad_init(member) ad_init(mnlxt_rt_link_t, member)

static struct access_data link_data[MNLXT_RT_LINK_MAX] = {
	[MNLXT_RT_LINK_TYPE] = link_ad_init(type),
	[MNLXT_RT_LINK_FAMILY] = link_ad_init(family),
	[MNLXT_RT_LINK_NAME] = {}, // special case
	[MNLXT_RT_LINK_INDEX] = link_ad_init(index),
	[MNLXT_RT_LINK_FLAGS] = link_ad_init(flags),
	[MNLXT_RT_LINK_HWADDR] = link_ad_init(mac),
	[MNLXT_RT_LINK_MTU] = link_ad_init(mtu),
	[MNLXT_RT_LINK_MASTER] = link_ad_init(master),
	[MNLXT_RT_LINK_STATE] = link_ad_init(state),
	[MNLXT_RT_LINK_PARENT] = link_ad_init(parent),
	[MNLXT_RT_LINK_INFO] = {}, // special case
};

mnlxt_rt_link_t *mnlxt_rt_link_new() {
	return calloc(1, sizeof(mnlxt_rt_link_t));
}

mnlxt_rt_link_t *mnlxt_rt_link_clone(const mnlxt_rt_link_t *src, uint64_t filter) {
	mnlxt_rt_link_t *dst = NULL;
	if (NULL == src) {
		errno = EINVAL;
	} else if (NULL != (dst = mnlxt_rt_link_new()) && filter) {
		memcpy(dst, src, sizeof(mnlxt_rt_link_t));
		dst->prop_flags = src->prop_flags & filter;
	}
	return dst;
}

void mnlxt_rt_link_free(mnlxt_rt_link_t *rt_link) {
	if (NULL != rt_link) {
		free(rt_link);
	}
}

void mnlxt_rt_link_FREE(void *rt_link) {
	mnlxt_rt_link_free((mnlxt_rt_link_t *)rt_link);
}
#if 0
void mnlxt_rt_link_data_revoke(mnlxt_rt_link_t *rt_link, mnlxt_rt_link_data_t data) {
	if (NULL != rt_link && MNLXT_RT_LINK_MAX > data) {
		MNLXT_UNSET_PROP_FLAG(rt_link, data);
	}
}
#endif
static int mnlxt_rt_link_set_ptr(mnlxt_rt_link_t *rt_link, mnlxt_rt_link_data_t data, void *ptr, uint8_t size) {
	int rc = -1;
	if (NULL == rt_link || MNLXT_RT_LINK_MAX <= (unsigned)data || link_data[data].size != size
			|| 0 == link_data[data].size) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(rt_link, data);
		memcpy(((char *)rt_link + link_data[data].offset), ptr, size);
		rc = 0;
	}
	return rc;
}

static inline int mnlxt_rt_link_set_u32(mnlxt_rt_link_t *rt_link, mnlxt_rt_link_data_t data, uint32_t u32) {
	return mnlxt_rt_link_set_ptr(rt_link, data, &u32, sizeof(uint32_t));
}

static inline int mnlxt_rt_link_set_u16(mnlxt_rt_link_t *rt_link, mnlxt_rt_link_data_t data, uint16_t u16) {
	return mnlxt_rt_link_set_ptr(rt_link, data, &u16, sizeof(uint16_t));
}

static inline int mnlxt_rt_link_set_u8(mnlxt_rt_link_t *rt_link, mnlxt_rt_link_data_t data, uint8_t u8) {
	return mnlxt_rt_link_set_ptr(rt_link, data, &u8, sizeof(uint8_t));
}

static int mnlxt_rt_link_get_ptr(const mnlxt_rt_link_t *rt_link, mnlxt_rt_link_data_t data, void *ptr, uint8_t size) {
	int rc = -1;
	if (NULL == rt_link || MNLXT_RT_LINK_MAX <= (unsigned)data || link_data[data].size != size
			|| 0 == link_data[data].size) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rt_link, data)) {
		rc = 1;
	} else {
		memcpy(ptr, ((char *)rt_link + link_data[data].offset), size);
		rc = 0;
	}
	return rc;
}

static inline int mnlxt_rt_link_get_u32(const mnlxt_rt_link_t *rt_link, mnlxt_rt_link_data_t data, uint32_t *pu32) {
	return mnlxt_rt_link_get_ptr(rt_link, data, pu32, sizeof(uint32_t));
}

static inline int mnlxt_rt_link_get_u16(const mnlxt_rt_link_t *rt_link, mnlxt_rt_link_data_t data, uint16_t *pu16) {
	return mnlxt_rt_link_get_ptr(rt_link, data, pu16, sizeof(uint16_t));
}

static inline int mnlxt_rt_link_get_u8(const mnlxt_rt_link_t *rt_link, mnlxt_rt_link_data_t data, uint8_t *pu8) {
	return mnlxt_rt_link_get_ptr(rt_link, data, pu8, sizeof(uint8_t));
}

int mnlxt_rt_link_set_info_kind(mnlxt_rt_link_t *rt_link, mnlxt_rt_link_info_kind_t info_kind) {
	mnlxt_rt_link_info_kind_t kind;
	int rc = mnlxt_rt_link_get_info_kind(rt_link, &kind);
	if (0 == rc && kind != info_kind) {
		errno = EINVAL;
		rc = -1;
	} else if (1 == rc) {
		rt_link->info.kind = info_kind;
		MNLXT_SET_PROP_FLAG(rt_link, MNLXT_RT_LINK_INFO);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_get_info_kind(const mnlxt_rt_link_t *rt_link, mnlxt_rt_link_info_kind_t *info_kind) {
	int rc = -1;
	if (NULL == rt_link || NULL == info_kind) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rt_link, MNLXT_RT_LINK_INFO)) {
		rc = 1;
	} else {
		*info_kind = rt_link->info.kind;
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_set_type(mnlxt_rt_link_t *rt_link, uint16_t type) {
	return mnlxt_rt_link_set_u16(rt_link, MNLXT_RT_LINK_TYPE, type);
}

int mnlxt_rt_link_get_type(const mnlxt_rt_link_t *rt_link, uint16_t *type) {
	return mnlxt_rt_link_get_u16(rt_link, MNLXT_RT_LINK_TYPE, type);
}

int mnlxt_rt_link_set_family(mnlxt_rt_link_t *rt_link, uint8_t family) {
	return mnlxt_rt_link_set_u8(rt_link, MNLXT_RT_LINK_FAMILY, family);
}

int mnlxt_rt_link_get_family(const mnlxt_rt_link_t *rt_link, uint8_t *family) {
	return mnlxt_rt_link_get_u8(rt_link, MNLXT_RT_LINK_FAMILY, family);
}

int mnlxt_rt_link_get_name(const mnlxt_rt_link_t *rt_link, mnlxt_if_name_t name) {
	int rc = -1;
	if (NULL == rt_link || NULL == name) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rt_link, MNLXT_RT_LINK_NAME)) {
		rc = 1;
	} else {
		strncpy(name, rt_link->name, sizeof(mnlxt_if_name_t));
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_set_name(mnlxt_rt_link_t *rt_link, const mnlxt_if_name_t name) {
	int rc = -1;
	size_t len;
	if (NULL == rt_link || NULL == name || sizeof(rt_link->name) <= (len = strlen(name))) {
		errno = EINVAL;
	} else {
		memcpy(rt_link->name, name, len + 1);
		MNLXT_SET_PROP_FLAG(rt_link, MNLXT_RT_LINK_NAME);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_get_index(const mnlxt_rt_link_t *rt_link, uint32_t *index) {
	return mnlxt_rt_link_get_u32(rt_link, MNLXT_RT_LINK_INDEX, index);
}

int mnlxt_rt_link_set_index(mnlxt_rt_link_t *rt_link, uint32_t index) {
	return mnlxt_rt_link_set_u32(rt_link, MNLXT_RT_LINK_INDEX, index);
}

int mnlxt_rt_link_get_parent(const mnlxt_rt_link_t *rt_link, uint32_t *if_index) {
	return mnlxt_rt_link_get_u32(rt_link, MNLXT_RT_LINK_PARENT, if_index);
}

int mnlxt_rt_link_set_parent(mnlxt_rt_link_t *rt_link, uint32_t if_index) {
	return mnlxt_rt_link_set_u32(rt_link, MNLXT_RT_LINK_PARENT, if_index);
}

int mnlxt_rt_link_get_flags(const mnlxt_rt_link_t *rt_link, uint32_t *flags) {
	return mnlxt_rt_link_get_u32(rt_link, MNLXT_RT_LINK_FLAGS, flags);
}

int mnlxt_rt_link_set_flags(mnlxt_rt_link_t *rt_link, uint32_t flags) {
	int rc = mnlxt_rt_link_set_u32(rt_link, MNLXT_RT_LINK_FLAGS, flags);
	if (0 == rc) {
		rt_link->flag_mask = (uint32_t)-1;
	}
	return rc;
}

static int mnlxt_rt_link_set_selected_flags(mnlxt_rt_link_t *rt_link, uint32_t flags, int on) {
	int rc = -1;
	if (NULL == rt_link || 0 == flags) {
		errno = EINVAL;
	} else {
		if (on) {
			rt_link->flags |= flags;
		} else {
			rt_link->flags &= ~flags;
		}
		rt_link->flag_mask |= flags;
		MNLXT_SET_PROP_FLAG(rt_link, MNLXT_RT_LINK_FLAGS);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_set_flags_on(mnlxt_rt_link_t *link, uint32_t flags) {
	return mnlxt_rt_link_set_selected_flags(link, flags, 1);
}

int mnlxt_rt_link_set_flags_off(mnlxt_rt_link_t *link, uint32_t flags) {
	return mnlxt_rt_link_set_selected_flags(link, flags, 0);
}
#if 0 /* not in use yet */
int mnlxt_rt_link_reset_flags(mnlxt_rt_link_t *link, uint32_t flags) {
	int rc = -1;
	if (NULL == link || 0 == flags) {
		errno = EINVAL;
	} else {
		link->flags &= ~flags;
		link->flag_mask &= ~flags;
		if ( 0 == link->flag_mask) {
			MNLXT_UNSET_PROP_FLAG(link, MNLXT_RT_LINK_FLAGS);
		}
	}
	return rc;
}
#endif

int mnlxt_rt_link_get_mtu(const mnlxt_rt_link_t *rt_link, uint32_t *mtu) {
	return mnlxt_rt_link_get_u32(rt_link, MNLXT_RT_LINK_MTU, mtu);
}

int mnlxt_rt_link_set_mtu(mnlxt_rt_link_t *rt_link, uint32_t mtu) {
	return mnlxt_rt_link_set_u32(rt_link, MNLXT_RT_LINK_MTU, mtu);
}

int mnlxt_rt_link_get_master(const mnlxt_rt_link_t *rt_link, uint32_t *if_index) {
	return mnlxt_rt_link_get_u32(rt_link, MNLXT_RT_LINK_MASTER, if_index);
}

int mnlxt_rt_link_set_master(mnlxt_rt_link_t *rt_link, uint32_t if_index) {
	return mnlxt_rt_link_set_u32(rt_link, MNLXT_RT_LINK_MASTER, if_index);
}

int mnlxt_rt_link_get_state(const mnlxt_rt_link_t *rt_link, uint8_t *state) {
	return mnlxt_rt_link_get_u8(rt_link, MNLXT_RT_LINK_STATE, state);
}

int mnlxt_rt_link_set_state(mnlxt_rt_link_t *rt_link, uint8_t state) {
	return mnlxt_rt_link_set_u8(rt_link, MNLXT_RT_LINK_STATE, state);
}

int mnlxt_rt_link_get_hwaddr(const mnlxt_rt_link_t *rt_link, mnlxt_eth_addr_t *mac) {
	return mnlxt_rt_link_get_ptr(rt_link, MNLXT_RT_LINK_HWADDR, mac, sizeof(mnlxt_eth_addr_t));
}

int mnlxt_rt_link_set_hwaddr(mnlxt_rt_link_t *rt_link, mnlxt_eth_addr_t mac) {
	return mnlxt_rt_link_set_ptr(rt_link, MNLXT_RT_LINK_HWADDR, mac, sizeof(mnlxt_eth_addr_t));
}

int mnlxt_rt_link_get_updown(const mnlxt_rt_link_t *link, int *up) {
	int rc = -1;
	if (NULL == up) {
		errno = EINVAL;
	} else {
		uint32_t flags = 0;
		rc = mnlxt_rt_link_get_flags(link, &flags);
		if (0 == rc) {
			if (flags & IFF_UP) {
				*up = 1;
			} else {
				*up = 0;
			}
		}
	}
	return rc;
}

int mnlxt_rt_link_set_updown(mnlxt_rt_link_t *link, int up) {
	return mnlxt_rt_link_set_selected_flags(link, IFF_UP, up);
}

int mnlxt_rt_link_set_vlan_id(mnlxt_rt_link_t *rt_link, uint16_t id) {
	int rc = -1;
	if (0 == mnlxt_rt_link_set_info_kind(rt_link, MNLXT_RT_LINK_INFO_KIND_VLAN)) {
		MNLXT_SET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_VLAN_ID);
		rt_link->info.data.vlan.id = id;
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_get_vlan_id(const mnlxt_rt_link_t *rt_link, uint16_t *id) {
	int rc = -1;
	if (NULL == rt_link || NULL == id) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rt_link, MNLXT_RT_LINK_INFO)) {
		rc = 1;
	} else if (MNLXT_RT_LINK_INFO_KIND_VLAN != rt_link->info.kind) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_VLAN_ID)) {
		rc = 1;
	} else {
		*id = rt_link->info.data.vlan.id;
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_set_xfrm_ifindex(mnlxt_rt_link_t *rt_link, uint32_t if_index) {
	int rc = -1;
	if (0 == mnlxt_rt_link_set_info_kind(rt_link, MNLXT_RT_LINK_INFO_KIND_XFRM)) {
		MNLXT_SET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_XFRM_IFINDEX);
		rt_link->info.data.xfrm.if_index = if_index;
		rc = 0;
		if (!MNLXT_GET_PROP_FLAG(rt_link, MNLXT_RT_LINK_PARENT)) {
			rc = mnlxt_rt_link_set_parent(rt_link, if_index);
		}
	}
	return rc;
}

int mnlxt_rt_link_get_xfrm_ifindex(const mnlxt_rt_link_t *rt_link, uint32_t *if_index) {
	int rc = -1;
	if (NULL == rt_link || NULL == if_index) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rt_link, MNLXT_RT_LINK_INFO)) {
		rc = 1;
	} else if (MNLXT_RT_LINK_INFO_KIND_XFRM != rt_link->info.kind) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_XFRM_IFINDEX)) {
		rc = 1;
	} else {
		*if_index = rt_link->info.data.xfrm.if_index;
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_set_xfrm_id(mnlxt_rt_link_t *rt_link, uint32_t id) {
	int rc = -1;
	if (0 == mnlxt_rt_link_set_info_kind(rt_link, MNLXT_RT_LINK_INFO_KIND_XFRM)) {
		MNLXT_SET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_XFRM_ID);
		rt_link->info.data.xfrm.id = id;
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_get_xfrm_id(const mnlxt_rt_link_t *rt_link, uint32_t *id) {
	int rc = -1;
	if (NULL == rt_link || NULL == id) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rt_link, MNLXT_RT_LINK_INFO)) {
		rc = 1;
	} else if (MNLXT_RT_LINK_INFO_KIND_XFRM != rt_link->info.kind) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_XFRM_ID)) {
		rc = 1;
	} else {
		*id = rt_link->info.data.xfrm.id;
		rc = 0;
	}
	return rc;
}
