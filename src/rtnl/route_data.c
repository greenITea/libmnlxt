/*
 * route_data.c		Libmnlxt Routing Routes
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

static int mnlxt_rt_route_cmp(const mnlxt_rt_route_t *rt_route1, const mnlxt_rt_route_t *rt_route2,
															mnlxt_rt_route_data_t data) {
	int rc = data + 1;
	size_t family_size;
	switch (data) {
	case MNLXT_RT_ROUTE_FAMILY:
		if (rt_route1->family != rt_route2->family) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_TABLE:
		if (rt_route1->table != rt_route2->table) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_TYPE:
		if (rt_route1->type != rt_route2->type) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_PROTOCOL:
		if (rt_route1->protocol != rt_route2->protocol) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_SCOPE:
		if (rt_route1->scope != rt_route2->scope) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_PRIORITY:
		if (rt_route1->priority != rt_route2->priority) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_IIFINDEX:
		if (rt_route1->iif_index != rt_route2->iif_index) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_OIFINDEX:
		if (rt_route1->oif_index != rt_route2->oif_index) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_SRC_PREFIX:
		if (rt_route1->src_prefix != rt_route2->src_prefix) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_DST_PREFIX:
		if (rt_route1->dst_prefix != rt_route2->dst_prefix) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_SRC:
		family_size = (AF_INET == rt_route1->family ? sizeof(rt_route1->src.in) : sizeof(rt_route1->src));
		if (0 != memcmp(&rt_route1->src, &rt_route2->src, family_size)) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_DST:
		family_size = (AF_INET == rt_route1->family ? sizeof(rt_route1->dst.in) : sizeof(rt_route1->dst));
		if (0 != memcmp(&rt_route1->dst, &rt_route2->dst, family_size)) {
			goto failed;
		}
		break;
	case MNLXT_RT_ROUTE_GATEWAY:
		family_size = (AF_INET == rt_route1->family ? sizeof(rt_route1->gateway.in) : sizeof(rt_route1->gateway));
		if (0 != memcmp(&rt_route1->gateway, &rt_route2->gateway, family_size)) {
			goto failed;
		}
		break;
	}
	rc = 0;
failed:
	return rc;
}

int mnlxt_rt_route_match(const mnlxt_rt_route_t *rt_route, const mnlxt_rt_route_t *match) {
	int rc = -1;
	if (NULL == match) {
		errno = EINVAL;
	} else {
		rc = mnlxt_rt_route_compare(rt_route, match, match->prop_flags);
	}
	return rc;
}

int mnlxt_rt_route_compare(const mnlxt_rt_route_t *rt_route1, const mnlxt_rt_route_t *rt_route2, uint64_t filter) {
	int rc = -1, i;
	if (NULL == rt_route1 || NULL == rt_route2) {
		errno = EINVAL;
	} else {
		for (i = 0; i < MNLXT_RT_ROUTE_MAX; ++i) {
			uint64_t flag = MNLXT_FLAG(i);
			if (0 == (flag & filter)) {
				continue;
			}
			if (0 == (rt_route1->prop_flags & flag)) {
				if (0 == (rt_route2->prop_flags & flag)) {
					/* both not set */
					continue;
				}
				goto failed;
			} else if (0 == (rt_route2->prop_flags & flag)) {
				goto failed;
			} else if (0 != mnlxt_rt_route_cmp(rt_route1, rt_route2, i)) {
				goto failed;
			}
		}
		return 0;
	failed:
		rc = ++i;
	}
	return rc;
}

mnlxt_rt_route_t *mnlxt_rt_route_get(const mnlxt_message_t *message) {
	mnlxt_rt_route_t *route = NULL;
	if (message && message->payload
			&& (RTM_NEWROUTE == message->nlmsg_type || RTM_GETROUTE == message->nlmsg_type
					|| RTM_DELROUTE == message->nlmsg_type)) {
		route = (mnlxt_rt_route_t *)message->payload;
	}
	return route;
}

mnlxt_rt_route_t *mnlxt_rt_route_remove(mnlxt_message_t *message) {
	mnlxt_rt_route_t *route = mnlxt_rt_route_get(message);
	if (NULL != route) {
		message->payload = NULL;
	}
	return route;
}

