/*
 * mnlxt_data.c		Libmnlxt Data
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

#include "libmnlxt/data.h"

const char *mnlxt_message_type(const mnlxt_message_t *msg) {
	const char *type = NULL;
	if (msg && msg->handler) {
		type = msg->handler->name;
	}
	return type;
}

mnlxt_message_t *mnlxt_message_new() {
	return calloc(1, sizeof(mnlxt_message_t));
}

void mnlxt_message_free(mnlxt_message_t *message) {
	if (message) {
		if (message->payload) {
			mnlxt_data_free_cb_t free_foo = NULL;
			if (message->handler) {
				free_foo = message->handler->free;
			}
			if (free_foo) {
				free_foo(message->payload);
			} else {
				free(message->payload);
			}
		}
		free(message);
	}
}

static int mnlxt_request(struct nlmsghdr *nlh, int bus, mnlxt_data_t *data) {
	int rc = -1;
	if (NULL == nlh) {
		errno = EINVAL;
	} else {
		mnlxt_handle_t handle = {};
		mnlxt_buffer_t mnlxt_buf = {};
		if (0 == mnlxt_connect(&handle, bus, 0)) {
			if (0 < mnlxt_send(&handle, nlh)) {
				while (1) {
					/* get data or acknowledge */
					int ret = mnlxt_receive(&handle, &mnlxt_buf);
					if (0 < ret) {
						/* parse answer */
						ret = mnlxt_data_parse(data, &mnlxt_buf);
						if (0 != ret) {
							/* parsing stop or parsing error */
							if (1 == ret) {
								rc = 0;
							}
							mnlxt_buffer_clean(&mnlxt_buf);
							break;
						}
						mnlxt_buffer_clean(&mnlxt_buf);
					} else if (0 > ret) {
						/* an error by receiving message */
						if (NULL != data) {
							data->error_str = handle.error_str;
						}
						break;
					} else {
						/* no data */
						rc = 0;
						break;
					}
				}
			} else if (NULL != handle.error_str && NULL != data) {
				data->error_str = handle.error_str;
			}
			mnlxt_disconnect(&handle);
			if (NULL != mnlxt_buf.buf) {
				free(mnlxt_buf.buf);
			}
		}
	}
	return rc;
}

int mnlxt_message_request(const mnlxt_message_t *message, int bus) {
	int rc = -1;
	if (!message) {
		errno = EINVAL;
	} else {
		struct nlmsghdr *nlh = mnlxt_msghdr_create(message);
		if (nlh) {
			nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
			if (message->flags) {
				nlh->nlmsg_flags |= message->flags;
			} else if (message->handler) {
				nlh->nlmsg_flags |= message->handler->flags;
			}
			rc = mnlxt_request(nlh, bus, NULL);
			mnlxt_msghdr_free(nlh);
		}
	}

	return rc;
}

struct nlmsghdr *mnlxt_msghdr_create(const mnlxt_message_t *message) {
	struct nlmsghdr *nlh_msg = NULL;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = message->nlmsg_type;
	mnlxt_data_put_cb_t cb_foo = NULL;
	if (NULL == message->handler) {
		errno = EBADMSG;
	} else {
		cb_foo = message->handler->put;
		if (cb_foo) {
			if (0 == cb_foo(nlh, message->payload, message->nlmsg_type)) {
				nlh_msg = malloc(nlh->nlmsg_len);
				if (nlh_msg) {
					memcpy(nlh_msg, buf, nlh->nlmsg_len);
				}
			}
		} else {
			/* unsupported message type? */
			errno = EBADMSG;
		}
	}
	return nlh_msg;
}

void mnlxt_msghdr_free(struct nlmsghdr *msghdr) {
	if (msghdr) {
		free(msghdr);
	}
}

