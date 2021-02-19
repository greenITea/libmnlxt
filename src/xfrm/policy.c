/*
 * policy.c		Libmnlxt Xfrm/IPsec Policy
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

#include "libmnlxt/xfrm_policy.h"
#include "private/internal.h"

#define policy_ad_init(member) ad_init(mnlxt_xfrm_policy_t, member)

static struct access_data policy_data[MNLXT_XFRM_POLICY_MAX] = {
	[MNLXT_XFRM_POLICY_FAMILY] = policy_ad_init(family),
	[MNLXT_XFRM_POLICY_PROTO] = policy_ad_init(proto),
	[MNLXT_XFRM_POLICY_SRC_PREFIXLEN] = policy_ad_init(src.prefixlen),
	[MNLXT_XFRM_POLICY_DST_PREFIXLEN] = policy_ad_init(dst.prefixlen),
	[MNLXT_XFRM_POLICY_SRC_ADDR] = {}, // special case
	[MNLXT_XFRM_POLICY_DST_ADDR] = {}, // special case
	[MNLXT_XFRM_POLICY_SRC_PORT] = policy_ad_init(src.port),
	[MNLXT_XFRM_POLICY_DST_PORT] = policy_ad_init(dst.port),
	[MNLXT_XFRM_POLICY_INDEX] = policy_ad_init(index),
	[MNLXT_XFRM_POLICY_IFINDEX] = policy_ad_init(if_index),
	[MNLXT_XFRM_POLICY_PRIO] = policy_ad_init(priority),
	[MNLXT_XFRM_POLICY_ACTION] = policy_ad_init(action),
	[MNLXT_XFRM_POLICY_DIR] = policy_ad_init(dir),
	[MNLXT_XFRM_POLICY_MARK] = policy_ad_init(mark),
	[MNLXT_XFRM_POLICY_TMPLS] = {}, // special case
};

mnlxt_xfrm_policy_t *mnlxt_xfrm_policy_new() {
	return calloc(1, sizeof(mnlxt_xfrm_policy_t));
}

mnlxt_xfrm_policy_t *mnlxt_xfrm_policy_clone(const mnlxt_xfrm_policy_t *src, uint64_t filter) {
	mnlxt_xfrm_policy_t *dst = NULL;
	do {
		if (NULL == src) {
			errno = EINVAL;
			break;
		}
		uint64_t prop_flags = (uint64_t)src->prop_flags & filter;
		mnlxt_xfrm_tmpl_t *tmpls = NULL;
		if ((prop_flags & MNLXT_FLAG(MNLXT_XFRM_POLICY_TMPLS)) && NULL != src->tmpls && src->tmpl_num) {
			if (NULL == (tmpls = calloc(src->tmpl_num, sizeof(mnlxt_xfrm_tmpl_t)))) {
				break;
			} else {
				memcpy(tmpls, src->tmpls, src->tmpl_num * sizeof(mnlxt_xfrm_tmpl_t));
			}
		}
		if (NULL == (dst = mnlxt_xfrm_policy_new())) {
			if (NULL != tmpls) {
				free(tmpls);
			}
			break;
		}
		if (prop_flags) {
			memcpy(dst, src, sizeof(mnlxt_xfrm_policy_t));
			dst->tmpls = tmpls;
			dst->prop_flags = prop_flags;
		}
	} while (0);
	return dst;
}

void mnlxt_xfrm_policy_free(mnlxt_xfrm_policy_t *policy) {
	if (NULL != policy) {
		if (NULL != policy->tmpls) {
			free(policy->tmpls);
		}
		free(policy);
	}
}

static int mnlxt_xfrm_policy_set_ptr(mnlxt_xfrm_policy_t *policy, mnlxt_xfrm_policy_data_t data, void *ptr,
																		 uint8_t size) {
	int rc = -1;
	if (NULL == policy || MNLXT_XFRM_POLICY_MAX <= (unsigned)data || policy_data[data].size != size
			|| 0 == policy_data[data].size) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(policy, data);
		memcpy(((char *)policy + policy_data[data].offset), ptr, size);
		rc = 0;
	}
	return rc;
}

static inline int mnlxt_xfrm_policy_set_u32(mnlxt_xfrm_policy_t *policy, mnlxt_xfrm_policy_data_t data, uint32_t u32) {
	return mnlxt_xfrm_policy_set_ptr(policy, data, &u32, sizeof(uint32_t));
}

static inline int mnlxt_xfrm_policy_set_u8(mnlxt_xfrm_policy_t *policy, mnlxt_xfrm_policy_data_t data, uint8_t u8) {
	return mnlxt_xfrm_policy_set_ptr(policy, data, &u8, sizeof(uint8_t));
}

static int mnlxt_xfrm_policy_get_ptr(const mnlxt_xfrm_policy_t *policy, mnlxt_xfrm_policy_data_t data, void *ptr,
																		 uint8_t size) {
	int rc = -1;
	if (NULL == policy || MNLXT_XFRM_POLICY_MAX <= (unsigned)data || policy_data[data].size != size
			|| 0 == policy_data[data].size) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(policy, data)) {
		rc = 1;
	} else {
		memcpy(ptr, ((char *)policy + policy_data[data].offset), size);
		rc = 0;
	}
	return rc;
}

static inline int mnlxt_xfrm_policy_get_u32(const mnlxt_xfrm_policy_t *policy, mnlxt_xfrm_policy_data_t data,
																						uint32_t *pu32) {
	return mnlxt_xfrm_policy_get_ptr(policy, data, pu32, sizeof(uint32_t));
}

static inline int mnlxt_xfrm_policy_get_u16(const mnlxt_xfrm_policy_t *policy, mnlxt_xfrm_policy_data_t data,
																						uint16_t *pu16) {
	return mnlxt_xfrm_policy_get_ptr(policy, data, pu16, sizeof(uint16_t));
}

static inline int mnlxt_xfrm_policy_get_u8(const mnlxt_xfrm_policy_t *policy, mnlxt_xfrm_policy_data_t data,
																					 uint8_t *pu8) {
	return mnlxt_xfrm_policy_get_ptr(policy, data, pu8, sizeof(uint8_t));
}

int mnlxt_xfrm_policy_set_family(mnlxt_xfrm_policy_t *policy, uint8_t family) {
	int rc = -1;
	if (NULL == policy) {
		errno = EINVAL;
	} else if (AF_INET != family && AF_INET6 != family) {
		errno = EAFNOSUPPORT;
	} else if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_FAMILY)) {
		if (family == policy->family) {
			rc = 0;
		} else {
			errno = EINVAL;
		}
	} else {
		policy->family = family;
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_FAMILY);
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_family(const mnlxt_xfrm_policy_t *policy, uint8_t *family) {
	return mnlxt_xfrm_policy_get_u8(policy, MNLXT_XFRM_POLICY_FAMILY, family);
}

static inline int mnlxt_port_proto(uint8_t proto) {
	return (IPPROTO_TCP == proto || IPPROTO_UDP == proto || IPPROTO_SCTP == proto || IPPROTO_DCCP == proto);
}

int mnlxt_xfrm_policy_set_proto(mnlxt_xfrm_policy_t *policy, uint8_t proto) {
	int rc = -1;
	if (NULL == policy) {
		errno = EINVAL;
	} else if ((MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_PORT)
							|| MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_SRC_PORT))
						 && !mnlxt_port_proto(proto)) {
		/* protocol with no ports */
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_PROTO);
		policy->proto = proto;
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_proto(const mnlxt_xfrm_policy_t *policy, uint8_t *proto) {
	return mnlxt_xfrm_policy_get_u8(policy, MNLXT_XFRM_POLICY_PROTO, proto);
}

