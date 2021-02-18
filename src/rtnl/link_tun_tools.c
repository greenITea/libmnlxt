/*
 * link_tun_tools.c		Libmnlxt Routing Link, TUN/TAP tools
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "libmnlxt/rt_link_tun.h"

static int mnlxt_tun_open(const char *name, uint32_t flags) {
	struct ifreq ifr = {};
	int fd = -1;

	if (NULL == name || 0 == name[0]) {
		errno = EINVAL;
		goto err;
	}

	if (0 > (fd = open("/dev/net/tun", O_RDWR))) {
		goto err;
	}

	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name) - 1);
	ifr.ifr_flags = flags;

	if (0 > ioctl(fd, TUNSETIFF, (void *)&ifr)) {
		goto err;
		close(fd);
		fd = -1;
	}
err:
	return fd;
}

int mnlxt_tun_create(const char *name, uid_t uid, gid_t gid, uint32_t flags) {
	struct ifreq ifr = {};
	int fd = -1, rc = -1;

#ifdef IFF_TUN_EXCL
	ifr.ifr_flags |= IFF_TUN_EXCL;
#endif

	if (0 > (fd = mnlxt_tun_open(name, flags))) {
		goto err;
	}

	if (uid != -1 && 0 > ioctl(fd, TUNSETOWNER, uid)) {
		goto err;
	}

	if (gid != -1 && 0 > ioctl(fd, TUNSETGROUP, gid)) {
		goto err;
	}

	if ((flags & IFF_PERSIST) && 0 > ioctl(fd, TUNSETPERSIST, 1)) {
		goto err;
	}

	rc = fd;
err:
	if (0 > rc && 0 <= fd) {
		close(fd);
	}
	return rc;
}

int mnlxt_tun_delete(const char *name, uint8_t type) {
	int rc = -1;
	int fd = mnlxt_tun_open(name, type);
	if (0 <= fd) {
		if (0 <= ioctl(fd, TUNSETPERSIST, 0)) {
			rc = 0;
		}
		close(fd);
	}
	return rc;
}

int mnlxt_rt_link_tun_create(mnlxt_rt_link_t *rt_link) {
	int rc = -1;
	uint8_t type;
	uint16_t tun_flags;
	uid_t uid = -1;
	gid_t gid = -1;
	uint32_t flags = IFF_PERSIST;
	int fd;
	mnlxt_if_name_t name;

	if (0 != mnlxt_rt_link_get_tun_type(rt_link, &type) || 0 != mnlxt_rt_link_get_name(rt_link, name)) {
		errno = EINVAL;
		goto err;
	}
	mnlxt_rt_link_get_tun_gid(rt_link, &gid);
	mnlxt_rt_link_get_tun_uid(rt_link, &uid);
	mnlxt_rt_link_get_tun_flags(rt_link, &tun_flags);
	if (!(tun_flags & MNLXT_RT_LINK_TUN_FLAG_PI)) {
		flags |= IFF_NO_PI;
	}
	if (tun_flags & MNLXT_RT_LINK_TUN_FLAG_VNET_HDR) {
		flags |= IFF_VNET_HDR;
	}
	if (tun_flags & MNLXT_RT_LINK_TUN_FLAG_MULTI_QUEUE) {
		flags |= IFF_MULTI_QUEUE;
	}
	flags |= type;
	fd = mnlxt_tun_create(name, uid, gid, flags);
	if (0 > fd) {
		goto err;
	} else {
		/* close TUN-dvice descriptor */
		close(fd);
		/* add device index */
		uint32_t if_index = if_nametoindex(name);
		if (0 == if_index) {
			goto err;
		}
		mnlxt_rt_link_set_index(rt_link, if_index);
	}
	rc = 0;
err:
	return rc;
}

int mnlxt_rt_link_tun_delete(mnlxt_rt_link_t *rt_link) {
	uint8_t type;
	int rc = -1;
	mnlxt_if_name_t name;
	if (0 != mnlxt_rt_link_get_tun_type(rt_link, &type)) {
		errno = EINVAL;
		goto err;
	}

	if (0 != mnlxt_rt_link_get_name(rt_link, name)) {
		/* no device name is available, try to get it from link index */
		uint32_t if_index;
		if (0 != mnlxt_rt_link_get_index(rt_link, &if_index)) {
			errno = EINVAL;
			goto err;
		}
		if (NULL == if_indextoname(if_index, name)) {
			goto err;
		}
	}
	rc = mnlxt_tun_delete(name, type);
err:
	return rc;
}
