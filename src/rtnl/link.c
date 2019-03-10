/*
 * link.c		Libmnlxt Routing Link
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

#include <libmnlxt/rt_link.h>

#include "internal.h"

mnlxt_rt_link_t *mnlxt_rt_link_new() {
	return calloc(1, sizeof(mnlxt_rt_link_t));
}

void mnlxt_rt_link_free(mnlxt_rt_link_t *link) {
	if (link) {
		if (link->name) {
			free(link->name);
		}
		free(link);
	}
}

void mnlxt_rt_link_FREE(void *link) {
	mnlxt_rt_link_free((mnlxt_rt_link_t *)link);
}

void mnlxt_rt_link_data_revoke(mnlxt_rt_link_t *link, mnlxt_rt_link_data_t index) {
	if (link && MNLXT_RT_LINK_MAX > index) {
		MNLXT_UNSET_PROP_FLAG(link, index);
	}
}

int mnlxt_rt_link_set_info_kind(mnlxt_rt_link_t *link, mnlxt_rt_link_info_kind_t info_kind) {
	int rc = -1;
	if (link) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_INFO)) {
			if (link->info.kind == info_kind) {
				rc = 0;
			} else {
				errno = EINVAL;
			}
		} else {
			link->info.kind = info_kind;
			MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_INFO);
			/* TODO: create ext */
			rc = 0;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_info_kind(mnlxt_rt_link_t *link, mnlxt_rt_link_info_kind_t *info_kind) {
	int rc = -1;
	if (link && info_kind) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_INFO)) {
			*info_kind = link->info.kind;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_type(mnlxt_rt_link_t *link, uint16_t type) {
	int rc = -1;
	if (link) {
		link->type = type;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_TYPE);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_type(mnlxt_rt_link_t *link, uint16_t *type) {
	int rc = -1;
	if (link && type) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_TYPE)) {
			*type = link->type;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_family(mnlxt_rt_link_t *link, uint8_t family) {
	int rc = -1;
	if (link) {
		link->family = family;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_FAMILY);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_family(mnlxt_rt_link_t *link, uint8_t *family) {
	int rc = -1;
	if (link && family) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_FAMILY)) {
			*family = link->family;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_name(mnlxt_rt_link_t *link, const char **name) {
	int rc = -1;
	if (link && name) {
		if ((MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_NAME)) && link->name) {
			*name = link->name;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_name(mnlxt_rt_link_t *link, const char *name) {
	int rc = -1;
	if (link && name) {
		link->name = strdup(name);
		if (link->name) {
			rc = 0;
			MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_NAME);
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_index(mnlxt_rt_link_t *link, uint32_t *index) {
	int rc = -1;
	if (link && index) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_INDEX)) {
			*index = link->index;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_index(mnlxt_rt_link_t *link, uint32_t index) {
	int rc = -1;
	if (link) {
		link->index = index;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_INDEX);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_parent(mnlxt_rt_link_t *link, uint32_t *index) {
	int rc = -1;
	if (link && index) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_PARENT)) {
			*index = link->parent;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_parent(mnlxt_rt_link_t *link, uint32_t index) {
	int rc = -1;
	if (link) {
		link->parent = index;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_PARENT);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_flags(mnlxt_rt_link_t *link, uint32_t *flags) {
	int rc = -1;
	if (link && flags) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_FLAGS)) {
			*flags = link->flags;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_flags(mnlxt_rt_link_t *link, uint32_t flags) {
	int rc = -1;
	if (link) {
		link->flags = flags;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_FLAGS);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_mtu(mnlxt_rt_link_t *link, uint32_t *mtu) {
	int rc = -1;
	if (link && mtu) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_MTU)) {
			*mtu = link->mtu;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_mtu(mnlxt_rt_link_t *link, uint32_t mtu) {
	int rc = -1;
	if (link) {
		link->mtu = mtu;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_MTU);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_master(mnlxt_rt_link_t *link, uint32_t *master) {
	int rc = -1;
	if (link && master) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_MASTER)) {
			*master = link->master;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_master(mnlxt_rt_link_t *link, uint32_t master) {
	int rc = -1;
	if (link) {
		link->master = master;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_MASTER);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_state(mnlxt_rt_link_t *link, uint8_t *state) {
	int rc = -1;
	if (link && state) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_STATE)) {
			*state = link->state;
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_state(mnlxt_rt_link_t *link, uint8_t state) {
	int rc = -1;
	if (link) {
		link->state = state;
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_STATE);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_hwaddr(mnlxt_rt_link_t *link, eth_addr_t *mac) {
	int rc = -1;
	if (link && mac) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_HWADDR)) {
			memcpy(*mac, link->mac, sizeof(link->mac));
			rc = 0;
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_hwaddr(mnlxt_rt_link_t *link, eth_addr_t mac) {
	int rc = -1;
	if (link && mac) {
		memcpy(link->mac, mac, sizeof(link->mac));
		MNLXT_SET_PROP_FLAG(link, MNLXT_RT_LINK_HWADDR);
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_get_updown(mnlxt_rt_link_t *link, int *up) {
	int rc = -1;
	if (up) {
		uint32_t flags = 0;
		rc = mnlxt_rt_link_get_flags(link, &flags);
		if (0 == rc) {
			if (flags & IFF_UP) {
				*up = 1;
			} else {
				*up = 0;
			}
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_link_set_updown(mnlxt_rt_link_t *link, int up) {
	uint32_t flags = 0;
	int rc = mnlxt_rt_link_get_flags(link, &flags);
	if (-1 != rc) {
		if (up) {
			flags |= IFF_UP;
		} else {
			flags &= ~IFF_UP;
		}
		rc = mnlxt_rt_link_set_flags(link, flags);
		link->flag_mask |= IFF_UP;
	}
	return rc;
}

int mnlxt_rt_link_set_vlan_id(mnlxt_rt_link_t *link, uint16_t id) {
	int rc = -1;
	if (-1 != mnlxt_rt_link_set_info_kind(link, MNLXT_RT_LINK_INFO_KIND_VLAN)) {
		link->info.prop_flags |= 0x1 << MNLXT_RT_LINK_VLAN_ID;
		link->info.data.vlan.id = id;
		rc = 0;
	}
	return rc;
}

int mnlxt_rt_link_get_vlan_id(mnlxt_rt_link_t *link, uint16_t *id) {
	int rc = -1;
	if (link && id) {
		if (MNLXT_GET_PROP_FLAG(link, MNLXT_RT_LINK_INFO)) {
			if (MNLXT_RT_LINK_INFO_KIND_VLAN == link->info.kind && (link->info.prop_flags & (0x1 << MNLXT_RT_LINK_VLAN_ID))) {
				*id = link->info.data.vlan.id;
				rc = 0;
			} else {
				errno = EINVAL;
			}
		} else {
			rc = 1;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}
