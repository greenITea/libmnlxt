/*
 * link_vlan.c		Libmnlxt Routing Link VLAN
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>

#include "libmnlxt/rt_link_vlan.h"
#include "private/internal.h"

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
