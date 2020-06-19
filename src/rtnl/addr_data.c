/*
 * addr_data.c		Libmnlxt Routing Address
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>
#include <string.h>

#include <libmnlxt/rt.h>

#include "internal.h"

#include "config.h"

static int mnlxt_rt_addr_cmp(const mnlxt_rt_addr_t *rt_addr1, const mnlxt_rt_addr_t *rt_addr2,
														 mnlxt_rt_addr_data_t data) {
	int rc = data + 1;
	size_t addr_size;
	switch (data) {
	case MNLXT_RT_ADDR_FAMILY:
		if (rt_addr1->family != rt_addr2->family) {
			goto failed;
		}
		break;
	case MNLXT_RT_ADDR_PREFIXLEN:
		if (rt_addr1->prefixlen != rt_addr2->prefixlen) {
			goto failed;
		}
		break;
	case MNLXT_RT_ADDR_FLAGS:
		if (rt_addr1->flags != rt_addr2->flags) {
			goto failed;
		}
		break;
	case MNLXT_RT_ADDR_SCOPE:
		if (rt_addr1->scope != rt_addr2->scope) {
			goto failed;
		}
		break;
	case MNLXT_RT_ADDR_IFINDEX:
		if (rt_addr1->if_index != rt_addr2->if_index) {
			goto failed;
		}
		break;
	case MNLXT_RT_ADDR_ADDR:
		addr_size = (AF_INET == rt_addr1->family ? sizeof(rt_addr1->addr.in) : sizeof(rt_addr1->addr));
		if (0 != memcmp(&rt_addr1->addr, &rt_addr2->addr, addr_size)) {
			goto failed;
		}
		break;
	case MNLXT_RT_ADDR_LOCAL:
		addr_size = (AF_INET == rt_addr1->family ? sizeof(rt_addr1->addr_local.in) : sizeof(rt_addr1->addr_local));
		if (0 != memcmp(&rt_addr1->addr_local, &rt_addr2->addr_local, addr_size)) {
			goto failed;
		}
		break;
	case MNLXT_RT_ADDR_LABEL:
		if (!rt_addr1->label || !rt_addr2->label || 0 != strcmp(rt_addr1->label, rt_addr2->label)) {
			goto failed;
		}
		break;
	case MNLXT_RT_ADDR_CACHEINFO:
#if 0 /* don't compare cache info */
		if (0 != memcmp(&rt_addr1->cacheinfo, &rt_addr2->cacheinfo, sizeof(addr->cacheinfo))) {
			goto failed;
		}
#endif
		break;
	}
	rc = 0;
failed:
	return rc;
}

int mnlxt_rt_addr_match(const mnlxt_rt_addr_t *addr, const mnlxt_rt_addr_t *match) {
	int rc = -1;
	if (NULL == match) {
		errno = EINVAL;
	} else {
		rc = mnlxt_rt_addr_compare(addr, match, match->prop_flags);
	}
	return rc;
}

int mnlxt_rt_addr_compare(const mnlxt_rt_addr_t *rt_addr1, const mnlxt_rt_addr_t *rt_addr2, uint64_t filter) {
	int rc = -1, i;
	if (NULL == rt_addr1 || NULL == rt_addr2) {
		errno = EINVAL;
	} else {
		for (i = 0; i < MNLXT_RT_ADDR_MAX; ++i) {
			uint64_t flag = MNLXT_FLAG(i);
			if (0 == (flag & filter)) {
				continue;
			}
			if (0 == (rt_addr1->prop_flags & flag)) {
				if (0 == (rt_addr2->prop_flags & flag)) {
					/* both not set */
					continue;
				}
				goto failed;
			} else if (0 == (rt_addr2->prop_flags & flag)) {
				goto failed;
			} else if (0 != mnlxt_rt_addr_cmp(rt_addr1, rt_addr2, i)) {
				goto failed;
			}
		}
		return 0;
	failed:
		rc = ++i;
	}
	return rc;
}

mnlxt_rt_addr_t *mnlxt_rt_addr_get(const mnlxt_message_t *message) {
	mnlxt_rt_addr_t *addr = NULL;
	if (message && message->payload
			&& (RTM_NEWADDR == message->nlmsg_type || RTM_GETADDR == message->nlmsg_type
					|| RTM_DELADDR == message->nlmsg_type)) {
		addr = (mnlxt_rt_addr_t *)message->payload;
	}
	return addr;
}