int mnlxt_rt_route_put(struct nlmsghdr *nlh, const mnlxt_rt_route_t *route) {
	int rc = -1;
	if (route && nlh) {
		struct rtmsg *rtm = mnl_nlmsg_put_extra_header(nlh, sizeof(struct rtmsg));
		int flag = 0x1;
		size_t family_size = (AF_INET == route->family ? sizeof(route->src.in) : sizeof(route->src));
		int i = 0;
		for (; i < MNLXT_RT_ROUTE_MAX; ++i) {
			if (route->prop_flags & flag) {
				switch (i) {
				case MNLXT_RT_ROUTE_FAMILY:
					rtm->rtm_family = route->family;
					break;
				case MNLXT_RT_ROUTE_TABLE:
					rtm->rtm_table = route->table;
					break;
				case MNLXT_RT_ROUTE_TYPE:
					rtm->rtm_type = route->type;
					break;
				case MNLXT_RT_ROUTE_PROTOCOL:
					rtm->rtm_protocol = route->protocol;
					break;
				case MNLXT_RT_ROUTE_SCOPE:
					rtm->rtm_scope = route->scope;
					break;
				case MNLXT_RT_ROUTE_PRIORITY:
					mnl_attr_put_u32(nlh, RTA_PRIORITY, route->priority);
					break;
				case MNLXT_RT_ROUTE_OIFINDEX:
					mnl_attr_put_u32(nlh, RTA_OIF, route->oif_index);
					break;
				case MNLXT_RT_ROUTE_IIFINDEX:
					mnl_attr_put_u32(nlh, RTA_IIF, route->iif_index);
					break;
				case MNLXT_RT_ROUTE_SRC_PREFIX:
					rtm->rtm_src_len = route->src_prefix;
					break;
				case MNLXT_RT_ROUTE_DST_PREFIX:
					rtm->rtm_dst_len = route->dst_prefix;
					break;
				case MNLXT_RT_ROUTE_SRC:
					mnl_attr_put(nlh, RTA_SRC, family_size, &route->src);
					break;
				case MNLXT_RT_ROUTE_DST:
					mnl_attr_put(nlh, RTA_DST, family_size, &route->dst);
					break;
				case MNLXT_RT_ROUTE_GATEWAY:
					mnl_attr_put(nlh, RTA_GATEWAY, family_size, &route->gateway);
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

int mnlxt_rt_route_PUT(struct nlmsghdr *nlh, const void *route, uint16_t nlmsg_type) {
	(void)nlmsg_type;
	return mnlxt_rt_route_put(nlh, (mnlxt_rt_route_t *)route);
}

int mnlxt_rt_route_data(const struct nlmsghdr *nlh, mnlxt_data_t *data) {
	int rc = MNL_CB_ERROR;
	mnlxt_rt_route_t *route = NULL;
	mnlxt_message_t *msg = NULL;

	if (NULL == data) {
		errno = EINVAL;
		goto end;
	}

	if (NULL == nlh) {
		errno = EINVAL;
		data->error_str = "invalid arguments";
		goto end;
	}

	if (RTM_GETROUTE != nlh->nlmsg_type && RTM_NEWROUTE != nlh->nlmsg_type && RTM_DELROUTE != nlh->nlmsg_type) {
		errno = EBADMSG;
		data->error_str = "unsupported message type";
		goto end;
	}

	struct rtmsg *rtm = mnl_nlmsg_get_payload(nlh);

	/* IPv6 and IPv4 support only */
	size_t family_size = 0;
	if (AF_INET == rtm->rtm_family) {
		family_size = sizeof(struct in_addr);
	} else if (AF_INET6 == rtm->rtm_family) {
		family_size = sizeof(struct in6_addr);
	} else {
		rc = MNL_CB_OK;
		goto end;
	}

	route = mnlxt_rt_route_new();
	if (!route) {
		data->error_str = "mnlxt_rt_route_new failed";
		goto end;
	}

	mnlxt_rt_route_set_family(route, rtm->rtm_family);
	mnlxt_rt_route_set_protocol(route, rtm->rtm_protocol);
	mnlxt_rt_route_set_table(route, rtm->rtm_table);
	mnlxt_rt_route_set_type(route, rtm->rtm_type);
	mnlxt_rt_route_set_scope(route, rtm->rtm_scope);
	mnlxt_rt_route_set_dst_prefix(route, rtm->rtm_dst_len);
	mnlxt_rt_route_set_src_prefix(route, rtm->rtm_src_len);

	struct nlattr *attr;
	mnl_attr_for_each(attr, nlh, sizeof(*rtm)) {
		int type = mnl_attr_get_type(attr);
		/* skip unsupported attribute in user-space */
		if (0 > mnl_attr_type_valid(attr, RTA_MAX)) {
			continue;
		}
		switch (type) {
		case RTA_UNSPEC:
			break;
		case RTA_DST:
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, family_size)) {
				data->error_str = "RTA_DST validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_route_set_dst(route, rtm->rtm_family, mnl_attr_get_payload(attr))) {
				data->error_str = "mnlxt_rt_route_set_dst failed";
				goto end;
			}
			break;
		case RTA_SRC:
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, family_size)) {
				data->error_str = "RTA_SRC validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_route_set_src(route, rtm->rtm_family, mnl_attr_get_payload(attr))) {
				data->error_str = "mnlxt_rt_route_set_src failed";
				goto end;
			}
			break;
		case RTA_GATEWAY:
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, family_size)) {
				data->error_str = "RTA_GATEWAY validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_route_set_gateway(route, rtm->rtm_family, mnl_attr_get_payload(attr))) {
				data->error_str = "mnlxt_rt_route_set_gateway failed";
				goto end;
			}
			break;
		case RTA_PRIORITY:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "RTA_PRIORITY validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_route_set_priority(route, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_route_set_priority failed";
				goto end;
			}
			break;
		case RTA_OIF:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "RTA_OIF validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_route_set_oifindex(route, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_route_set_oif_index failed";
				goto end;
			}
			break;
		case RTA_IIF:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "RTA_IIF validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_route_set_iifindex(route, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_route_set_iif_index failed";
				goto end;
			}
			break;
		default:
			break;
		}
	}

	msg = mnlxt_rt_message_new(nlh->nlmsg_type, 0, route);
	if (NULL == msg) {
		data->error_str = "mnlxt_rt_message_new failed";
		goto end;
	}

	mnlxt_data_add(data, msg);
	route = NULL;
	msg = NULL;
	rc = MNL_CB_OK;