int mnlxt_xfrm_policy_set_src_prefixlen(mnlxt_xfrm_policy_t *policy, uint8_t prefixlen) {
	return mnlxt_xfrm_policy_set_u8(policy, MNLXT_XFRM_POLICY_SRC_PREFIXLEN, prefixlen);
}

int mnlxt_xfrm_policy_get_src_prefixlen(const mnlxt_xfrm_policy_t *policy, uint8_t *prefixlen) {
	return mnlxt_xfrm_policy_get_u8(policy, MNLXT_XFRM_POLICY_SRC_PREFIXLEN, prefixlen);
}

int mnlxt_xfrm_policy_set_dst_prefixlen(mnlxt_xfrm_policy_t *policy, uint8_t prefixlen) {
	return mnlxt_xfrm_policy_set_u8(policy, MNLXT_XFRM_POLICY_DST_PREFIXLEN, prefixlen);
}

int mnlxt_xfrm_policy_get_dst_prefixlen(const mnlxt_xfrm_policy_t *policy, uint8_t *prefixlen) {
	return mnlxt_xfrm_policy_get_u8(policy, MNLXT_XFRM_POLICY_DST_PREFIXLEN, prefixlen);
}

int mnlxt_xfrm_policy_set_src_addr(mnlxt_xfrm_policy_t *policy, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (NULL == policy || NULL == buf) {
		errno = EINVAL;
	} else {
		rc = mnlxt_xfrm_policy_set_family(policy, family);
		if (0 == rc) {
			memcpy(&policy->src.addr, buf, sizeof(policy->src.addr));
			MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_SRC_ADDR);
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_get_src_addr(const mnlxt_xfrm_policy_t *policy, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (NULL == policy || NULL == buf) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_SRC_ADDR)) {
		rc = 1;
	} else {
		*buf = &policy->src.addr;
		if (NULL != family) {
			*family = policy->family;
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_set_dst_addr(mnlxt_xfrm_policy_t *policy, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (NULL == policy || NULL == buf) {
		errno = EINVAL;
	} else {
		rc = mnlxt_xfrm_policy_set_family(policy, family);
		if (0 == rc) {
			memcpy(&policy->dst.addr, buf, sizeof(mnlxt_inet_addr_t));
			MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_ADDR);
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_get_dst_addr(const mnlxt_xfrm_policy_t *policy, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (NULL == policy || NULL == buf) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_ADDR)) {
		rc = 1;
	} else {
		*buf = &policy->dst.addr;
		if (NULL != family) {
			*family = policy->family;
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_set_src_port(mnlxt_xfrm_policy_t *policy, uint16_t port) {
	int rc = -1;
	if (NULL == policy) {
		errno = EINVAL;
	} else if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_PROTO) && !mnlxt_port_proto(policy->proto)) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_SRC_PORT);
		policy->src.port = port;
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_src_port(const mnlxt_xfrm_policy_t *policy, uint16_t *port) {
	return mnlxt_xfrm_policy_get_u16(policy, MNLXT_XFRM_POLICY_SRC_PORT, port);
}

int mnlxt_xfrm_policy_set_dst_port(mnlxt_xfrm_policy_t *policy, uint16_t port) {
	int rc = -1;
	if (NULL == policy) {
		errno = EINVAL;
	} else if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_PROTO) && !mnlxt_port_proto(policy->proto)) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_PORT);
		policy->dst.port = port;
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_dst_port(const mnlxt_xfrm_policy_t *policy, uint16_t *port) {
	return mnlxt_xfrm_policy_get_u16(policy, MNLXT_XFRM_POLICY_DST_PORT, port);
}

int mnlxt_xfrm_policy_set_ifindex(mnlxt_xfrm_policy_t *policy, uint32_t ifindex) {
	return mnlxt_xfrm_policy_set_u32(policy, MNLXT_XFRM_POLICY_IFINDEX, ifindex);
}

int mnlxt_xfrm_policy_get_ifindex(const mnlxt_xfrm_policy_t *policy, uint32_t *ifindex) {
	return mnlxt_xfrm_policy_get_u32(policy, MNLXT_XFRM_POLICY_IFINDEX, ifindex);
}

int mnlxt_xfrm_policy_set_action(mnlxt_xfrm_policy_t *policy, uint8_t action) {
	int rc = -1;
	if (XFRM_POLICY_ALLOW != action && XFRM_POLICY_BLOCK != action) {
		errno = EINVAL;
	} else {
		rc = mnlxt_xfrm_policy_set_u8(policy, MNLXT_XFRM_POLICY_ACTION, action);
	}
	return rc;
}

int mnlxt_xfrm_policy_get_action(const mnlxt_xfrm_policy_t *policy, uint8_t *action) {
	return mnlxt_xfrm_policy_get_u8(policy, MNLXT_XFRM_POLICY_ACTION, action);
}

int mnlxt_xfrm_policy_set_dir(mnlxt_xfrm_policy_t *policy, uint8_t dir) {
	int rc = -1;
	if (XFRM_POLICY_MAX <= dir) {
		errno = EINVAL;
	} else {
		rc = mnlxt_xfrm_policy_set_u8(policy, MNLXT_XFRM_POLICY_DIR, dir);
	}
	return rc;
}

int mnlxt_xfrm_policy_get_dir(const mnlxt_xfrm_policy_t *policy, uint8_t *dir) {
	return mnlxt_xfrm_policy_get_u8(policy, MNLXT_XFRM_POLICY_DIR, dir);
}

int mnlxt_xfrm_policy_set_priority(mnlxt_xfrm_policy_t *policy, uint32_t priority) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_PRIO);
		policy->priority = priority;
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_priority(const mnlxt_xfrm_policy_t *policy, uint32_t *priority) {
	int rc = -1;
	if (!policy || !priority) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_PRIO)) {
			*priority = policy->priority;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_mark(mnlxt_xfrm_policy_t *policy, uint32_t mark, uint32_t mask) {
	int rc = mnlxt_xfrm_policy_set_u32(policy, MNLXT_XFRM_POLICY_MARK, mark);
	if (0 == rc) {
		policy->mark.mask = mask ? mask : (uint32_t)-1;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_mark(const mnlxt_xfrm_policy_t *policy, uint32_t *mark, uint32_t *mask) {
	uint32_t m;
	int rc = mnlxt_xfrm_policy_get_u32(policy, MNLXT_XFRM_POLICY_MARK, &m);
	if (0 == rc) {
		if (NULL != mark) {
			*mark = m;
		}
		if (NULL != mask) {
			*mask = policy->mark.mask;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_index(mnlxt_xfrm_policy_t *policy, uint32_t index) {
	return mnlxt_xfrm_policy_set_u32(policy, MNLXT_XFRM_POLICY_INDEX, index);
}

int mnlxt_xfrm_policy_get_index(const mnlxt_xfrm_policy_t *policy, uint32_t *index) {
	return mnlxt_xfrm_policy_get_u32(policy, MNLXT_XFRM_POLICY_INDEX, index);
}
