/*
 * rule.c		Libmnlxt Routing Rules
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

#include "libmnlxt/rt_rule.h"
#include "private/internal.h"

#include <linux/if.h>

#define rule_ad_init(member) ad_init(mnlxt_rt_rule_t, member)

static struct access_data rule_data[MNLXT_RT_RULE_MAX] = {
	[MNLXT_RT_RULE_FAMILY] = rule_ad_init(family),
	[MNLXT_RT_RULE_SRC_PREFIX] = rule_ad_init(src_prefix),
	[MNLXT_RT_RULE_DST_PREFIX] = rule_ad_init(dst_prefix),
	[MNLXT_RT_RULE_SRC] = {}, // special case
	[MNLXT_RT_RULE_DST] = {}, // special case
	[MNLXT_RT_RULE_TABLE] = rule_ad_init(table),
	[MNLXT_RT_RULE_IIFNAME] = {}, // special case
	[MNLXT_RT_RULE_OIFNAME] = {}, // special case
	[MNLXT_RT_RULE_TOS] = rule_ad_init(tos),
	[MNLXT_RT_RULE_ACTION] = rule_ad_init(action),
	[MNLXT_RT_RULE_PRIORITY] = rule_ad_init(priority),
	[MNLXT_RT_RULE_FWMARK] = rule_ad_init(fwmark),
	[MNLXT_RT_RULE_FWMASK] = rule_ad_init(fwmask),
};

mnlxt_rt_rule_t *mnlxt_rt_rule_new() {
	return calloc(1, sizeof(mnlxt_rt_rule_t));
}

mnlxt_rt_rule_t *mnlxt_rt_rule_clone(const mnlxt_rt_rule_t *src, uint64_t filter) {
	mnlxt_rt_rule_t *dst = NULL;
	if (NULL == src) {
		errno = EINVAL;
	} else if (NULL != (dst = mnlxt_rt_rule_new()) && filter) {
		memcpy(dst, src, sizeof(mnlxt_rt_rule_t));
		dst->prop_flags = src->prop_flags & filter;
	}
	return dst;
}

void mnlxt_rt_rule_free(mnlxt_rt_rule_t *rule) {
	if (NULL != rule) {
		if (NULL != rule->iif_name) {
			free(rule->iif_name);
		}
		if (NULL != rule->oif_name) {
			free(rule->oif_name);
		}
		free(rule);
	}
}

static int mnlxt_rt_rule_set_ptr(mnlxt_rt_rule_t *rule, mnlxt_rt_rule_data_t data, void *ptr, uint8_t size) {
	int rc = -1;
	if (NULL == rule || MNLXT_RT_RULE_MAX <= (unsigned)data || rule_data[data].size != size
			|| 0 == rule_data[data].size) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(rule, data);
		memcpy(((char *)rule + rule_data[data].offset), ptr, size);
		rc = 0;
	}
	return rc;
}

static inline int mnlxt_rt_rule_set_u32(mnlxt_rt_rule_t *rule, mnlxt_rt_rule_data_t data, uint32_t u32) {
	return mnlxt_rt_rule_set_ptr(rule, data, &u32, sizeof(uint32_t));
}

static inline int mnlxt_rt_rule_set_u8(mnlxt_rt_rule_t *rule, mnlxt_rt_rule_data_t data, uint8_t u8) {
	return mnlxt_rt_rule_set_ptr(rule, data, &u8, sizeof(uint8_t));
}

static int mnlxt_rt_rule_get_ptr(const mnlxt_rt_rule_t *rule, mnlxt_rt_rule_data_t data, void *ptr, uint8_t size) {
	int rc = -1;
	if (NULL == rule || MNLXT_RT_RULE_MAX <= (unsigned)data || rule_data[data].size != size
			|| 0 == rule_data[data].size) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rule, data)) {
		rc = 1;
	} else {
		memcpy(ptr, ((char *)rule + rule_data[data].offset), size);
		rc = 0;
	}
	return rc;
}

static inline int mnlxt_rt_rule_get_u32(const mnlxt_rt_rule_t *rule, mnlxt_rt_rule_data_t data, uint32_t *pu32) {
	return mnlxt_rt_rule_get_ptr(rule, data, pu32, sizeof(uint32_t));
}

static inline int mnlxt_rt_rule_get_u8(const mnlxt_rt_rule_t *rule, mnlxt_rt_rule_data_t data, uint8_t *pu8) {
	return mnlxt_rt_rule_get_ptr(rule, data, pu8, sizeof(uint8_t));
}

int mnlxt_rt_rule_set_family(mnlxt_rt_rule_t *rule, uint8_t family) {
	int rc = -1;
	if (NULL == rule) {
		errno = EINVAL;
	} else if (AF_INET != family && AF_INET6 != family) {
		errno = EAFNOSUPPORT;
	} else if (MNLXT_GET_PROP_FLAG(rule, MNLXT_RT_RULE_FAMILY)) {
		if (family == rule->family) {
			rc = 0;
		} else {
			errno = EINVAL;
		}
	} else {
		rule->family = family;
		MNLXT_SET_PROP_FLAG(rule, MNLXT_RT_RULE_FAMILY);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_rule_get_family(const mnlxt_rt_rule_t *rule, uint8_t *family) {
	return mnlxt_rt_rule_get_u8(rule, MNLXT_RT_RULE_FAMILY, family);
}

int mnlxt_rt_rule_set_src_prefix(mnlxt_rt_rule_t *rule, uint8_t prefixlen) {
	return mnlxt_rt_rule_set_u8(rule, MNLXT_RT_RULE_SRC_PREFIX, prefixlen);
}

int mnlxt_rt_rule_get_src_prefix(const mnlxt_rt_rule_t *rule, uint8_t *prefixlen) {
	return mnlxt_rt_rule_get_u8(rule, MNLXT_RT_RULE_SRC_PREFIX, prefixlen);
}

int mnlxt_rt_rule_set_dst_prefix(mnlxt_rt_rule_t *rule, uint8_t prefixlen) {
	return mnlxt_rt_rule_set_u8(rule, MNLXT_RT_RULE_DST_PREFIX, prefixlen);
}

int mnlxt_rt_rule_get_dst_prefix(const mnlxt_rt_rule_t *rule, uint8_t *prefixlen) {
	return mnlxt_rt_rule_get_u8(rule, MNLXT_RT_RULE_DST_PREFIX, prefixlen);
}

int mnlxt_rt_rule_set_src(mnlxt_rt_rule_t *rule, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (NULL == rule || NULL == buf) {
		errno = EINVAL;
	} else {
		rc = mnlxt_rt_rule_set_family(rule, family);
		if (0 == rc) {
			memcpy(&rule->src, buf, (family == AF_INET6 ? sizeof(struct in6_addr) : sizeof(struct in_addr)));
			MNLXT_SET_PROP_FLAG(rule, MNLXT_RT_RULE_SRC);
		}
	}
	return rc;
}

int mnlxt_rt_rule_get_src(const mnlxt_rt_rule_t *rule, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (NULL == rule || NULL == buf) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rule, MNLXT_RT_RULE_SRC)) {
		rc = 1;
	} else {
		*buf = &rule->src;
		if (NULL != family) {
			*family = rule->family;
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_rule_set_dst(mnlxt_rt_rule_t *rule, uint8_t family, const mnlxt_inet_addr_t *buf) {
	int rc = -1;
	if (NULL == rule || NULL == buf) {
		errno = EINVAL;
	} else {
		rc = mnlxt_rt_rule_set_family(rule, family);
		if (0 == rc) {
			memcpy(&rule->dst, buf, (family == AF_INET6 ? sizeof(struct in6_addr) : sizeof(struct in_addr)));
			MNLXT_SET_PROP_FLAG(rule, MNLXT_RT_RULE_DST);
		}
	}
	return rc;
}

int mnlxt_rt_rule_get_dst(const mnlxt_rt_rule_t *rule, uint8_t *family, const mnlxt_inet_addr_t **buf) {
	int rc = -1;
	if (NULL == rule || NULL == buf) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rule, MNLXT_RT_RULE_DST)) {
		rc = 1;
	} else {
		*buf = &rule->dst;
		if (NULL != family) {
			*family = rule->family;
		}
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_rule_set_table(mnlxt_rt_rule_t *rule, uint8_t table) {
	return mnlxt_rt_rule_set_u8(rule, MNLXT_RT_RULE_TABLE, table);
}

int mnlxt_rt_rule_get_table(const mnlxt_rt_rule_t *rule, uint8_t *table) {
	return mnlxt_rt_rule_get_u8(rule, MNLXT_RT_RULE_TABLE, table);
}

int mnlxt_rt_rule_set_action(mnlxt_rt_rule_t *rule, uint8_t action) {
	return mnlxt_rt_rule_set_u8(rule, MNLXT_RT_RULE_ACTION, action);
}

int mnlxt_rt_rule_get_action(const mnlxt_rt_rule_t *rule, uint8_t *action) {
	return mnlxt_rt_rule_get_u8(rule, MNLXT_RT_RULE_ACTION, action);
}

int mnlxt_rt_rule_set_tos(mnlxt_rt_rule_t *rule, uint8_t tos) {
	return mnlxt_rt_rule_set_u8(rule, MNLXT_RT_RULE_TOS, tos);
}

int mnlxt_rt_rule_get_tos(const mnlxt_rt_rule_t *rule, uint8_t *tos) {
	return mnlxt_rt_rule_get_u8(rule, MNLXT_RT_RULE_TOS, tos);
}

int mnlxt_rt_rule_set_priority(mnlxt_rt_rule_t *rule, uint32_t priority) {
	return mnlxt_rt_rule_set_u32(rule, MNLXT_RT_RULE_PRIORITY, priority);
}

int mnlxt_rt_rule_get_priority(const mnlxt_rt_rule_t *rule, uint32_t *priority) {
	return mnlxt_rt_rule_get_u32(rule, MNLXT_RT_RULE_PRIORITY, priority);
}

int mnlxt_rt_rule_set_iif_name(mnlxt_rt_rule_t *rule, const mnlxt_if_name_t iif_name) {
	int rc = -1;
	char *iface = NULL;
	if (NULL == rule || NULL == iif_name) {
		errno = EINVAL;
	} else if (NULL != (iface = strndup(iif_name, sizeof(mnlxt_if_name_t) - 1))) {
		if (NULL != rule->iif_name) {
			free(rule->iif_name);
		}
		rule->iif_name = iface;
		MNLXT_SET_PROP_FLAG(rule, MNLXT_RT_RULE_IIFNAME);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_rule_get_iif_name(const mnlxt_rt_rule_t *rule, mnlxt_if_name_t iif_name) {
	int rc = -1;
	if (NULL == rule || NULL == iif_name) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rule, MNLXT_RT_RULE_IIFNAME) || NULL == rule->iif_name) {
		rc = 1;
	} else {
		strncpy(iif_name, rule->iif_name, sizeof(mnlxt_if_name_t));
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_rule_set_oif_name(mnlxt_rt_rule_t *rule, const mnlxt_if_name_t oif_name) {
	int rc = -1;
	char *iface = NULL;
	if (NULL == rule || NULL == oif_name) {
		errno = EINVAL;
	} else if (NULL != (iface = strndup(oif_name, sizeof(mnlxt_if_name_t) - 1))) {
		if (NULL != rule->oif_name) {
			free(rule->oif_name);
		}
		rule->oif_name = iface;
		MNLXT_SET_PROP_FLAG(rule, MNLXT_RT_RULE_OIFNAME);
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_rule_get_oif_name(const mnlxt_rt_rule_t *rule, mnlxt_if_name_t oif_name) {
	int rc = -1;
	if (NULL == rule || NULL == oif_name) {
		errno = EINVAL;
	} else if (!MNLXT_GET_PROP_FLAG(rule, MNLXT_RT_RULE_OIFNAME) || NULL == rule->oif_name) {
		rc = 1;
	} else {
		strncpy(oif_name, rule->oif_name, sizeof(mnlxt_if_name_t));
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_rule_set_fwmark(mnlxt_rt_rule_t *rule, uint32_t fwmark) {
	return mnlxt_rt_rule_set_u32(rule, MNLXT_RT_RULE_FWMARK, fwmark);
}

int mnlxt_rt_rule_get_fwmark(const mnlxt_rt_rule_t *rule, uint32_t *fwmark) {
	return mnlxt_rt_rule_get_u32(rule, MNLXT_RT_RULE_FWMARK, fwmark);
}

int mnlxt_rt_rule_set_fwmask(mnlxt_rt_rule_t *rule, uint32_t fwmask) {
	return mnlxt_rt_rule_set_u32(rule, MNLXT_RT_RULE_FWMASK, fwmask);
}

int mnlxt_rt_rule_get_fwmask(const mnlxt_rt_rule_t *rule, uint32_t *fwmask) {
	return mnlxt_rt_rule_get_u32(rule, MNLXT_RT_RULE_FWMASK, fwmask);
}