static int mnlxt_data_cb(const struct nlmsghdr *nlh, void *data) {
	int rc = MNL_CB_ERROR;
	mnlxt_data_t *mnlxt_data = (mnlxt_data_t *)data;
	mnl_cb_t cb_foo = NULL;
	if (NULL == nlh || NULL == mnlxt_data) {
		errno = EINVAL;
	} else if (NLM_F_DUMP_INTR & nlh->nlmsg_flags) {
		mnlxt_data->error_str = "dump interruption";
		errno = EINTR;
	} else if (mnlxt_data->nhandlers > nlh->nlmsg_type
						 && NULL != (cb_foo = mnlxt_data->handlers[nlh->nlmsg_type].parse)) {
		rc = cb_foo(nlh, data);
	} else if (NLMSG_ERROR == nlh->nlmsg_type) {
		struct nlmsgerr *err = mnl_nlmsg_get_payload(nlh);
		if (mnl_nlmsg_size(sizeof(struct nlmsgerr)) > nlh->nlmsg_len) {
			mnlxt_data->error_str = "invalid message";
			errno = EBADMSG;
		} else {
			if (0 != err->error) {
				strerror_r(-err->error, mnlxt_data->error_buf, sizeof(mnlxt_data->error_buf));
				mnlxt_data->error_str = mnlxt_data->error_buf;
				errno = -err->error;
			} else {
				rc = MNL_CB_STOP;
			}
		}
	} else if (NLMSG_NOOP == nlh->nlmsg_type || NLMSG_OVERRUN == nlh->nlmsg_type) {
		rc = MNL_CB_OK;
	} else if (NLMSG_DONE == nlh->nlmsg_type) {
		rc = MNL_CB_STOP;
	} else {
		mnlxt_data->error_str = "unsupported message type";
		errno = EBADMSG;
		/* ignore unsupported messages and continue */
		rc = MNL_CB_OK;
	}
	return rc;
}

int mnlxt_data_parse(mnlxt_data_t *data, mnlxt_buffer_t *buffer) {
	int rc = -1;
	if (NULL != buffer && NULL != buffer->buf && 0 != buffer->len) {
		int ret;
		if (NULL != data) {
			if (NULL == data->handlers) {
				data->handlers = buffer->data_handlers;
				data->nhandlers = buffer->data_nhandlers;
			}
			ret = mnl_cb_run(buffer->buf, buffer->len, buffer->seq, buffer->portid, mnlxt_data_cb, data);
		} else {
			ret = mnl_cb_run(buffer->buf, buffer->len, buffer->seq, buffer->portid, NULL, NULL);
		}
		if (MNL_CB_ERROR == ret) {
			if (NULL != data && NULL == data->error_str) {
				data->error_str = "mnl_cb_run failed";
			}
		} else if (MNL_CB_STOP == ret) {
			rc = 1;
		} else {
			rc = 0;
		}
	} else {
		errno = EINVAL;
	}
	return rc;
}

void mnlxt_buffer_clean(mnlxt_buffer_t *buffer) {
	if (buffer) {
		if (buffer->buf) {
			free(buffer->buf);
		}
		memset(buffer, 0, sizeof(mnlxt_buffer_t));
	}
}

void mnlxt_data_clean(mnlxt_data_t *data) {
	if (NULL != data) {
		mnlxt_message_t *msg;
		while (NULL != (msg = mnlxt_data_remove(data, NULL))) {
			mnlxt_message_free(msg);
		}
		memset(data, 0, sizeof(*data));
	}
}

int mnlxt_data_dump(mnlxt_data_t *data, int bus, struct nlmsghdr *nlh) {
	int rc = -1;
	if (NULL == nlh || NULL == data) {
		errno = EINVAL;
	} else {
		nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
		rc = mnlxt_request(nlh, bus, data);
	}
	return rc;
}

mnlxt_message_t *mnlxt_data_iterate(mnlxt_data_t *data, mnlxt_message_t *message) {
	mnlxt_message_t *msg = NULL;
	if (data) {
		if (message) {
			msg = message->next;
		} else {
			msg = data->first;
		}
	} else {
		errno = EINVAL;
	}
	return msg;
}

mnlxt_message_t *mnlxt_data_remove(mnlxt_data_t *data, mnlxt_message_t *message) {
	mnlxt_message_t *msg = NULL;
	if (data) {
		msg = data->first;
		if (message && message != msg) {
			while (msg && msg->next != message) {
				msg = msg->next;
			}
			if (msg) {
				if (data->last == message) {
					data->last = msg;
				}
				msg->next = message->next;
				msg = message;
			}
		} else if (msg) {
			if (data->last == msg) {
				data->last = NULL;
			}
			data->first = msg->next;
		}
	} else {
		errno = EINVAL;
	}
	if (msg) {
		msg->next = NULL;
	}
	return msg;
}

void mnlxt_data_add(mnlxt_data_t *data, mnlxt_message_t *message) {
	if (data && message) {
		if (data->last) {
			data->last->next = message;
		} else {
			data->first = message;
		}
		data->last = message;
		message->next = NULL;
	} else {
		errno = EINVAL;
	}
}
