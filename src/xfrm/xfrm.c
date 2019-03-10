/*
 * xfrm.c		Libmnlxt Xfrm/IPsec
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>

#include <libmnlxt/xfrm.h>

#include "internal.h"

static const mnlxt_data_cb_t data_handlers[] = {
	[XFRM_MSG_NEWPOLICY]
	= {"NEWPOLICY", mnlxt_xfrm_policy_DATA, mnlxt_xfrm_policy_PUT, mnlxt_xfrm_policy_FREE, NLM_F_CREATE},
	[XFRM_MSG_DELPOLICY] = {"DELPOLICY", mnlxt_xfrm_policy_DATA, mnlxt_xfrm_policy_PUT, mnlxt_xfrm_policy_FREE, 0},
	[XFRM_MSG_GETPOLICY] = {"GETPOLICY", mnlxt_xfrm_policy_DATA, mnlxt_xfrm_policy_PUT, mnlxt_xfrm_policy_FREE, 0},
	[XFRM_MSG_UPDPOLICY]
	= {"UPDPOLICY", mnlxt_xfrm_policy_DATA, mnlxt_xfrm_policy_PUT, mnlxt_xfrm_policy_FREE, NLM_F_CREATE | NLM_F_REPLACE},
};

static const size_t data_nhandlers = MNL_ARRAY_SIZE(data_handlers);

int mnlxt_xfrm_connect(mnlxt_handle_t *handle, int group) {
	int rc = mnlxt_connect(handle, NETLINK_XFRM, group);

	if (!rc) {
		handle->data_handlers = data_handlers;
		handle->data_nhandlers = data_nhandlers;
	}

	return rc;
}

int mnlxt_xfrm_data_dump(mnlxt_data_t *data, int type) {
	int rc = -1;

	if (NULL == data) {
		errno = EINVAL;

	} else {
		struct nlmsghdr *nlh;
		char buf[sizeof(struct nlmsghdr) + sizeof(struct xfrm_userpolicy_info) + 64];
		nlh = mnl_nlmsg_put_header(buf);
		nlh->nlmsg_type = type;

		data->handlers = data_handlers;
		data->nhandlers = data_nhandlers;

		rc = mnlxt_data_dump(data, NETLINK_XFRM, nlh);
	}

	return rc;
}

static const mnlxt_data_cb_t *mnlxt_xfrm_type_handler(uint16_t type) {
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

int mnlxt_xfrm_message_request(mnlxt_message_t *message) {
	const mnlxt_data_cb_t *data_cb;
	int rc = -1;

	if (NULL == message) {
		errno = EINVAL;

	} else if (NULL != (data_cb = mnlxt_xfrm_type_handler(message->nlmsg_type))) {
		message->handler = data_cb;
		rc = mnlxt_message_request(message, NETLINK_XFRM);
	}

	return rc;
}

mnlxt_message_t *mnlxt_xfrm_message_new(uint16_t type, void *payload) {
	const mnlxt_data_cb_t *data_cb;
	mnlxt_message_t *msg = NULL;

	if (NULL != (data_cb = mnlxt_xfrm_type_handler(type))) {
		if (NULL != (msg = mnlxt_message_new())) {
			msg->nlmsg_type = type;
			msg->payload = payload;
			msg->handler = data_cb;
		} else {
			errno = ENOMEM;
		}
	}

	return msg;
}
