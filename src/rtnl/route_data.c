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
#include <linux/rtnetlink.h>
#include <string.h>

#include <libmnlxt/rt.h>

int mnlxt_rt_route_match(mnlxt_rt_route_t *route, mnlxt_rt_route_t *match) {
	int i = -1;
	if (route && match) {
		int flag = 0x1;
		size_t family_size = (AF_INET == route->family ? sizeof(route->src.in) : sizeof(route->src));
		for (i = 0; i < MNLXT_RT_ROUTE_MAX; ++i) {
			if (match->prop_flags & flag) {
				if (route->prop_flags & flag) {
					switch (i) {
					case MNLXT_RT_ROUTE_FAMILY:
						if (route->family != match->family) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_TABLE:
						if (route->table != match->table) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_TYPE:
						if (route->type != match->type) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_PROTOCOL:
						if (route->protocol != match->protocol) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_SCOPE:
						if (route->scope != match->scope) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_PRIORITY:
						if (route->priority != match->priority) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_OIFINDEX:
						if (route->oif_index != match->oif_index) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_SRC_PREFIX:
						if (route->src_prefix != match->src_prefix) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_DST_PREFIX:
						if (route->dst_prefix != match->dst_prefix) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_SRC:
						if (0 != memcmp(&route->src, &match->src, family_size)) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_DST:
						if (0 != memcmp(&route->dst, &match->dst, family_size)) {
							goto failed;
						}
						break;
					case MNLXT_RT_ROUTE_GATEWAY:
						if (0 != memcmp(&route->gateway, &match->gateway, family_size)) {
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

mnlxt_rt_route_t *mnlxt_rt_route_get(mnlxt_message_t *message) {
	mnlxt_rt_route_t *route = NULL;
	if (message && message->payload
			&& (RTM_NEWROUTE == message->nlmsg_type || RTM_GETROUTE == message->nlmsg_type
					|| RTM_DELROUTE == message->nlmsg_type)) {
		route = (mnlxt_rt_route_t *)message->payload;
	}
	return route;
}

int mnlxt_rt_route_put(struct nlmsghdr *nlh, mnlxt_rt_route_t *route) {
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

int mnlxt_rt_route_PUT(struct nlmsghdr *nlh, void *route, uint16_t nlmsg_type) {
	(void)nlmsg_type;
	return mnlxt_rt_route_put(nlh, (mnlxt_rt_route_t *)route);
}

int mnlxt_rt_route_data(const struct nlmsghdr *nlh, mnlxt_data_t *data) {
	int rc = MNL_CB_ERROR;
	mnlxt_rt_route_t *route = NULL;
	mnlxt_message_t *msg = NULL;

	if (!data) {
		errno = EINVAL;
		goto end;
	}

	if (!nlh) {
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

	msg = mnlxt_rt_message_new(nlh->nlmsg_type, route);
	if (!msg) {
		data->error_str = "mnlxt_rt_message_new failed";
		goto end;
	}

	mnlxt_data_add(data, msg);
	route = NULL;
	msg = NULL;
	rc = MNL_CB_OK;
end:
	if (route) {
		mnlxt_rt_route_free(route);
	}
	if (msg) {
		mnlxt_message_free(msg);
	}
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

mnlxt_message_t *mnlxt_rt_route_message(mnlxt_rt_route_t **route, uint16_t type) {
	mnlxt_message_t *message = NULL;
	if (!route || !*route || !(RTM_NEWROUTE == type || RTM_DELROUTE == type)) {
		errno = EINVAL;
	} else {
		message = mnlxt_rt_message_new(type, *route);
		if (message) {
			*route = NULL;
		}
	}
	return message;
}
