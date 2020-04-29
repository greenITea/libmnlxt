/*
 * link_data.c		Libmnlxt Routing Link
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

static const char *info_kinds[] = {
	[MNLXT_RT_LINK_INFO_KIND_BR] = "bridge",
	[MNLXT_RT_LINK_INFO_KIND_VLAN] = "vlan",
	[MNLXT_RT_LINK_INFO_KIND_BOND] = "bond",
	[MNLXT_RT_LINK_INFO_KIND_TUN] = "tun",
};

static int mnlxt_rt_link_vlan_match(const mnlxt_rt_link_vlan_t *vlan, uint16_t prop_flags,
																		const mnlxt_rt_link_vlan_t *match, uint16_t match_flags) {
	int i = -1;
	if (vlan && match) {
		int flag = 0x1;
		for (i = 0; i < MNLXT_RT_LINK_VLAN_MAX; ++i) {
			if (match_flags & flag) {
				if (prop_flags & flag) {
					switch (i) {
					case MNLXT_RT_LINK_VLAN_ID:
						if (vlan->id != match->id) {
							goto fail;
						}
					}
				}
			}
		}
		return 0;
	fail:
		++i;
	}
	return i;
}

static int mnlxt_rt_link_info_match(const mnlxt_rt_link_info_t *link_info, const mnlxt_rt_link_info_t *info_match) {
	int rc = 1;
	if (link_info && info_match) {
		if (link_info->kind == info_match->kind) {
			switch (link_info->kind) {
			case MNLXT_RT_LINK_INFO_KIND_BOND:
				/*TODO*/ rc = 0;
				break;
			case MNLXT_RT_LINK_INFO_KIND_BR:
				/*TODO*/ rc = 0;
				break;
			case MNLXT_RT_LINK_INFO_KIND_VLAN:
				rc = mnlxt_rt_link_vlan_match(&link_info->data.vlan, link_info->prop_flags, &info_match->data.vlan,
																			info_match->prop_flags);
				break;
			case MNLXT_RT_LINK_INFO_KIND_TUN:
				/*TODO*/ rc = 0;
				break;
			}
		}
	} else {
		rc = -1;
	}
	return rc;
}

static mnlxt_rt_link_info_kind_t mnlxt_rt_link_info_kind_get_enum(const char *kind) {
	int rc = -1;
	if (kind) {
		int index = 0;
		for (; index < MNLXT_RT_LINK_INFO_KIND_MAX; ++index) {
			if (0 == strcmp(kind, info_kinds[index])) {
				break;
			}
		}
		if (index < MNLXT_RT_LINK_INFO_KIND_MAX) {
			rc = index;
		}
	}
	return (mnlxt_rt_link_info_kind_t)rc;
}

static const char *mnlxt_rt_link_info_kind_get_string(mnlxt_rt_link_info_kind_t info_kind) {
	char *kind = NULL;
	if (0 <= info_kind && info_kind < MNLXT_RT_LINK_INFO_KIND_MAX) {
		kind = (char *)info_kinds[info_kind];
	}
	return (const char *)kind;
}

static int mnlxt_rt_link_cmp(const mnlxt_rt_link_t *rt_link1, const mnlxt_rt_link_t *rt_link2,
														 mnlxt_rt_link_data_t data) {
	int rc = data + 1;
	uint32_t flag_mask;
	switch (data) {
	case MNLXT_RT_LINK_TYPE:
		if (rt_link1->type != rt_link2->type) {
			goto failed;
		}
		break;
	case MNLXT_RT_LINK_FAMILY:
		if (rt_link1->family != rt_link2->family) {
			goto failed;
		}
		break;
	case MNLXT_RT_LINK_NAME:
		if (0 != strcmp(rt_link1->name, rt_link2->name)) {
			goto failed;
		}
		break;
	case MNLXT_RT_LINK_INDEX:
		if (rt_link1->index != rt_link2->index) {
			goto failed;
		}
		break;
	case MNLXT_RT_LINK_FLAGS:
		/* calculate common flag mask */
		flag_mask = rt_link1->flag_mask & rt_link2->flag_mask;
		/* compare what can be compare */
		if ((rt_link1->flags & flag_mask) != (rt_link2->flags & flag_mask)) {
			goto failed;
		}
		break;
	case MNLXT_RT_LINK_HWADDR:
		if (0 != memcmp(rt_link1->mac, rt_link2->mac, sizeof(rt_link1->mac))) {
			goto failed;
		}
		break;
	case MNLXT_RT_LINK_MTU:
		if (rt_link1->mtu != rt_link2->mtu) {
			goto failed;
		}
		break;
	case MNLXT_RT_LINK_MASTER:
		if (rt_link1->master != rt_link2->master) {
			goto failed;
		}
		break;
	case MNLXT_RT_LINK_STATE:
		if (rt_link1->state != rt_link2->state) {
			goto failed;
		}
		break;
	case MNLXT_RT_LINK_PARENT:
		if (rt_link1->parent != rt_link2->parent) {
			goto failed;
		}
		break;
	case MNLXT_RT_LINK_INFO:
		if (mnlxt_rt_link_info_match(&rt_link1->info, &rt_link2->info)) {
			goto failed;
		}
		break;
	}
	rc = 0;
failed:
	return rc;
}

