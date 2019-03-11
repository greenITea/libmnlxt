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

#include <libmnlxt/rt_route.h>

#include "internal.h"

mnlxt_rt_route_t *mnlxt_rt_route_new() {
	return calloc(1, sizeof(mnlxt_rt_route_t));
}

void mnlxt_rt_route_free(mnlxt_rt_route_t *route) {
	if (route) {
		free(route);
	}
}

void mnlxt_rt_route_FREE(void *route) {
	mnlxt_rt_route_free((mnlxt_rt_route_t *)route);
}

int mnlxt_rt_route_set_family(mnlxt_rt_route_t *route, uint8_t family) {
	int rc = -1;
	if (route) {
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
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_family(const mnlxt_rt_route_t *route, uint8_t *family) {
	int rc = -1;
	if (route && family) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_FAMILY)) {
			*family = route->family;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_protocol(mnlxt_rt_route_t *route, uint8_t protocol) {
	int rc = -1;
	if (route) {
		route->protocol = protocol;
		MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_PROTOCOL);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_protocol(const mnlxt_rt_route_t *route, uint8_t *protocol) {
	int rc = -1;
	if (route && protocol) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_PROTOCOL)) {
			*protocol = route->protocol;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_table(mnlxt_rt_route_t *route, uint8_t table) {
	int rc = -1;
	if (route) {
		route->table = table;
		MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_TABLE);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_table(const mnlxt_rt_route_t *route, uint8_t *table) {
	int rc = -1;
	if (route && table) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_TABLE)) {
			*table = route->table;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_scope(mnlxt_rt_route_t *route, uint8_t scope) {
	int rc = -1;
	if (route) {
		route->scope = scope;
		MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_SCOPE);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_scope(const mnlxt_rt_route_t *route, uint8_t *scope) {
	int rc = -1;
	if (route && scope) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_SCOPE)) {
			*scope = route->scope;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_type(mnlxt_rt_route_t *route, uint8_t type) {
	int rc = -1;
	if (route) {
		route->type = type;
		MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_TYPE);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_route_get_type(const mnlxt_rt_route_t *route, uint8_t *type) {
	int rc = -1;
	if (route && type) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_TYPE)) {
			*type = route->type;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_src_prefix(mnlxt_rt_route_t *route, uint8_t prefixlen) {
	int rc = -1;
	if (route) {
		route->src_prefix = prefixlen;
		MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_SRC_PREFIX);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_src_prefix(const mnlxt_rt_route_t *route, uint8_t *prefixlen) {
	int rc = -1;
	if (route && prefixlen) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_SRC_PREFIX)) {
			*prefixlen = route->src_prefix;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_dst_prefix(mnlxt_rt_route_t *route, uint8_t prefixlen) {
	int rc = -1;
	if (route) {
		route->dst_prefix = prefixlen;
		MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_DST_PREFIX);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_dst_prefix(const mnlxt_rt_route_t *route, uint8_t *prefixlen) {
	int rc = -1;
	if (route && prefixlen) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_DST_PREFIX)) {
			*prefixlen = route->dst_prefix;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_src(mnlxt_rt_route_t *route, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (route && buf) {
		rc = mnlxt_rt_route_set_family(route, family);
		if (0 == rc) {
			memcpy(&route->src, buf, (family == AF_INET6 ? sizeof(struct in6_addr) : sizeof(struct in_addr)));
			MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_SRC);
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_src(const mnlxt_rt_route_t *route, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (route && buf) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_SRC)) {
			*buf = &route->src;
			if (family) {
				*family = route->family;
			}
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_dst(mnlxt_rt_route_t *route, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (route && buf) {
		rc = mnlxt_rt_route_set_family(route, family);
		if (0 == rc) {
			if ((AF_INET == family && INADDR_ANY != buf->in.s_addr)
					|| (AF_INET6 == family && !IN6_IS_ADDR_UNSPECIFIED(&buf->in6))) {
				/* set only if destination is not the default route, else it have to be not set */
				memcpy(&route->dst, buf, (family == AF_INET6 ? sizeof(struct in6_addr) : sizeof(struct in_addr)));
				MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_DST);
			}
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_dst(const mnlxt_rt_route_t *route, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (route && buf) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_DST)) {
			*buf = &route->dst;
			if (family) {
				*family = route->family;
			}
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_gateway(mnlxt_rt_route_t *route, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (route && buf) {
		rc = mnlxt_rt_route_set_family(route, family);
		if (0 == rc) {
			memcpy(&route->gateway, buf, (family == AF_INET6 ? sizeof(struct in6_addr) : sizeof(struct in_addr)));
			MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_GATEWAY);
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_gateway(const mnlxt_rt_route_t *route, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (route && buf) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_GATEWAY)) {
			*buf = &route->gateway;
			if (family) {
				*family = route->family;
			}
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_rt_route_set_priority(mnlxt_rt_route_t *route, uint32_t priority) {
	int rc = -1;
	if (route) {
		route->priority = priority;
		MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_PRIORITY);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_priority(const mnlxt_rt_route_t *route, uint32_t *priority) {
	int rc = -1;
	if (route && priority) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_PRIORITY)) {
			*priority = route->priority;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_oifindex(mnlxt_rt_route_t *route, uint32_t if_index) {
	int rc = -1;
	if (route) {
		route->oif_index = if_index;
		MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_OIFINDEX);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_oifindex(const mnlxt_rt_route_t *route, uint32_t *if_index) {
	int rc = -1;
	if (route && if_index) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_OIFINDEX)) {
			*if_index = route->oif_index;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_set_iifindex(mnlxt_rt_route_t *route, uint32_t if_index) {
	int rc = -1;
	if (route) {
		route->iif_index = if_index;
		MNLXT_SET_PROP_FLAG(route, MNLXT_RT_ROUTE_IIFINDEX);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_route_get_iifindex(const mnlxt_rt_route_t *route, uint32_t *if_index) {
	int rc = -1;
	if (route && if_index) {
		if (MNLXT_GET_PROP_FLAG(route, MNLXT_RT_ROUTE_IIFINDEX)) {
			*if_index = route->iif_index;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}
