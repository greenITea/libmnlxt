/*
 * link_xfrm.c		Libmnlxt Routing Link XFRM
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>

#include "libmnlxt/rt_link_xfrm.h"
#include "private/internal.h"

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
