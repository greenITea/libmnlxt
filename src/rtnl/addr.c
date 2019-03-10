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

#include <libmnlxt/rt_addr.h>

#include "internal.h"

mnlxt_rt_addr_t *mnlxt_rt_addr_new() {
	mnlxt_rt_addr_t *addr = calloc(1, sizeof(mnlxt_rt_addr_t));
	if (addr) {
		addr->cacheinfo.ifa_prefered = MNLXT_RT_ADDR_LIFE_TIME_INFINITY;
		addr->cacheinfo.ifa_valid = MNLXT_RT_ADDR_LIFE_TIME_INFINITY;
	}
	return addr;
}

void mnlxt_rt_addr_free(mnlxt_rt_addr_t *addr) {
	if (addr) {
		if (addr->label) {
			free(addr->label);
		}
		free(addr);
	}
}

void mnlxt_rt_addr_FREE(void *addr) {
	mnlxt_rt_addr_free((mnlxt_rt_addr_t *)addr);
}

int mnlxt_rt_addr_set_family(mnlxt_rt_addr_t *addr, uint8_t family) {
	int rc = -1;
	if (addr) {
		if (AF_INET == family || AF_INET6 == family) {
			if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_FAMILY)) {
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
		} else {
			errno = EAFNOSUPPORT;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_family(mnlxt_rt_addr_t *addr, uint8_t *family) {
	int rc = -1;
	if (addr && family) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_FAMILY)) {
			*family = addr->family;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_set_prefixlen(mnlxt_rt_addr_t *addr, uint8_t prefixlen) {
	int rc = -1;
	if (addr) {
		addr->prefixlen = prefixlen;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_PREFIXLEN);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_prefixlen(mnlxt_rt_addr_t *addr, uint8_t *prefixlen) {
	int rc = -1;
	if (addr && prefixlen) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_PREFIXLEN)) {
			*prefixlen = addr->prefixlen;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_set_flags(mnlxt_rt_addr_t *addr, uint32_t flags) {
	int rc = -1;
	if (addr) {
		addr->flags = flags;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_FLAGS);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_flags(mnlxt_rt_addr_t *addr, uint32_t *flags) {
	int rc = -1;
	if (addr && flags) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_FLAGS)) {
			*flags = addr->flags;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_set_scope(mnlxt_rt_addr_t *addr, uint8_t scope) {
	int rc = -1;
	if (addr) {
		addr->scope = scope;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_SCOPE);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_scope(mnlxt_rt_addr_t *addr, uint8_t *scope) {
	int rc = -1;
	if (addr && scope) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_SCOPE)) {
			*scope = addr->scope;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_set_ifindex(mnlxt_rt_addr_t *addr, uint32_t if_index) {
	int rc = -1;
	if (addr) {
		addr->if_index = if_index;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_IFINDEX);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_ifindex(mnlxt_rt_addr_t *addr, uint32_t *if_index) {
	int rc = -1;
	if (addr && if_index) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_IFINDEX)) {
			*if_index = addr->if_index;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_set_addr(mnlxt_rt_addr_t *addr, uint8_t family, inet_addr_t *buf) {
	int rc = -1;
	if (addr && buf) {
		rc = mnlxt_rt_addr_set_family(addr, family);
		if (0 == rc) {
			size_t len = sizeof(addr->addr);
			if (AF_INET == family) {
				len = sizeof(struct in_addr);
				if (!MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_LOCAL)) {
					MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_LOCAL);
					memcpy(&addr->addr_local, buf, len);
				}
			}
			memcpy(&addr->addr, buf, len);
			MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_ADDR);
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_addr(mnlxt_rt_addr_t *addr, uint8_t *family, inet_addr_t **buf) {
	int rc = -1;
	if (addr && buf) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_ADDR)) {
			*buf = &addr->addr;
			if (family) {
				*family = addr->family;
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

int mnlxt_rt_addr_set_local(mnlxt_rt_addr_t *addr, uint8_t family, inet_addr_t *buf) {
	int rc = mnlxt_rt_addr_set_family(addr, family);
	if (0 == rc) {
		memcpy(&addr->addr_local, buf, (family == AF_INET6 ? sizeof(struct in6_addr) : sizeof(struct in_addr)));
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_LOCAL);
	}
	return rc;
}

int mnlxt_rt_addr_get_local(mnlxt_rt_addr_t *addr, uint8_t *family, inet_addr_t **buf) {
	int rc = -1;
	if (addr && buf) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_LOCAL)) {
			*buf = &addr->addr_local;
			if (family) {
				*family = addr->family;
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

int mnlxt_rt_addr_set_label(mnlxt_rt_addr_t *addr, const char *label) {
	int rc = -1;
	if (addr && label) {
		if (addr->label) {
			free(addr->label);
		}
		addr->label = strdup(label);
		if (addr->label) {
			MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_LABEL);
			rc = 0;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_label(mnlxt_rt_addr_t *addr, char **label) {
	int rc = -1;
	if (addr && label) {
		if (addr->label) {
			*label = addr->label;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_valid_lifetime(mnlxt_rt_addr_t *addr, uint32_t *valid_lft) {
	int rc = -1;
	if (addr && valid_lft) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO)) {
			*valid_lft = addr->cacheinfo.ifa_valid;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_set_valid_lifetime(mnlxt_rt_addr_t *addr, uint32_t valid_lft) {
	int rc = -1;
	if (addr) {
		addr->cacheinfo.ifa_valid = valid_lft;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_preferred_lifetime(mnlxt_rt_addr_t *addr, uint32_t *preferred_lft) {
	int rc = -1;
	if (addr && preferred_lft) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO)) {
			*preferred_lft = addr->cacheinfo.ifa_prefered;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_set_preferred_lifetime(mnlxt_rt_addr_t *addr, uint32_t preferred_lft) {
	int rc = -1;
	if (addr) {
		addr->cacheinfo.ifa_prefered = preferred_lft;
		MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_create_time(mnlxt_rt_addr_t *addr, uint32_t *create_time) {
	int rc = -1;
	if (addr && create_time) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO)) {
			*create_time = addr->cacheinfo.cstamp;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_get_last_update_time(mnlxt_rt_addr_t *addr, uint32_t *update_time) {
	int rc = -1;
	if (addr && update_time) {
		if (MNLXT_GET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO)) {
			*update_time = addr->cacheinfo.tstamp;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}
