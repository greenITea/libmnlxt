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
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <libmnlxt/xfrm_policy.h>

#include "internal.h"

mnlxt_xfrm_policy_t *mnlxt_xfrm_policy_new() {
	return calloc(1, sizeof(mnlxt_xfrm_policy_t));
}

void mnlxt_xfrm_policy_free(mnlxt_xfrm_policy_t *policy) {
	if (policy) {
		if (policy->tmpls) {
			free(policy->tmpls);
		}
		free(policy);
	}
}

void mnlxt_xfrm_policy_FREE(void *policy) {
	mnlxt_xfrm_policy_free((mnlxt_xfrm_policy_t *)policy);
}

int mnlxt_xfrm_policy_set_family(mnlxt_xfrm_policy_t *policy, uint8_t family) {
	int rc = -1;
	if (policy) {
		if (AF_INET == family || AF_INET6 == family) {
			if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_FAMILY)) {
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
		} else {
			errno = EAFNOSUPPORT;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_family(mnlxt_xfrm_policy_t *policy, uint8_t *family) {
	int rc = -1;
	if (!policy || !family) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_FAMILY)) {
			*family = policy->family;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

static int mnlxt_port_proto(uint8_t proto) {
	return (IPPROTO_TCP == proto || IPPROTO_UDP == proto || IPPROTO_SCTP == proto || IPPROTO_DCCP == proto);
}

int mnlxt_xfrm_policy_set_proto(mnlxt_xfrm_policy_t *policy, uint8_t proto) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		if ((MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_PORT)
				 || MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_SRC_PORT))
				&& !mnlxt_port_proto(proto)) {
			errno = EINVAL;
		} else {
			MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_PROTO);
			policy->proto = proto;
			rc = 0;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_get_proto(mnlxt_xfrm_policy_t *policy, uint8_t *proto) {
	int rc = -1;
	if (!policy || !proto) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_PROTO)) {
			*proto = policy->proto;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_src_prefixlen(mnlxt_xfrm_policy_t *policy, uint8_t prefixlen) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_SRC_PREFIXLEN);
		policy->src.prefixlen = prefixlen;
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_src_prefixlen(mnlxt_xfrm_policy_t *policy, uint8_t *prefixlen) {
	int rc = -1;
	if (!policy || !prefixlen) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_SRC_PREFIXLEN)) {
			*prefixlen = policy->src.prefixlen;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_dst_prefixlen(mnlxt_xfrm_policy_t *policy, uint8_t prefixlen) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_PREFIXLEN);
		policy->dst.prefixlen = prefixlen;
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_dst_prefixlen(mnlxt_xfrm_policy_t *policy, uint8_t *prefixlen) {
	int rc = -1;
	if (!policy || !prefixlen) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_PREFIXLEN)) {
			*prefixlen = policy->dst.prefixlen;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_src_addr(mnlxt_xfrm_policy_t *policy, uint8_t family, inet_addr_t *buf) {
	int rc = -1;
	if (!policy || !buf) {
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

int mnlxt_xfrm_policy_get_src_addr(mnlxt_xfrm_policy_t *policy, uint8_t *family, inet_addr_t **buf) {
	int rc = -1;
	if (!policy || !buf) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_SRC_ADDR)) {
			*buf = &policy->src.addr;
			if (family) {
				*family = policy->family;
			}
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_dst_addr(mnlxt_xfrm_policy_t *policy, uint8_t family, inet_addr_t *buf) {
	int rc = -1;
	if (!policy || !buf) {
		errno = EINVAL;
	} else {
		rc = mnlxt_xfrm_policy_set_family(policy, family);
		if (0 == rc) {
			memcpy(&policy->dst.addr, buf, sizeof(inet_addr_t));
			MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_ADDR);
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_get_dst_addr(mnlxt_xfrm_policy_t *policy, uint8_t *family, inet_addr_t **buf) {
	int rc = -1;
	if (!policy || !buf) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_ADDR)) {
			*buf = &policy->dst.addr;
			if (family) {
				*family = policy->family;
			}
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_src_port(mnlxt_xfrm_policy_t *policy, uint16_t port) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_PROTO) && !mnlxt_port_proto(policy->proto)) {
			errno = EINVAL;
		} else {
			MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_SRC_PORT);
			policy->src.port = port;
			rc = 0;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_get_src_port(mnlxt_xfrm_policy_t *policy, uint16_t *port) {
	int rc = -1;
	if (!policy || !port) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_SRC_PORT)) {
			*port = policy->src.port;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_dst_port(mnlxt_xfrm_policy_t *policy, uint16_t port) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_PROTO) && !mnlxt_port_proto(policy->proto)) {
			errno = EINVAL;
		} else {
			MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_PORT);
			policy->dst.port = port;
			rc = 0;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_get_dst_port(mnlxt_xfrm_policy_t *policy, uint16_t *port) {
	int rc = -1;
	if (!policy || !port) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DST_PORT)) {
			*port = policy->dst.port;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_ifindex(mnlxt_xfrm_policy_t *policy, uint32_t ifindex) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		policy->if_index = ifindex;
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_IFINDEX);
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_ifindex(mnlxt_xfrm_policy_t *policy, uint32_t *ifindex) {
	int rc = -1;
	if (!policy || !ifindex) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_IFINDEX)) {
			*ifindex = policy->if_index;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_action(mnlxt_xfrm_policy_t *policy, uint8_t action) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		if (XFRM_POLICY_ALLOW != action && XFRM_POLICY_BLOCK != action) {
			errno = EINVAL;
		}
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_ACTION);
		policy->action = action;
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_action(mnlxt_xfrm_policy_t *policy, uint8_t *action) {
	int rc = -1;
	if (!policy || !action) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_ACTION)) {
			*action = policy->action;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_dir(mnlxt_xfrm_policy_t *policy, uint8_t dir) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		if (XFRM_POLICY_MAX <= dir) {
			errno = EINVAL;
		} else {
			MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DIR);
			policy->dir = dir;
			rc = 0;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_get_dir(mnlxt_xfrm_policy_t *policy, uint8_t *dir) {
	int rc = -1;
	if (!policy || !dir) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_DIR)) {
			*dir = policy->dir;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
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

int mnlxt_xfrm_policy_get_priority(mnlxt_xfrm_policy_t *policy, uint32_t *priority) {
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
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_MARK);
		policy->mark.value = mark;
		policy->mark.mask = mask ? mask : (uint32_t)-1;
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_mark(mnlxt_xfrm_policy_t *policy, uint32_t *mark, uint32_t *mask) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_MARK)) {
			if (mark) {
				*mark = policy->mark.value;
			}
			if (mask) {
				*mask = policy->mark.mask;
			}
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_set_index(mnlxt_xfrm_policy_t *policy, uint32_t index) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_INDEX);
		policy->index = index;
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_index(mnlxt_xfrm_policy_t *policy, uint32_t *index) {
	int rc = -1;
	if (!policy || !index) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_INDEX)) {
			*index = policy->index;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}
#if 0
int mnlxt_xfrm_policy_set_(mnlxt_xfrm_policy_t *policy,  ) {
	int rc = -1;
	if (!policy) {
		errno = EINVAL;
	} else {
		MNLXT_SET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_);
		policy-> = ;
		rc = 0;
	}
	return rc;
}

int mnlxt_xfrm_policy_get_(mnlxt_xfrm_policy_t *policy,  *) {
	int rc = -1;
	if (!policy || !) {
		errno = EINVAL;
	} else {
		if (MNLXT_GET_PROP_FLAG(policy, MNLXT_XFRM_POLICY_)) {
			* = policy->;
			rc = 0;
		} else {
			rc = 1;
		}
	}
	return rc;
}
#endif