int mnlxt_rt_link_match(const mnlxt_rt_link_t *rt_link, const mnlxt_rt_link_t *match) {
	return mnlxt_rt_link_compare(rt_link, match, match->prop_flags);
}

int mnlxt_rt_link_compare(const mnlxt_rt_link_t *rt_link1, const mnlxt_rt_link_t *rt_link2, uint64_t filter) {
	int rc = -1, i;
	if (NULL == rt_link1 || NULL == rt_link2) {
		errno = EINVAL;
	} else {
		for (i = 0; i < MNLXT_RT_LINK_MAX; ++i) {
			uint64_t flag = MNLXT_FLAG(i);
			if (0 == (flag & filter)) {
				continue;
			}
			if (0 == (rt_link1->prop_flags & flag)) {
				if (0 == (rt_link2->prop_flags & flag)) {
					/* both not set */
					continue;
				}
				goto failed;
			} else if (0 == (rt_link2->prop_flags & flag)) {
				goto failed;
			} else if (0 != mnlxt_rt_link_cmp(rt_link1, rt_link2, i)) {
				goto failed;
			}
		}
		return 0;
	failed:
		rc = ++i;
	}
	return rc;
}

static int mnlxt_rt_link_info_put(struct nlmsghdr *nlh, const mnlxt_rt_link_t *link) {
	int rc = -1;
	mnlxt_rt_link_info_kind_t info_kind = -1;
	struct nlattr *nest1, *nest2;

	if (0 == mnlxt_rt_link_get_info_kind(link, &info_kind)) {
		const char *kind = mnlxt_rt_link_info_kind_get_string(info_kind);
		if (kind) {
			nest1 = mnl_attr_nest_start(nlh, IFLA_LINKINFO);
			mnl_attr_put_str(nlh, IFLA_INFO_KIND, kind);
			if (MNLXT_RT_LINK_INFO_KIND_VLAN == info_kind) {
				uint16_t id = 0;
				if (0 == mnlxt_rt_link_get_vlan_id(link, &id)) {
					nest2 = mnl_attr_nest_start(nlh, IFLA_INFO_DATA);
					mnl_attr_put_u16(nlh, IFLA_VLAN_ID, id);
					mnl_attr_nest_end(nlh, nest2);
				}
			}
			mnl_attr_nest_end(nlh, nest1);
			rc = 0;
		}
	}
	return rc;
}

