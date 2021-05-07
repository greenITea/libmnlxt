/*
 * addr.c		Libmnlxt Routing Address
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

#include "libmnlxt/rt_addr.h"
#include "private/internal.h"

#define addr_ad_init(member) ad_init(mnlxt_rt_addr_t, member)

static struct access_data addr_data[MNLXT_RT_ADDR_MAX] = {
	[MNLXT_RT_ADDR_FAMILY] = addr_ad_init(family),
	[MNLXT_RT_ADDR_PREFIXLEN] = addr_ad_init(prefixlen),
	[MNLXT_RT_ADDR_FLAGS] = addr_ad_init(flags),
	[MNLXT_RT_ADDR_SCOPE] = addr_ad_init(scope),
	[MNLXT_RT_ADDR_IFINDEX] = addr_ad_init(if_index),
	[MNLXT_RT_ADDR_ADDR] = {},			// special case
	[MNLXT_RT_ADDR_LOCAL] = {},			// special case
	[MNLXT_RT_ADDR_LABEL] = {},			// special case
	[MNLXT_RT_ADDR_CACHEINFO] = {}, // special case
};

mnlxt_rt_addr_t *mnlxt_rt_addr_new() {
	mnlxt_rt_addr_t *addr = calloc(1, sizeof(mnlxt_rt_addr_t));
	if (addr) {
		addr->cacheinfo.ifa_prefered = MNLXT_RT_ADDR_LIFE_TIME_INFINITY;
		addr->cacheinfo.ifa_valid = MNLXT_RT_ADDR_LIFE_TIME_INFINITY;
	}
	return addr;
}

mnlxt_rt_addr_t *mnlxt_rt_addr_clone(const mnlxt_rt_addr_t *src, uint64_t filter) {
	mnlxt_rt_addr_t *dst = NULL;
	char *label = NULL;

	do {
		if (NULL == src) {
			errno = EINVAL;
			break;
		}

		uint64_t prop_flags = (uint64_t)src->prop_flags & filter;
		if ((prop_flags & MNLXT_FLAG(MNLXT_RT_ADDR_LABEL)) && NULL != src->label) {
			label = strdup(src->label);
			if (NULL == label)
				break;
		}

		dst = mnlxt_rt_addr_new();
		if (NULL == dst)
			break;

		if (prop_flags) {
			*dst = *src;
			dst->label = label;
			label = NULL;
			dst->prop_flags = prop_flags;
		}
	} while (0);

	if (NULL != label)
		free(label);

	return dst;
}

void mnlxt_rt_addr_free(mnlxt_rt_addr_t *addr) {
	if (NULL != addr) {
		if (NULL != addr->label) {
			free(addr->label);
		}
		free(addr);
	}
}

static int mnlxt_rt_addr_set_ptr(mnlxt_rt_addr_t *addr, mnlxt_rt_addr_data_t data, void *ptr, uint8_t size) {
	int rc = -1;
	if (NULL == addr || MNLXT_RT_ADDR_MAX <= (unsigned)data || addr_data[data].size != size
			|| 0 == addr_data[data].size) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(addr, data);
		memcpy(((char *)addr + addr_data[data].offset), ptr, size);
		rc = 0;
	}
	return rc;
}

static inline int mnlxt_rt_addr_set_u32(mnlxt_rt_addr_t *addr, mnlxt_rt_addr_data_t data, uint32_t u32) {
	return mnlxt_rt_addr_set_ptr(addr, data, &u32, sizeof(uint32_t));
}

static inline int mnlxt_rt_addr_set_u8(mnlxt_rt_addr_t *addr, mnlxt_rt_addr_data_t data, uint8_t u8) {
	return mnlxt_rt_addr_set_ptr(addr, data, &u8, sizeof(uint8_t));
}

static int mnlxt_rt_addr_get_ptr(const mnlxt_rt_addr_t *addr, mnlxt_rt_addr_data_t data, void *ptr, uint8_t size) {
	int rc = -1;
	if (NULL == addr || MNLXT_RT_ADDR_MAX <= (unsigned)data || addr_data[data].size != size
			|| 0 == addr_data[data].size) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(addr, data)) {
		rc = 1;
	} else {
		memcpy(ptr, ((char *)addr + addr_data[data].offset), size);
		rc = 0;
	}
	return rc;
}

static inline int mnlxt_rt_addr_get_u32(const mnlxt_rt_addr_t *addr, mnlxt_rt_addr_data_t data, uint32_t *pu32) {
	return mnlxt_rt_addr_get_ptr(addr, data, pu32, sizeof(uint32_t));
}

static inline int mnlxt_rt_addr_get_u8(const mnlxt_rt_addr_t *addr, mnlxt_rt_addr_data_t data, uint8_t *pu8) {
	return mnlxt_rt_addr_get_ptr(addr, data, pu8, sizeof(uint8_t));
}

int mnlxt_rt_addr_set_family(mnlxt_rt_addr_t *addr, uint8_t family) {
	int rc = -1;
	if (NULL == addr) {
		errno = EINVAL;
	} else if (AF_INET != family && AF_INET6 != family) {
		errno = EAFNOSUPPORT;
	} else if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_FAMILY)) {
		if (family == addr->family) {
			rc = 0;
		} else {
			errno = EINVAL;
		}
	} else {
		addr->family = family;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_FAMILY);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_addr_get_family(const mnlxt_rt_addr_t *addr, uint8_t *family) {
	return mnlxt_rt_addr_get_u8(addr, MNLXT_RT_ADDR_FAMILY, family);
}

int mnlxt_rt_addr_set_prefixlen(mnlxt_rt_addr_t *addr, uint8_t prefixlen) {
	return mnlxt_rt_addr_set_u8(addr, MNLXT_RT_ADDR_PREFIXLEN, prefixlen);
}

int mnlxt_rt_addr_get_prefixlen(const mnlxt_rt_addr_t *addr, uint8_t *prefixlen) {
	return mnlxt_rt_addr_get_u8(addr, MNLXT_RT_ADDR_PREFIXLEN, prefixlen);
}

int mnlxt_rt_addr_set_flags(mnlxt_rt_addr_t *addr, uint32_t flags) {
	return mnlxt_rt_addr_set_u32(addr, MNLXT_RT_ADDR_FLAGS, flags);
}

int mnlxt_rt_addr_get_flags(const mnlxt_rt_addr_t *addr, uint32_t *flags) {
	return mnlxt_rt_addr_get_u32(addr, MNLXT_RT_ADDR_FLAGS, flags);
}

int mnlxt_rt_addr_set_scope(mnlxt_rt_addr_t *addr, uint8_t scope) {
	return mnlxt_rt_addr_set_u8(addr, MNLXT_RT_ADDR_SCOPE, scope);
}

int mnlxt_rt_addr_get_scope(const mnlxt_rt_addr_t *addr, uint8_t *scope) {
	return mnlxt_rt_addr_get_u8(addr, MNLXT_RT_ADDR_SCOPE, scope);
}

int mnlxt_rt_addr_set_ifindex(mnlxt_rt_addr_t *addr, uint32_t if_index) {
	return mnlxt_rt_addr_set_u32(addr, MNLXT_RT_ADDR_IFINDEX, if_index);
}

int mnlxt_rt_addr_get_ifindex(const mnlxt_rt_addr_t *addr, uint32_t *if_index) {
	return mnlxt_rt_addr_get_u32(addr, MNLXT_RT_ADDR_IFINDEX, if_index);
}

int mnlxt_rt_addr_set_addr(mnlxt_rt_addr_t *addr, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (NULL == addr || NULL == buf) {
		errno = EINVAL;
	} else {
		rc = mnlxt_rt_addr_set_family(addr, family);
		if (0 == rc) {
			if (AF_INET == family) {
				if (!MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_LOCAL)) {
					MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_LOCAL);
					addr->addr_local = *buf;
				}
			}
			addr->addr = *buf;
			MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_ADDR);
		}
	}
	return rc;
}

int mnlxt_rt_addr_get_addr(const mnlxt_rt_addr_t *addr, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (NULL == addr || NULL == buf) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_ADDR)) {
		rc = 1;
	} else {
		*buf = &addr->addr;
		if (NULL != family) {
			*family = addr->family;
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_addr_set_local(mnlxt_rt_addr_t *addr, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = mnlxt_rt_addr_set_family(addr, family);
	if (0 == rc) {
		addr->addr_local = *buf;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_LOCAL);
	}
	return rc;
}

int mnlxt_rt_addr_get_local(const mnlxt_rt_addr_t *addr, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (NULL == addr || NULL == buf) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_LOCAL)) {
		rc = 1;
	} else {
		*buf = &addr->addr_local;
		if (NULL != family) {
			*family = addr->family;
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_addr_set_label(mnlxt_rt_addr_t *addr, const mnlxt_if_name_t label) {
	int rc = -1;
	char *label_copy;
	if (NULL == addr || NULL == label) {
		errno = EINVAL;
	} else if (NULL != (label_copy = strndup(label, sizeof(mnlxt_if_name_t) - 1))) {
		if (NULL != addr->label) {
			free(addr->label);
		}
		addr->label = label_copy;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_LABEL);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_addr_get_label(const mnlxt_rt_addr_t *addr, mnlxt_if_name_t label) {
	int rc = -1;
	if (NULL == addr || NULL == label) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_LABEL) || NULL == addr->label) {
		rc = 1;
	} else {
		strncpy(label, addr->label, sizeof(mnlxt_if_name_t));
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_addr_get_valid_lifetime(const mnlxt_rt_addr_t *addr, uint32_t *valid_lft) {
	int rc = -1;
	if (NULL == addr || NULL == valid_lft) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO)) {
		rc = 1;
	} else {
		*valid_lft = addr->cacheinfo.ifa_valid;
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_addr_set_valid_lifetime(mnlxt_rt_addr_t *addr, uint32_t valid_lft) {
	int rc = -1;
	if (NULL == addr) {
		errno = EINVAL;
	} else {
		addr->cacheinfo.ifa_valid = valid_lft;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_addr_get_preferred_lifetime(const mnlxt_rt_addr_t *addr, uint32_t *preferred_lft) {
	int rc = -1;
	if (NULL == addr || NULL == preferred_lft) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO)) {
		rc = 1;
	} else {
		*preferred_lft = addr->cacheinfo.ifa_prefered;
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_addr_set_preferred_lifetime(mnlxt_rt_addr_t *addr, uint32_t preferred_lft) {
	int rc = -1;
	if (NULL == addr) {
		errno = EINVAL;
	} else {
		addr->cacheinfo.ifa_prefered = preferred_lft;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_addr_get_create_time(const mnlxt_rt_addr_t *addr, uint32_t *create_time) {
	int rc = -1;
	if (NULL == addr || NULL == create_time) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO)) {
		rc = 1;
	} else {
		*create_time = addr->cacheinfo.cstamp;
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_addr_get_last_update_time(const mnlxt_rt_addr_t *addr, uint32_t *update_time) {
	int rc = -1;
	if (NULL == addr || NULL == update_time) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO)) {
		rc = 1;
	} else {
		*update_time = addr->cacheinfo.tstamp;
		rc = 0;
	}
	return rc;
}
