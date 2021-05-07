/*
 * route.c		Libmnlxt Routing Routes
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

#include "libmnlxt/rt_route.h"
#include "private/internal.h"

#define route_ad_init(member) ad_init(mnlxt_rt_route_t, member)

static struct access_data route_data[MNLXT_RT_ROUTE_MAX] = {
	[MNLXT_RT_ROUTE_FAMILY] = route_ad_init(family),
	[MNLXT_RT_ROUTE_TYPE] = route_ad_init(type),
	[MNLXT_RT_ROUTE_TABLE] = route_ad_init(table),
	[MNLXT_RT_ROUTE_PROTOCOL] = route_ad_init(protocol),
	[MNLXT_RT_ROUTE_SCOPE] = route_ad_init(scope),
	[MNLXT_RT_ROUTE_PRIORITY] = route_ad_init(priority),
	[MNLXT_RT_ROUTE_IIFINDEX] = route_ad_init(iif_index),
	[MNLXT_RT_ROUTE_OIFINDEX] = route_ad_init(oif_index),
	[MNLXT_RT_ROUTE_SRC_PREFIX] = route_ad_init(src_prefix),
	[MNLXT_RT_ROUTE_DST_PREFIX] = route_ad_init(dst_prefix),
	[MNLXT_RT_ROUTE_SRC] = {},		 // special case
	[MNLXT_RT_ROUTE_DST] = {},		 // special case
	[MNLXT_RT_ROUTE_GATEWAY] = {}, // special case
};

mnlxt_rt_route_t *mnlxt_rt_route_new() {
	return calloc(1, sizeof(mnlxt_rt_route_t));
}

mnlxt_rt_route_t *mnlxt_rt_route_clone(const mnlxt_rt_route_t *src, uint64_t filter) {
	mnlxt_rt_route_t *dst = NULL;
	if (NULL == src) {
		errno = EINVAL;
	} else if (NULL != (dst = mnlxt_rt_route_new()) && filter) {
		*dst = *src;
		dst->prop_flags = src->prop_flags & filter;
	}
	return dst;
}

void mnlxt_rt_route_free(mnlxt_rt_route_t *route) {
	if (NULL != route) {
		free(route);
	}
}

static int mnlxt_rt_route_set_ptr(mnlxt_rt_route_t *route, mnlxt_rt_route_data_t data, void *ptr, uint8_t size) {
	int rc = -1;
	if (NULL == route || MNLXT_RT_ROUTE_MAX <= (unsigned)data || route_data[data].size != size
			|| 0 == route_data[data].size) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(route, data);
		memcpy(((char *)route + route_data[data].offset), ptr, size);
		rc = 0;
	}
	return rc;
}

static inline int mnlxt_rt_route_set_u32(mnlxt_rt_route_t *route, mnlxt_rt_route_data_t data, uint32_t u32) {
	return mnlxt_rt_route_set_ptr(route, data, &u32, sizeof(uint32_t));
}

static inline int mnlxt_rt_route_set_u8(mnlxt_rt_route_t *route, mnlxt_rt_route_data_t data, uint8_t u8) {
	return mnlxt_rt_route_set_ptr(route, data, &u8, sizeof(uint8_t));
}

static int mnlxt_rt_route_get_ptr(const mnlxt_rt_route_t *route, mnlxt_rt_route_data_t data, void *ptr, uint8_t size) {
	int rc = -1;
	if (NULL == route || MNLXT_RT_ROUTE_MAX <= (unsigned)data || route_data[data].size != size
			|| 0 == route_data[data].size) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(route, data)) {
		rc = 1;
	} else {
		memcpy(ptr, ((char *)route + route_data[data].offset), size);
		rc = 0;
	}
	return rc;
}

static inline int mnlxt_rt_route_get_u32(const mnlxt_rt_route_t *route, mnlxt_rt_route_data_t data, uint32_t *pu32) {
	return mnlxt_rt_route_get_ptr(route, data, pu32, sizeof(uint32_t));
}

static inline int mnlxt_rt_route_get_u8(const mnlxt_rt_route_t *route, mnlxt_rt_route_data_t data, uint8_t *pu8) {
	return mnlxt_rt_route_get_ptr(route, data, pu8, sizeof(uint8_t));
}

int mnlxt_rt_route_set_family(mnlxt_rt_route_t *route, uint8_t family) {
	int rc = -1;
	if (NULL == route) {
		errno = EINVAL;
	} else {
		if (AF_INET == family || AF_INET6 == family) {
			if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_FAMILY)) {
				if (family == route->family) {
					rc = 0;
				}
			} else {
				route->family = family;
				MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_FAMILY);
				rc = 0;
			}
		}
	}
	return rc;
}

int mnlxt_rt_route_get_family(const mnlxt_rt_route_t *route, uint8_t *family) {
	return mnlxt_rt_route_get_u8(route, MNLXT_RT_ROUTE_FAMILY, family);
}

int mnlxt_rt_route_set_protocol(mnlxt_rt_route_t *route, uint8_t protocol) {
	return mnlxt_rt_route_set_u8(route, MNLXT_RT_ROUTE_PROTOCOL, protocol);
}

int mnlxt_rt_route_get_protocol(const mnlxt_rt_route_t *route, uint8_t *protocol) {
	return mnlxt_rt_route_get_u8(route, MNLXT_RT_ROUTE_PROTOCOL, protocol);
}

int mnlxt_rt_route_set_table(mnlxt_rt_route_t *route, uint8_t table) {
	return mnlxt_rt_route_set_u8(route, MNLXT_RT_ROUTE_TABLE, table);
}

int mnlxt_rt_route_get_table(const mnlxt_rt_route_t *route, uint8_t *table) {
	return mnlxt_rt_route_get_u8(route, MNLXT_RT_ROUTE_TABLE, table);
}

int mnlxt_rt_route_set_scope(mnlxt_rt_route_t *route, uint8_t scope) {
	return mnlxt_rt_route_set_u8(route, MNLXT_RT_ROUTE_SCOPE, scope);
}

int mnlxt_rt_route_get_scope(const mnlxt_rt_route_t *route, uint8_t *scope) {
	return mnlxt_rt_route_get_u8(route, MNLXT_RT_ROUTE_SCOPE, scope);
}

int mnlxt_rt_route_set_type(mnlxt_rt_route_t *route, uint8_t type) {
	return mnlxt_rt_route_set_u8(route, MNLXT_RT_ROUTE_TYPE, type);
}

int mnlxt_rt_route_get_type(const mnlxt_rt_route_t *route, uint8_t *type) {
	return mnlxt_rt_route_get_u8(route, MNLXT_RT_ROUTE_TYPE, type);
}

int mnlxt_rt_route_set_src_prefix(mnlxt_rt_route_t *route, uint8_t prefixlen) {
	return mnlxt_rt_route_set_u8(route, MNLXT_RT_ROUTE_SRC_PREFIX, prefixlen);
}

int mnlxt_rt_route_get_src_prefix(const mnlxt_rt_route_t *route, uint8_t *prefixlen) {
	return mnlxt_rt_route_get_u8(route, MNLXT_RT_ROUTE_SRC_PREFIX, prefixlen);
}

int mnlxt_rt_route_set_dst_prefix(mnlxt_rt_route_t *route, uint8_t prefixlen) {
	return mnlxt_rt_route_set_u8(route, MNLXT_RT_ROUTE_DST_PREFIX, prefixlen);
}

int mnlxt_rt_route_get_dst_prefix(const mnlxt_rt_route_t *route, uint8_t *prefixlen) {
	return mnlxt_rt_route_get_u8(route, MNLXT_RT_ROUTE_DST_PREFIX, prefixlen);
}

int mnlxt_rt_route_set_src(mnlxt_rt_route_t *route, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (NULL == route || NULL == buf) {
		errno = EINVAL;
	} else {
		rc = mnlxt_rt_route_set_family(route, family);
		if (0 == rc) {
			route->src = *buf;
			MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_SRC);
		}
	}
	return rc;
}

int mnlxt_rt_route_get_src(const mnlxt_rt_route_t *route, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (NULL == route || NULL == buf) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_SRC)) {
		rc = 1;
	} else {
		*buf = &route->src;
		if (NULL != family) {
			*family = route->family;
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_route_set_dst(mnlxt_rt_route_t *route, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (NULL == route || NULL == buf) {
		errno = EINVAL;
	} else {
		rc = mnlxt_rt_route_set_family(route, family);
		if (0 == rc) {
			if ((AF_INET == family && INADDR_ANY != buf->in.s_addr)
					|| (AF_INET6 == family && !IN6_IS_ADDR_UNSPECIFIED(&buf->in6))) {
				/* set only if destination is not the default route, else it have to be not set */
				route->dst = *buf;
				MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_DST);
			}
		}
	}
	return rc;
}