int mnlxt_rt_link_put(struct nlmsghdr *nlh, const mnlxt_rt_link_t *link) {
	int rc = -1;
	if (link && nlh) {
		struct ifinfomsg *ifm = mnl_nlmsg_put_extra_header(nlh, sizeof(struct ifinfomsg));
		int flag = 0x1;
		int i = 0;
		for (; i < MNLXT_RT_LINK_MAX; ++i) {
			if (link->prop_flags & flag) {
				switch (i) {
				case MNLXT_RT_LINK_TYPE:
					ifm->ifi_type = link->type;
					break;
				case MNLXT_RT_LINK_FAMILY:
					ifm->ifi_family = link->family;
					break;
				case MNLXT_RT_LINK_NAME:
					mnl_attr_put_str(nlh, IFLA_IFNAME, link->name);
					break;
				case MNLXT_RT_LINK_INDEX:
					ifm->ifi_index = link->index;
					break;
				case MNLXT_RT_LINK_FLAGS:
					ifm->ifi_flags = link->flags;
					ifm->ifi_change = link->flag_mask;
					break;
				case MNLXT_RT_LINK_HWADDR:
					mnl_attr_put(nlh, IFLA_ADDRESS, sizeof(link->mac), link->mac);
					break;
				case MNLXT_RT_LINK_MTU:
					mnl_attr_put_u32(nlh, IFLA_MTU, link->mtu);
					break;
				case MNLXT_RT_LINK_MASTER:
					mnl_attr_put_u32(nlh, IFLA_MASTER, link->master);
					break;
				case MNLXT_RT_LINK_STATE:
					mnl_attr_put_u32(nlh, IFLA_OPERSTATE, link->state);
					break;
				case MNLXT_RT_LINK_PARENT:
					mnl_attr_put_u32(nlh, IFLA_LINK, link->parent);
					break;
				case MNLXT_RT_LINK_INFO:
					mnlxt_rt_link_info_put(nlh, link);
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

int mnlxt_rt_link_PUT(struct nlmsghdr *nlh, const void *link, uint16_t nlmsg_type) {
	(void)nlmsg_type;
	return mnlxt_rt_link_put(nlh, (mnlxt_rt_link_t *)link);
}

static int mnlxt_rt_link_info_data_vlan(const struct nlattr *link_vlan_attr, mnlxt_data_t *data,
																				mnlxt_rt_link_t *link) {
	int rc = -1;
	const struct nlattr *attr;

	mnl_attr_for_each_nested(attr, link_vlan_attr) {
		int type = mnl_attr_get_type(attr);
		switch (type) {
		case IFLA_VLAN_ID:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U16)) {
				data->error_str = "IFLA_VLAN_ID validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_vlan_id(link, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_link_set_vlan_id failed";
				goto end;
			}
			break;
			/*
			 * IFLA_VLAN_FLAGS,
			 * IFLA_VLAN_EGRESS_QOS,
			 * IFLA_VLAN_INGRESS_QOS,
			 * IFLA_VLAN_PROTOCOL
			 */
		}
	}
	rc = 0;
end:
	return rc;
}

static int mnlxt_rt_link_info_attr(const struct nlattr *link_info_attr, mnlxt_data_t *data, mnlxt_rt_link_t *link) {
	int rc = -1;
	const struct nlattr *attr, *data_attr = NULL;
	mnlxt_rt_link_info_kind_t info_kind = -1;
	const char *info_kind_str = NULL;

	mnl_attr_for_each_nested(attr, link_info_attr) {
		int type = mnl_attr_get_type(attr);
		switch (type) {
		case IFLA_INFO_KIND:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_STRING)) {
				data->error_str = "IFLA_INFO_KIND validation failed";
				goto end;
			}
			info_kind_str = mnl_attr_get_str(attr);
			info_kind = mnlxt_rt_link_info_kind_get_enum(info_kind_str);
			if (-1 == info_kind) {
				strncpy(data->error_buf, "not supported IFLA_INFO_KIND: ", sizeof(data->error_buf) - 1);
				strncat(data->error_buf, info_kind_str, sizeof(data->error_buf) - 1);
				data->error_str = data->error_buf;
				/* unknown kind: store the message, but report as success */
				rc = 0;
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_info_kind(link, info_kind)) {
				data->error_str = "mnlxt_rt_link_set_info_kind failed";
				goto end;
			}
			break;
		case IFLA_INFO_DATA:
			data_attr = attr;
			break;
		}
	}
	if (data_attr) {
		switch (info_kind) {
		case MNLXT_RT_LINK_INFO_KIND_BOND:
			/*TODO*/ rc = 0;
			break;
		case MNLXT_RT_LINK_INFO_KIND_BR:
			/*TODO*/ rc = 0;
			break;
		case MNLXT_RT_LINK_INFO_KIND_VLAN:
			rc = mnlxt_rt_link_info_data_vlan(data_attr, data, link);
			break;
		case MNLXT_RT_LINK_INFO_KIND_TUN:
			/*TODO*/ rc = 0;
			break;
		}
	} else {
		rc = 0;
	}
end:
	return rc;
}

int mnlxt_rt_link_data(const struct nlmsghdr *nlh, mnlxt_data_t *data) {
	int rc = MNL_CB_ERROR;
	mnlxt_message_t *msg = NULL;
	mnlxt_rt_link_t *link = NULL;
	if (!data) {
		errno = EINVAL;
		goto end;
	}

	if (!nlh) {
		errno = EINVAL;
		data->error_str = "invalid arguments";
		goto end;
	}

	if (RTM_GETLINK != nlh->nlmsg_type && RTM_NEWLINK != nlh->nlmsg_type && RTM_DELLINK != nlh->nlmsg_type
			&& RTM_SETLINK != nlh->nlmsg_type) {
		errno = EBADMSG;
		data->error_str = "unsupported message type";
		goto end;
	}

	link = mnlxt_rt_link_new();
	if (!link) {
		data->error_str = "nl_link_new failed";
		goto end;
	}

	struct ifinfomsg *ifm = mnl_nlmsg_get_payload(nlh);

	mnlxt_rt_link_set_flags(link, ifm->ifi_flags);
	mnlxt_rt_link_set_index(link, ifm->ifi_index);
	mnlxt_rt_link_set_type(link, ifm->ifi_type);
	mnlxt_rt_link_set_family(link, ifm->ifi_family);

	struct nlattr *attr;
	mnl_attr_for_each(attr, nlh, sizeof(*ifm)) {
		int type = mnl_attr_get_type(attr);

		/* skip unsupported attribute in user-space */
		if (0 > mnl_attr_type_valid(attr, IFLA_MAX)) {
			continue;
		}

		switch (type) {
		case IFLA_MTU:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "IFLA_MTU validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_mtu(link, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_link_set_mtu failed";
				goto end;
			}
			break;
		case IFLA_IFNAME:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_STRING)) {
				data->error_str = "IFLA_IFNAME validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_name(link, mnl_attr_get_str(attr))) {
				data->error_str = "mnlxt_rt_link_set_name failed";
				goto end;
			}
			break;
		case IFLA_ADDRESS: /* hardware address interface L2 address */
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, sizeof(link->mac))) {
				data->error_str = "IFLA_ADDRESS validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_hwaddr(link, mnl_attr_get_payload(attr))) {
				data->error_str = "mnlxt_rt_link_set_hwaddr failed";
				goto end;
			}
			break;
		case IFLA_LINK: /* int Link type. */
			/* IFLA_LINK. For usual devices it is equal ifi_index. If it is a "virtual interface" (f.e. tunnel),
			 * ifi_link can point to real physical interface (f.e. for bandwidth calculations), or maybe 0, what means,
			 * that real media is unknown (usual for IPIP tunnels, when route to endpoint is allowed to change)
			 */
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "IFLA_LINK validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_parent(link, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_link_set_parent failed";
				goto end;
			}
			break;
			break;
		case IFLA_QDISC: /* ascii string Queueing discipline. */
			break;
		case IFLA_STATS: /* struct net_device_stats */
			break;
		case IFLA_OPERSTATE: /* link state */
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U8)) {
				data->error_str = "IFLA_OPERSTATE validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_state(link, mnl_attr_get_u8(attr))) {
				data->error_str = "mnlxt_rt_link_set_state failed";
				goto end;
			}
			break;
		case IFLA_MASTER:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "IFLA_MASTER validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_link_set_master(link, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_link_set_master failed";
				goto end;
			}
			break;
		case IFLA_LINKINFO:
			if (-1 == mnlxt_rt_link_info_attr(attr, data, link)) {
				goto end;
			}
			break;
		default:
			break;
		}
	}

	msg = mnlxt_rt_message_new(nlh->nlmsg_type, link);
	if (!msg) {
		data->error_str = "mnlxt_rt_message_new failed";
		goto end;
	}

	mnlxt_data_add(data, msg);
	link = NULL;
	msg = NULL;
	rc = MNL_CB_OK;