mnlxt_rt_addr_t *mnlxt_rt_addr_remove(mnlxt_message_t *message) {
	mnlxt_rt_addr_t *addr = mnlxt_rt_addr_get(message);
	if (NULL != addr) {
		message->payload = NULL;
	}
	return addr;
}

int mnlxt_rt_addr_put(struct nlmsghdr *nlh, const mnlxt_rt_addr_t *addr) {
	int rc = -1;
	if (addr && nlh) {
		struct ifaddrmsg *ifam = mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifaddrmsg));
		int i;
		int flag = 0x1;
		size_t addr_size = (AF_INET == addr->family ? sizeof(addr->addr.in) : sizeof(addr->addr));
		for (i = 0; i < MNLXT_RT_ADDR_MAX; ++i) {
			if (addr->prop_flags & flag) {
				switch (i) {
				case MNLXT_RT_ADDR_FAMILY:
					ifam->ifa_family = addr->family;
					break;
				case MNLXT_RT_ADDR_PREFIXLEN:
					ifam->ifa_prefixlen = addr->prefixlen;
					break;
				case MNLXT_RT_ADDR_FLAGS:
					ifam->ifa_flags = (uint8_t)addr->flags;
#if HAVE_IFA_FLAGS
					mnl_attr_put_u32(nlh, IFA_FLAGS, addr->flags);
#endif
					break;
				case MNLXT_RT_ADDR_SCOPE:
					ifam->ifa_scope = addr->scope;
					break;
				case MNLXT_RT_ADDR_IFINDEX:
					ifam->ifa_index = addr->if_index;
					break;
				case MNLXT_RT_ADDR_ADDR:
					mnl_attr_put(nlh, IFA_ADDRESS, addr_size, &addr->addr);
					break;
				case MNLXT_RT_ADDR_LOCAL:
					mnl_attr_put(nlh, IFA_LOCAL, addr_size, &addr->addr_local);
					break;
				case MNLXT_RT_ADDR_LABEL:
					mnl_attr_put_str(nlh, IFA_LABEL, addr->label);
					break;
				case MNLXT_RT_ADDR_CACHEINFO:
					mnl_attr_put(nlh, IFA_CACHEINFO, sizeof(addr->cacheinfo), &addr->cacheinfo);
					break;
				}
			}
			flag <<= 1;
		}
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_addr_PUT(struct nlmsghdr *nlh, const void *addr, uint16_t nlmsg_type) {
	(void)nlmsg_type;
	return mnlxt_rt_addr_put(nlh, (mnlxt_rt_addr_t *)addr);
}

int mnlxt_rt_addr_data(const struct nlmsghdr *nlh, mnlxt_data_t *data) {
	int rc = MNL_CB_ERROR;
	mnlxt_message_t *msg = NULL;
	mnlxt_rt_addr_t *rt_addr = NULL;

	if (NULL == data) {
		errno = EINVAL;
		goto end;
	}

	if (NULL == nlh) {
		errno = EINVAL;
		data->error_str = "invalid arguments";
		goto end;
	}

	if (RTM_GETADDR != nlh->nlmsg_type && RTM_NEWADDR != nlh->nlmsg_type && RTM_DELADDR != nlh->nlmsg_type) {
		errno = EBADMSG;
		data->error_str = "unsupported message type";
		goto end;
	}

	struct ifaddrmsg *ifam = mnl_nlmsg_get_payload(nlh);

	/* IPv6 and IPv4 support only */
	size_t len = 0;
	if (AF_INET == ifam->ifa_family) {
		len = sizeof(struct in_addr);
	} else if (AF_INET6 == ifam->ifa_family) {
		len = sizeof(struct in6_addr);
	} else {
		rc = MNL_CB_OK;
		goto end;
	}

	rt_addr = mnlxt_rt_addr_new();
	if (NULL == rt_addr) {
		data->error_str = "mnlxt_rt_addr_new failed";
		goto end;
	}

	mnlxt_rt_addr_set_prefixlen(rt_addr, ifam->ifa_prefixlen);
	mnlxt_rt_addr_set_flags(rt_addr, ifam->ifa_flags);
	mnlxt_rt_addr_set_ifindex(rt_addr, ifam->ifa_index);
	mnlxt_rt_addr_set_scope(rt_addr, ifam->ifa_scope);

	struct nlattr *attr;
	mnl_attr_for_each(attr, nlh, sizeof(*ifam)) {
		int type = mnl_attr_get_type(attr);
		/* skip unsupported attribute in user-space */
		if (0 > mnl_attr_type_valid(attr, IFA_MAX)) {
			continue;
		}
		switch (type) {
		case IFA_UNSPEC:
			break;
		case IFA_ADDRESS: /* ppp = remote */
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, len)) {
				data->error_str = "IFA_ADDRESS validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_addr_set_addr(rt_addr, ifam->ifa_family, mnl_attr_get_payload(attr))) {
				data->error_str = "mnlxt_rt_addr_set_addr failed";
				goto end;
			}
			break;
		case IFA_LOCAL: /* ppp = local */
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, len)) {
				data->error_str = "IFA_LOCAL validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_addr_set_local(rt_addr, ifam->ifa_family, mnl_attr_get_payload(attr))) {
				data->error_str = "mnlxt_rt_addr_set_local failed";
				goto end;
			}
			break;
		case IFA_LABEL:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_STRING)) {
				data->error_str = "IFA_LABEL validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_addr_set_label(rt_addr, mnl_attr_get_str(attr))) {
				data->error_str = "mnlxt_rt_addr_set_label failed";
				goto end;
			}
			break;
		case IFA_BROADCAST:
			break;
		case IFA_ANYCAST:
			break;
		case IFA_CACHEINFO:
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, sizeof(struct ifa_cacheinfo))) {
				data->error_str = "IFA_CACHEINFO validation failed";
				goto end;
			}
			memcpy(&rt_addr->cacheinfo, mnl_attr_get_payload(attr), sizeof(struct ifa_cacheinfo));
			MNLXT_SET_PROP_FLAG(rt_addr, MNLXT_RT_ADDR_CACHEINFO);
			break;
		case IFA_MULTICAST:
			break;
