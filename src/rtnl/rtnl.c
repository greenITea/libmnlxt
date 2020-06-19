/*
 * rtnl.c		Libmnlxt Routing
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>

#include <libmnlxt/rt.h>

#include "internal.h"

static const mnlxt_data_cb_t data_handlers[] = {
	[RTM_NEWLINK] = {"NEWLINK", mnlxt_rt_link_DATA, mnlxt_rt_link_PUT, mnlxt_rt_link_FREE, NLM_F_CREATE},
	[RTM_DELLINK] = {"DELLINK", mnlxt_rt_link_DATA, mnlxt_rt_link_PUT, mnlxt_rt_link_FREE, 0},
	[RTM_GETLINK] = {"GETLINK", mnlxt_rt_link_DATA, mnlxt_rt_link_PUT, mnlxt_rt_link_FREE, 0},
	[RTM_SETLINK] = {"SETLINK", mnlxt_rt_link_DATA, mnlxt_rt_link_PUT, mnlxt_rt_link_FREE, NLM_F_CREATE | NLM_F_REPLACE},

	[RTM_NEWADDR] = {"NEWADDR", mnlxt_rt_addr_DATA, mnlxt_rt_addr_PUT, mnlxt_rt_addr_FREE, NLM_F_CREATE},
	[RTM_DELADDR] = {"DELADDR", mnlxt_rt_addr_DATA, mnlxt_rt_addr_PUT, mnlxt_rt_addr_FREE, 0},
	[RTM_GETADDR] = {"GETADDR", mnlxt_rt_addr_DATA, mnlxt_rt_addr_PUT, mnlxt_rt_addr_FREE, 0},

	[RTM_NEWROUTE] = {"NEWROUTE", mnlxt_rt_route_DATA, mnlxt_rt_route_PUT, mnlxt_rt_route_FREE, NLM_F_CREATE},
	[RTM_DELROUTE] = {"DELROUTE", mnlxt_rt_route_DATA, mnlxt_rt_route_PUT, mnlxt_rt_route_FREE, 0},
	[RTM_GETROUTE] = {"GETROUTE", mnlxt_rt_route_DATA, mnlxt_rt_route_PUT, mnlxt_rt_route_FREE, 0},

	[RTM_NEWRULE] = {"NEWRULE", mnlxt_rt_rule_DATA, mnlxt_rt_rule_PUT, mnlxt_rt_rule_FREE, NLM_F_CREATE},
	[RTM_DELRULE] = {"DELRULE", mnlxt_rt_rule_DATA, mnlxt_rt_rule_PUT, mnlxt_rt_rule_FREE, 0},
	[RTM_GETRULE] = {"GETRULE", mnlxt_rt_rule_DATA, mnlxt_rt_rule_PUT, mnlxt_rt_rule_FREE, 0},
};

static const size_t data_nhandlers = MNL_ARRAY_SIZE(data_handlers);

int mnlxt_rt_connect(mnlxt_handle_t *handle, int group) {
	int rc = mnlxt_connect(handle, NETLINK_ROUTE, group);

	if (!rc) {
		handle->data_handlers = data_handlers;
		handle->data_nhandlers = data_nhandlers;
	}

	return rc;
}

int mnlxt_rt_data_dump(mnlxt_data_t *data, int type, unsigned char family) {
	int rc = -1;

	if (NULL == data) {
		errno = EINVAL;

	} else {
		struct nlmsghdr *nlh;
		struct rtgenmsg *rtg;
		char buf[sizeof(struct nlmsghdr) + sizeof(struct rtgenmsg) + 64];

		if (NULL != (nlh = mnl_nlmsg_put_header(buf))) {
			nlh->nlmsg_type = type;
		}

		if (NULL != (rtg = mnl_nlmsg_put_extra_header(nlh, sizeof(struct rtgenmsg)))) {
			rtg->rtgen_family = family;
		}

		data->handlers = data_handlers;
		data->nhandlers = data_nhandlers;

		rc = mnlxt_data_dump(data, NETLINK_ROUTE, nlh);
	}

	return rc;
}

static const mnlxt_data_cb_t *mnlxt_rt_type_handler(uint16_t type) {
	const mnlxt_data_cb_t *data_cb = NULL;

	if (data_nhandlers < type) {
		errno = EBADMSG;
	} else {
		data_cb = &data_handlers[type];
		if (NULL == data_cb->name) {
			data_cb = NULL;
			errno = EBADMSG;
		}
	}

	return data_cb;
}

int mnlxt_rt_message_request(mnlxt_message_t *message) {
	const mnlxt_data_cb_t *data_cb;
	int rc = -1;

	if (NULL == message) {
		errno = EINVAL;
	} else if (NULL != (data_cb = mnlxt_rt_type_handler(message->nlmsg_type))) {
		message->handler = data_cb;
		rc = mnlxt_message_request(message, NETLINK_ROUTE);
	}

	return rc;
}

mnlxt_message_t *mnlxt_rt_message_new(uint16_t type, uint16_t flags, void *payload) {
	const mnlxt_data_cb_t *data_cb;
	mnlxt_message_t *msg = NULL;

	if (NULL == (data_cb = mnlxt_rt_type_handler(type))) {
		errno = EINVAL;
	} else if (NULL != (msg = mnlxt_message_new())) {
		msg->nlmsg_type = type;
		msg->flags = flags;
		msg->payload = payload;
		msg->handler = data_cb;
	}

	return msg;
}
