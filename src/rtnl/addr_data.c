#include <errno.h>
#include <linux/if_addr.h>
#include <linux/rtnetlink.h>
#include <string.h>

#include "../../include/libmnlxt/rt.h"
#include "../internal.h"

int mnlxt_rt_addr_match(mnlxt_rt_addr_t *addr, mnlxt_rt_addr_t *match) {
	int i = -1;
	if (addr && match) {
		int flag = 0x1;
		size_t addr_size = (AF_INET == addr->family ? sizeof(addr->addr.in) : sizeof(addr->addr));
		for (i = 0; i < MNLXT_RT_ADDR_MAX; ++i) {
			if (match->prop_flags & flag) {
				if (addr->prop_flags & flag) {
					switch (i) {
					case MNLXT_RT_ADDR_FAMILY:
						if (addr->family != match->family) {
							goto failed;
						}
						break;
					case MNLXT_RT_ADDR_PREFIXLEN:
						if (addr->prefixlen != match->prefixlen) {
							goto failed;
						}
						break;
					case MNLXT_RT_ADDR_FLAGS:
						if (addr->flags != match->flags) {
							goto failed;
						}
						break;
					case MNLXT_RT_ADDR_SCOPE:
						if (addr->scope != match->scope) {
							goto failed;
						}
						break;
					case MNLXT_RT_ADDR_IFINDEX:
						if (addr->if_index != match->if_index) {
							goto failed;
						}
						break;
					case MNLXT_RT_ADDR_ADDR:
						if (0 != memcmp(&addr->addr, &match->addr, addr_size)) {
							goto failed;
						}
						break;
					case MNLXT_RT_ADDR_LOCAL:
						if (0 != memcmp(&addr->addr_local, &match->addr_local, addr_size)) {
							goto failed;
						}
						break;
					case MNLXT_RT_ADDR_LABEL:
						if (!addr->label || !match->label || 0 != strcmp(addr->label, match->label)) {
							goto failed;
						}
						break;
					case MNLXT_RT_ADDR_CACHEINFO:
						if (0 != memcmp(&addr->cacheinfo, &match->cacheinfo, sizeof(addr->cacheinfo))) {
							goto failed;
						}
						break;
					}
				} else {
					goto failed;
				}
			}
			flag <<= 1;
		}
		return 0;
	failed:
		++i;
	} else {
		errno = EINVAL;
	}
	return i;
}

mnlxt_rt_addr_t *mnlxt_rt_addr_get(mnlxt_message_t *message) {
	mnlxt_rt_addr_t *addr = NULL;
	if (message && message->payload
			&& (RTM_NEWADDR == message->nlmsg_type || RTM_GETADDR == message->nlmsg_type
					|| RTM_DELADDR == message->nlmsg_type)) {
		addr = (mnlxt_rt_addr_t *)message->payload;
	}
	return addr;
}

int mnlxt_rt_addr_put(struct nlmsghdr *nlh, mnlxt_rt_addr_t *addr) {
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
#if 0
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

int mnlxt_rt_addr_PUT(struct nlmsghdr *nlh, void *addr, uint16_t nlmsg_type) {
	(void)nlmsg_type;
	return mnlxt_rt_addr_put(nlh, (mnlxt_rt_addr_t *)addr);
}

int mnlxt_rt_addr_data(const struct nlmsghdr *nlh, mnlxt_data_t *data) {
	int rc = MNL_CB_ERROR;
	mnlxt_message_t *msg = NULL;
	mnlxt_rt_addr_t *addr = NULL;

	if (!data) {
		errno = EINVAL;
		goto end;
	}

	if (!nlh) {
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

	addr = mnlxt_rt_addr_new();
	if (!addr) {
		data->error_str = "mnlxt_rt_addr_new failed";
		goto end;
	}

	mnlxt_rt_addr_set_prefixlen(addr, ifam->ifa_prefixlen);
	mnlxt_rt_addr_set_flags(addr, ifam->ifa_flags);
	mnlxt_rt_addr_set_ifindex(addr, ifam->ifa_index);
	mnlxt_rt_addr_set_scope(addr, ifam->ifa_scope);

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
			if (-1 == mnlxt_rt_addr_set_addr(addr, ifam->ifa_family, mnl_attr_get_payload(attr))) {
				data->error_str = "mnlxt_rt_addr_set_addr failed";
				goto end;
			}
			break;
		case IFA_LOCAL: /* ppp = local */
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, len)) {
				data->error_str = "IFA_LOCAL validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_addr_set_local(addr, ifam->ifa_family, mnl_attr_get_payload(attr))) {
				data->error_str = "mnlxt_rt_addr_set_local failed";
				goto end;
			}
			break;
		case IFA_LABEL:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_STRING)) {
				data->error_str = "IFA_LABEL validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_addr_set_label(addr, mnl_attr_get_str(attr))) {
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
			memcpy(&addr->cacheinfo, mnl_attr_get_payload(attr), sizeof(struct ifa_cacheinfo));
			MNLXT_SET_PROP_FLAG(addr, MNLXT_RT_ADDR_CACHEINFO);
			break;
		case IFA_MULTICAST:
			break;
#if 0
		case IFA_FLAGS:
			/* u32 attribute that extends the u8 field ifa_flags */
			break;
#endif
		default:
			break;
		}
	}

	msg = mnlxt_rt_message_new(nlh->nlmsg_type, addr);
	if (!msg) {
		data->error_str = "mnlxt_rt_message_new failed";
		goto end;
	}

	mnlxt_data_add(data, msg);
	addr = NULL;
	msg = NULL;
	rc = MNL_CB_OK;
end:
	if (addr) {
		mnlxt_rt_addr_free(addr);
	}
	if (msg) {
		mnlxt_message_free(msg);
	}
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

mnlxt_message_t *mnlxt_rt_addr_message(mnlxt_rt_addr_t **addr, uint16_t type) {
	mnlxt_message_t *message = NULL;
	if (!addr || !*addr || !(RTM_NEWADDR == type || RTM_DELADDR == type)) {
		errno = EINVAL;
	} else {
		message = mnlxt_rt_message_new(type, *addr);
		if (message) {
			*addr = NULL;
		}
	}
	return message;
}