#if HAVE_IFA_FLAGS
		case IFA_FLAGS:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "IFA_FLAGS validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_addr_set_flags(rt_addr, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_addr_set_flags failed";
				goto end;
			}
			break;
#endif
		default:
			break;
		}
	}

	msg = mnlxt_rt_message_new(nlh->nlmsg_type, 0, rt_addr);
	if (NULL == msg) {
		data->error_str = "mnlxt_rt_message_new failed";
		goto end;
	}

	mnlxt_data_add(data, msg);
	rt_addr = NULL;
	msg = NULL;
	rc = MNL_CB_OK;
end:
	mnlxt_rt_addr_free(rt_addr);
	mnlxt_message_free(msg);
	return rc;
}

int mnlxt_rt_addr_DATA(const struct nlmsghdr *nlh, void *data) {
	return mnlxt_rt_addr_data(nlh, (mnlxt_data_t *)data);
}

mnlxt_rt_addr_t *mnlxt_rt_addr_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator) {
	mnlxt_rt_addr_t *addr = NULL;
	if (iterator) {
		while ((*iterator = mnlxt_data_iterate(data, *iterator))) {
			if ((addr = mnlxt_rt_addr_get(*iterator))) {
				break;
			}
		}
	}
	return addr;
}

int mnlxt_rt_addr_dump(mnlxt_data_t *data, unsigned char family) {
	int rc = -1;
	if (AF_INET != family && AF_INET6 != family && AF_UNSPEC != family) {
		errno = EAFNOSUPPORT;
	} else {
		rc = mnlxt_rt_data_dump(data, RTM_GETADDR, family);
	}
	return rc;
}

int mnlxt_rt_addr_request(mnlxt_rt_addr_t *rt_addr, uint16_t type, uint16_t flags) {
	int rc = -1;
	mnlxt_message_t *message = mnlxt_rt_addr_message(&rt_addr, type, flags);
	if (NULL != message) {
		rc = mnlxt_rt_message_request(message);
		mnlxt_rt_addr_remove(message);
		mnlxt_message_free(message);
	}
	return rc;
}

mnlxt_message_t *mnlxt_rt_addr_message(mnlxt_rt_addr_t **rt_addr, uint16_t type, uint16_t flags) {
	mnlxt_message_t *message = NULL;
	if (NULL == rt_addr || NULL == *rt_addr || !(RTM_NEWADDR == type || RTM_DELADDR == type)) {
		errno = EINVAL;
	} else if (NULL != (message = mnlxt_rt_message_new(type, flags, *rt_addr))) {
		*rt_addr = NULL;
	}
	return message;
}
