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

void mnlxt_rt_link_free(mnlxt_rt_link_t *link) {
	if (link) {
		free(link);
	}
}

void mnlxt_rt_link_FREE(void *link) {
	mnlxt_rt_link_free((mnlxt_rt_link_t *)link);
}

void mnlxt_rt_link_data_revoke(mnlxt_rt_link_t *link, mnlxt_rt_link_data_t index) {
	if (link && MNLXT_RT_LINK_MAX > index) {
		MNLXT_UNSET_PROP_FLAG(link, index);
	}
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

int mnlxt_rt_link_set_type(mnlxt_rt_link_t *link, uint16_t type) {
	int rc = -1;
	if (link) {
		link->type = type;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_TYPE);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_type(const mnlxt_rt_link_t *link, uint16_t *type) {
	int rc = -1;
	if (link && type) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_TYPE)) {
			*type = link->type;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_family(mnlxt_rt_link_t *link, uint8_t family) {
	int rc = -1;
	if (link) {
		link->family = family;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_FAMILY);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_family(const mnlxt_rt_link_t *link, uint8_t *family) {
	int rc = -1;
	if (link && family) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_FAMILY)) {
			*family = link->family;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_name(const mnlxt_rt_link_t *link, const char **name) {
	int rc = -1;
	if (link && name) {
		if ((MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_NAME))) {
			*name = link->name;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_name(mnlxt_rt_link_t *link, const char *name) {
	int rc = -1;
	if (link && name) {
		size_t len = strlen(name);
		if (len < sizeof(link->name)) {
			memcpy(link->name, name, len + 1);
			MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_NAME);
			rc = 0;
		} else {
			errno = EINVAL;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_index(const mnlxt_rt_link_t *link, uint32_t *index) {
	int rc = -1;
	if (link && index) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_INDEX)) {
			*index = link->index;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_index(mnlxt_rt_link_t *link, uint32_t index) {
	int rc = -1;
	if (link) {
		link->index = index;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_INDEX);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_parent(const mnlxt_rt_link_t *link, uint32_t *index) {
	int rc = -1;
	if (link && index) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_PARENT)) {
			*index = link->parent;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_parent(mnlxt_rt_link_t *link, uint32_t index) {
	int rc = -1;
	if (link) {
		link->parent = index;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_PARENT);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_flags(const mnlxt_rt_link_t *link, uint32_t *flags) {
	int rc = -1;
	if (link && flags) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_FLAGS)) {
			*flags = link->flags;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_flags(mnlxt_rt_link_t *link, uint32_t flags) {
	int rc = -1;
	if (link) {
		link->flags = flags;
		link->flag_mask = (uint32_t)-1;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_FLAGS);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

static int mnlxt_rt_link_set_selected_flags(mnlxt_rt_link_t *link, uint32_t flags, int on) {
	int rc = -1;
	if (NULL == link || 0 == flags) {
		errno = EINVAL;
	} else {
		if (on) {
			link->flags |= flags;
		} else {
			link->flags &= ~flags;
		}
		link->flag_mask |= flags;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_FLAGS);
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
int mnlxt_rt_link_get_mtu(const mnlxt_rt_link_t *link, uint32_t *mtu) {
	int rc = -1;
	if (link && mtu) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_MTU)) {
			*mtu = link->mtu;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_mtu(mnlxt_rt_link_t *link, uint32_t mtu) {
	int rc = -1;
	if (link) {
		link->mtu = mtu;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_MTU);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_master(const mnlxt_rt_link_t *link, uint32_t *master) {
	int rc = -1;
	if (link && master) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_MASTER)) {
			*master = link->master;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_master(mnlxt_rt_link_t *link, uint32_t master) {
	int rc = -1;
	if (link) {
		link->master = master;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_MASTER);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_state(const mnlxt_rt_link_t *link, uint8_t *state) {
	int rc = -1;
	if (link && state) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_STATE)) {
			*state = link->state;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_state(mnlxt_rt_link_t *link, uint8_t state) {
	int rc = -1;
	if (link) {
		link->state = state;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_STATE);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_hwaddr(const mnlxt_rt_link_t *link, eth_addr_t *mac) {
	int rc = -1;
	if (link && mac) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_HWADDR)) {
			memcpy(*mac, link->mac, sizeof(link->mac));
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_hwaddr(mnlxt_rt_link_t *link, eth_addr_t mac) {
	int rc = -1;
	if (link && mac) {
		memcpy(link->mac, mac, sizeof(link->mac));
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_HWADDR);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_updown(const mnlxt_rt_link_t *link, int *up) {
	int rc = -1;
	if (up) {
		uint32_t flags = 0;
		rc = mnlxt_rt_link_get_flags(link, &flags);
		if (0 == rc) {
			if (flags & IFF_UP) {
				*up = 1;
			} else {
				*up = 0;
			}
		}
	} else {
		errno = EINVAL;
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
