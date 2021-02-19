/*
 * link_tun.c		Libmnlxt Routing Link, TUN/TAP devices
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>
#include <linux/if_tun.h>
#include <stdlib.h>

#include "libmnlxt/rt_link_tun.h"
#include "private/internal.h"

static int mnlxt_rt_link_check_tun_kind(const mnlxt_rt_link_t *rt_link) {
	mnlxt_rt_link_info_kind_t info_kind = -1;
	int rc = mnlxt_rt_link_get_info_kind(rt_link, &info_kind);

	if (0 == rc && MNLXT_RT_LINK_INFO_KIND_TUN != info_kind) {
		errno = EINVAL;
		rc = -1;
	}
	return rc;
}

int mnlxt_rt_link_get_tun_type(const mnlxt_rt_link_t *rt_link, uint8_t *type) {
	int rc = -1;
	if (0 == (rc = mnlxt_rt_link_check_tun_kind(rt_link))) {
		if (!MNLXT_GET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_TUN_TYPE)) {
			rc = 1;
		} else {
			*type = rt_link->info.data.tun.type;
			rc = 0;
		}
	}
	return rc;
}

int mnlxt_rt_link_set_tun_type(mnlxt_rt_link_t *rt_link, uint8_t type) {
	int rc = -1;
	if ((type & IFF_TUN) && (type & IFF_TAP)) {
		errno = EINVAL;
	} else if (0 == mnlxt_rt_link_set_info_kind(rt_link, MNLXT_RT_LINK_INFO_KIND_TUN)) {
		rt_link->info.data.tun.type = type;
		if (0 == type) {
			MNLXT_UNSET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_TUN_TYPE);
		} else {
			MNLXT_SET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_TUN_TYPE);
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_get_tun_flags(const mnlxt_rt_link_t *rt_link, uint16_t *flags) {
	int rc = -1;
	if (0 == (rc = mnlxt_rt_link_check_tun_kind(rt_link))) {
		if (!MNLXT_GET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_TUN_FLAGS)) {
			rc = 1;
		} else {
			*flags = rt_link->info.data.tun.flags;
			rc = 0;
		}
	}
	return rc;
}

int mnlxt_rt_link_set_tun_flags(mnlxt_rt_link_t *rt_link, uint16_t flags) {
	int rc = -1;
	if (0 == mnlxt_rt_link_set_info_kind(rt_link, MNLXT_RT_LINK_INFO_KIND_TUN)) {
		rt_link->info.data.tun.flags = flags;
		if (0 == flags) {
			MNLXT_UNSET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_TUN_FLAGS);
		} else {
			MNLXT_SET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_TUN_FLAGS);
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_get_tun_uid(const mnlxt_rt_link_t *rt_link, uid_t *uid) {
	int rc = -1;
	if (0 == (rc = mnlxt_rt_link_check_tun_kind(rt_link))) {
		if (!MNLXT_GET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_TUN_UID)) {
			rc = 1;
		} else {
			*uid = rt_link->info.data.tun.uid;
			rc = 0;
		}
	}
	return rc;
}

int mnlxt_rt_link_set_tun_uid(mnlxt_rt_link_t *rt_link, uid_t uid) {
	int rc = -1;
	if (0 == mnlxt_rt_link_set_info_kind(rt_link, MNLXT_RT_LINK_INFO_KIND_TUN)) {
		rt_link->info.data.tun.uid = uid;
		MNLXT_SET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_TUN_UID);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_get_tun_gid(const mnlxt_rt_link_t *rt_link, gid_t *gid) {
	int rc = -1;
	if (0 == (rc = mnlxt_rt_link_check_tun_kind(rt_link))) {
		if (!MNLXT_GET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_TUN_GID)) {
			rc = 1;
		} else {
			*gid = rt_link->info.data.tun.gid;
			rc = 0;
		}
	}
	return rc;
}

int mnlxt_rt_link_set_tun_gid(mnlxt_rt_link_t *rt_link, gid_t gid) {
	int rc = -1;
	if (0 == mnlxt_rt_link_set_info_kind(rt_link, MNLXT_RT_LINK_INFO_KIND_TUN)) {
		rt_link->info.data.tun.gid = gid;
		MNLXT_SET_PROP_FLAG((&rt_link->info), MNLXT_RT_LINK_TUN_GID);
		rc = 0;
	}
	return rc;
}