end:
	if (link) {
		mnlxt_rt_link_free(link);
	}
	if (msg) {
		mnlxt_message_free(msg);
	}
	return rc;
}

int mnlxt_rt_link_DATA(const struct nlmsghdr *nlh, void *data) {
	return mnlxt_rt_link_data(nlh, (mnlxt_data_t *)data);
}

mnlxt_rt_link_t *mnlxt_rt_link_get(const mnlxt_message_t *message) {
	mnlxt_rt_link_t *link = NULL;
	if (message && message->payload
			&& (RTM_NEWLINK == message->nlmsg_type || RTM_GETLINK == message->nlmsg_type || RTM_DELLINK == message->nlmsg_type
					|| RTM_SETLINK == message->nlmsg_type)) {
		link = (mnlxt_rt_link_t *)message->payload;
	}
	return link;
}

mnlxt_rt_link_t *mnlxt_rt_link_remove(mnlxt_message_t *message) {
	mnlxt_rt_link_t *rt_link = mnlxt_rt_link_get(message);
	if (NULL != rt_link) {
		message->payload = NULL;
	}
	return rt_link;
}

mnlxt_rt_link_t *mnlxt_rt_link_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator) {
	mnlxt_rt_link_t *link = NULL;
	if (iterator) {
		while ((*iterator = mnlxt_data_iterate(data, *iterator))) {
			if ((link = mnlxt_rt_link_get(*iterator))) {
				break;
			}
		}
	}
	return link;
}

int mnlxt_rt_link_dump(mnlxt_data_t *data) {
	return mnlxt_rt_data_dump(data, RTM_GETLINK, AF_PACKET);
}

int mnlxt_rt_link_request(mnlxt_rt_link_t *rt_link, uint16_t type) {
	int rc = -1;
	mnlxt_message_t *message = mnlxt_rt_link_message(&rt_link, type);
	if (NULL != message) {
		rc = mnlxt_rt_message_request(message);
		mnlxt_rt_link_remove(message);
		mnlxt_message_free(message);
	}
	return rc;
}

mnlxt_message_t *mnlxt_rt_link_message(mnlxt_rt_link_t **link, uint16_t type) {
	mnlxt_message_t *message = NULL;
	if (!link || !*link || !(RTM_NEWLINK == type || RTM_DELLINK == type || RTM_SETLINK == type)) {
		errno = EINVAL;
	} else {
		message = mnlxt_rt_message_new(type, *link);
		if (message) {
			*link = NULL;
		}
	}
	return message;
}
