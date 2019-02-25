#include <errno.h>
#include <linux/rtnetlink.h>
#include <linux/xfrm.h>
#include <stdlib.h>
#include <string.h>

#include "../include/libmnlxt/data.h"
#include "../include/libmnlxt/rt_addr.h"
#include "../include/libmnlxt/rt_link.h"
#include "../include/libmnlxt/rt_route.h"
#include "../include/libmnlxt/rt_rule.h"
#include "../include/libmnlxt/xfrm_policy.h"

typedef void (*mnlxt_data_free_cb_t)(void *);
typedef int (*mnlxt_data_put_cb_t)(struct nlmsghdr *, void *, uint16_t);

typedef struct {
	const char *name;
	mnl_cb_t parse;
	mnlxt_data_put_cb_t put;
	mnlxt_data_free_cb_t free;
	uint16_t flags;
} mnlxt_data_cb_t;

static mnlxt_data_cb_t data_cb[] = {
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

	[XFRM_MSG_NEWPOLICY]
	= {"NEWPOLICY", mnlxt_xfrm_policy_DATA, mnlxt_xfrm_policy_PUT, mnlxt_xfrm_policy_FREE, NLM_F_CREATE},
	[XFRM_MSG_DELPOLICY] = {"DELPOLICY", mnlxt_xfrm_policy_DATA, mnlxt_xfrm_policy_PUT, mnlxt_xfrm_policy_FREE, 0},
	[XFRM_MSG_GETPOLICY] = {"GETPOLICY", mnlxt_xfrm_policy_DATA, mnlxt_xfrm_policy_PUT, mnlxt_xfrm_policy_FREE, 0},
	[XFRM_MSG_UPDPOLICY]
	= {"UPDPOLICY", mnlxt_xfrm_policy_DATA, mnlxt_xfrm_policy_PUT, mnlxt_xfrm_policy_FREE, NLM_F_CREATE | NLM_F_REPLACE},
};

static const size_t data_cb_array_len = sizeof(data_cb) / sizeof(mnlxt_data_cb_t);

const char *mnlxt_message_type(mnlxt_message_t *msg) {
	char *type = NULL;
	if (msg && msg->nlmsg_type < data_cb_array_len) {
		type = (char *)data_cb[msg->nlmsg_type].name;
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
			if (message->nlmsg_type < data_cb_array_len) {
				free_foo = data_cb[message->nlmsg_type].free;
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
	if (!nlh) {
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
						if (data) {
							data->error_str = handle.error_str;
						}
						break;
					} else {
						/* no data */
						rc = 0;
						break;
					}
				}
			} else if (handle.error_str && data) {
				data->error_str = handle.error_str;
			}
			mnlxt_disconnect(&handle);
			if (mnlxt_buf.buf) {
				free(mnlxt_buf.buf);
			}
		}
	}
	return rc;
}

int mnlxt_message_request(mnlxt_message_t *message, int bus) {
	int rc = -1;
	if (!message) {
		errno = EINVAL;
	} else {
		struct nlmsghdr *nlh = mnlxt_msghdr_create(message);
		if (nlh) {
			nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | data_cb[message->nlmsg_type].flags;
			rc = mnlxt_request(nlh, bus, NULL);
			mnlxt_msghdr_free(nlh);
		}
	}

	return rc;
}

struct nlmsghdr *mnlxt_msghdr_create(mnlxt_message_t *message) {
	struct nlmsghdr *nlh_msg = NULL;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = message->nlmsg_type;
	mnlxt_data_put_cb_t cb_foo = NULL;
	if (data_cb_array_len < nlh->nlmsg_type) {
		errno = EBADMSG;
	} else {
		cb_foo = data_cb[nlh->nlmsg_type].put;
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
	mnlxt_data_t *mnlxt_data = NULL;
	mnl_cb_t cb_foo = NULL;
	if (!nlh || !data) {
		errno = EINVAL;
	} else {
		mnlxt_data = (mnlxt_data_t *)data;
		if (data_cb_array_len < nlh->nlmsg_type) {
			errno = EBADMSG;
			mnlxt_data->error_str = "unsupported message type";
		} else {
			cb_foo = data_cb[nlh->nlmsg_type].parse;
			if (cb_foo) {
				rc = cb_foo(nlh, data);
			} else {
				/* unsupported message type? */
				rc = MNL_CB_OK;
			}
		}
	}
	return rc;
}

int mnlxt_data_parse(mnlxt_data_t *data, mnlxt_buffer_t *buffer) {
	int rc = -1;
	if (buffer && buffer->buf && buffer->len) {
		int ret;
		if (data) {
			ret = mnl_cb_run(buffer->buf, buffer->len, buffer->seq, buffer->portid, mnlxt_data_cb, data);
		} else {
			ret = mnl_cb_run(buffer->buf, buffer->len, buffer->seq, buffer->portid, NULL, NULL);
		}
		if (MNL_CB_ERROR == ret) {
			if (data && !data->error_str) {
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
	mnlxt_message_t *msg;
	while ((msg = mnlxt_data_remove(data, NULL))) {
		mnlxt_message_free(msg);
	}
}

int mnlxt_data_dump(mnlxt_data_t *data, int bus, struct nlmsghdr *nlh) {
	int rc = -1;
	if (!nlh || !data) {
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