int mnlxt_rt_route_get_dst(const mnlxt_rt_route_t *route, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (NULL == route || NULL == buf) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_DST)) {
		rc = 1;
	} else {
		*buf = &route->dst;
		if (NULL != family) {
			*family = route->family;
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_route_set_gateway(mnlxt_rt_route_t *route, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (NULL == route || NULL == buf) {
		errno = EINVAL;
	} else {
		rc = mnlxt_rt_route_set_family(route, family);
		if (0 == rc) {
			route->gateway = *buf;
			MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_GATEWAY);
		}
	}
	return rc;
}

int mnlxt_rt_route_get_gateway(const mnlxt_rt_route_t *route, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (NULL == route || NULL == buf) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_GATEWAY)) {
		rc = 1;
	} else {
		*buf = &route->gateway;
		if (NULL != family) {
			*family = route->family;
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_route_set_priority(mnlxt_rt_route_t *route, uint32_t priority) {
	return mnlxt_rt_route_set_u32(route, MNLXT_RT_ROUTE_PRIORITY, priority);
}

int mnlxt_rt_route_get_priority(const mnlxt_rt_route_t *route, uint32_t *priority) {
	return mnlxt_rt_route_get_u32(route, MNLXT_RT_ROUTE_PRIORITY, priority);
}

int mnlxt_rt_route_set_oifindex(mnlxt_rt_route_t *route, uint32_t if_index) {
	return mnlxt_rt_route_set_u32(route, MNLXT_RT_ROUTE_OIFINDEX, if_index);
}

int mnlxt_rt_route_get_oifindex(const mnlxt_rt_route_t *route, uint32_t *if_index) {
	return mnlxt_rt_route_get_u32(route, MNLXT_RT_ROUTE_OIFINDEX, if_index);
}

int mnlxt_rt_route_set_iifindex(mnlxt_rt_route_t *route, uint32_t if_index) {
	return mnlxt_rt_route_set_u32(route, MNLXT_RT_ROUTE_IIFINDEX, if_index);
}

int mnlxt_rt_route_get_iifindex(const mnlxt_rt_route_t *route, uint32_t *if_index) {
	return mnlxt_rt_route_get_u32(route, MNLXT_RT_ROUTE_IIFINDEX, if_index);
}