end:
	mnlxt_rt_route_free(route);
	mnlxt_message_free(msg);

	return rc;
}

int mnlxt_rt_route_DATA(const struct nlmsghdr *nlh, void *data) {
	return mnlxt_rt_route_data(nlh, (mnlxt_data_t *)data);
}
mnlxt_rt_route_t *mnlxt_rt_route_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator) {
	mnlxt_rt_route_t *route = NULL;
	if (iterator) {
		while ((*iterator = mnlxt_data_iterate(data, *iterator))) {
			if ((route = mnlxt_rt_route_get(*iterator))) {
				break;
			}
		}
	}
	return route;
}

int mnlxt_rt_route_dump(mnlxt_data_t *data, unsigned char family) {
	int rc = -1;
	if (AF_INET != family && AF_INET6 != family && AF_UNSPEC != family) {
		errno = EAFNOSUPPORT;
	} else {
		rc = mnlxt_rt_data_dump(data, RTM_GETROUTE, family);
	}
	return rc;
}

int mnlxt_rt_route_request(mnlxt_rt_route_t *rt_route, uint16_t type, uint16_t flags) {
	int rc = -1;
	mnlxt_message_t *message = mnlxt_rt_route_message(&rt_route, type, flags);
	if (NULL != message) {
		rc = mnlxt_rt_message_request(message);
		mnlxt_rt_route_remove(message);
		mnlxt_message_free(message);
	}
	return rc;
}

mnlxt_message_t *mnlxt_rt_route_message(mnlxt_rt_route_t **route, uint16_t type, uint16_t flags) {
	mnlxt_message_t *message = NULL;
	if (!route || !*route || !(RTM_NEWROUTE == type || RTM_DELROUTE == type)) {
		errno = EINVAL;
	} else if (NULL != (message = mnlxt_rt_message_new(type, flags, *route))) {
		*route = NULL;
	}
	return message;
}
