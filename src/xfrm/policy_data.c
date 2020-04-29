/*
 * policy_data.c		Libmnlxt Xfrm/IPsec Policy
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

#include <libmnlxt/xfrm.h>

static int mnlxt_xfrm_policy_cmp(const mnlxt_xfrm_policy_t *policy1, const mnlxt_xfrm_policy_t *policy2,
																 mnlxt_xfrm_policy_data_t data) {
	int rc = data + 1;
	size_t addr_size;
	switch (data) {
	case MNLXT_XFRM_POLICY_FAMILY:
		if (policy1->family != policy2->family) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_PROTO:
		if (policy1->proto != policy2->proto) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_SRC_PREFIXLEN:
		if (policy1->src.prefixlen != policy2->src.prefixlen) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_DST_PREFIXLEN:
		if (policy1->dst.prefixlen != policy2->dst.prefixlen) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_SRC_ADDR:
		addr_size = (AF_INET == policy1->family ? sizeof(policy1->src.addr.in) : sizeof(policy1->src.addr));
		if (0 != memcmp(&policy1->src.addr, &policy2->src.addr, addr_size)) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_DST_ADDR:
		addr_size = (AF_INET == policy1->family ? sizeof(policy1->dst.addr.in) : sizeof(policy1->dst.addr));
		if (0 != memcmp(&policy1->dst.addr, &policy2->dst.addr, addr_size)) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_SRC_PORT:
		if (policy1->src.port != policy2->src.port) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_DST_PORT:
		if (policy1->dst.port != policy2->dst.port) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_INDEX:
		if (policy1->index != policy2->index) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_IFINDEX:
		if (policy1->if_index != policy2->if_index) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_PRIO:
		if (policy1->priority != policy2->priority) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_ACTION:
		if (policy1->action != policy2->action) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_DIR:
		if (policy1->dir != policy2->dir) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_MARK:
		if (0 != memcmp(&policy1->mark, &policy2->mark, sizeof(policy1->mark))) {
			goto failed;
		}
		break;
	case MNLXT_XFRM_POLICY_TMPLS:
		/*TODO*/
		break;
	}
	rc = 0;
failed:
	return rc;
}

int mnlxt_xfrm_policy_match(const mnlxt_xfrm_policy_t *policy, const mnlxt_xfrm_policy_t *match) {
	return mnlxt_xfrm_policy_compare(policy, match, match->prop_flags);
}

int mnlxt_xfrm_policy_compare(const mnlxt_xfrm_policy_t *policy1, const mnlxt_xfrm_policy_t *policy2, uint64_t filter) {
	int rc = -1, i;
	if (NULL == policy1 || NULL == policy2) {
		errno = EINVAL;
	} else {
		for (i = 0; i < MNLXT_XFRM_POLICY_MAX; ++i) {
			uint64_t flag = MNLXT_FLAG(i);
			if (0 == (flag & filter)) {
				continue;
			}
			if (0 == (policy1->prop_flags & flag)) {
				if (0 == (policy2->prop_flags & flag)) {
					/* both not set */
					continue;
				}
				goto failed;
			} else if (0 == (policy2->prop_flags & flag)) {
				goto failed;
			} else if (0 != mnlxt_xfrm_policy_cmp(policy1, policy2, i)) {
				goto failed;
			}
		}
		return 0;
	failed:
		rc = ++i;
	}
	return rc;
}

mnlxt_xfrm_policy_t *mnlxt_xfrm_policy_get(const mnlxt_message_t *message) {
	mnlxt_xfrm_policy_t *policy = NULL;
	if (message && message->payload
			&& (XFRM_MSG_NEWPOLICY == message->nlmsg_type || XFRM_MSG_UPDPOLICY == message->nlmsg_type
					|| XFRM_MSG_GETPOLICY == message->nlmsg_type || XFRM_MSG_DELPOLICY == message->nlmsg_type)) {
		policy = (mnlxt_xfrm_policy_t *)message->payload;
	}
	return policy;
}

mnlxt_xfrm_policy_t *mnlxt_xfrm_policy_remove(mnlxt_message_t *message) {
	mnlxt_xfrm_policy_t *policy = mnlxt_xfrm_policy_get(message);
	if (NULL != policy) {
		message->payload = NULL;
	}
	return policy;
}

int mnlxt_xfrm_policy_put(struct nlmsghdr *nlh, const mnlxt_xfrm_policy_t *policy, uint16_t nlmsg_type) {
	int rc = -1;
	uint8_t *dir = NULL;
	uint32_t *index = NULL;
	struct xfrm_userpolicy_info *xpinfo = NULL;
	struct xfrm_userpolicy_id *xpid = NULL;
	struct xfrm_selector *sel = NULL;
	struct xfrm_mark mark = {0};

	if (!policy || !nlh) {
		errno = EINVAL;
		goto failed;
	}
	if (XFRM_MSG_GETPOLICY == nlh->nlmsg_type || XFRM_MSG_NEWPOLICY == nlh->nlmsg_type
			|| XFRM_MSG_UPDPOLICY == nlh->nlmsg_type) {
		xpinfo = mnl_nlmsg_put_extra_header(nlh, sizeof(struct xfrm_userpolicy_info));
		sel = &xpinfo->sel;
		index = &xpinfo->index;
		dir = &xpinfo->dir;
	} else if (XFRM_MSG_DELPOLICY == nlh->nlmsg_type) {
		xpid = mnl_nlmsg_put_extra_header(nlh, sizeof(struct xfrm_userpolicy_id));
		sel = &xpid->sel;
		index = &xpid->index;
		dir = &xpid->dir;
	} else {
		errno = EINVAL;
		goto failed;
	}
	int i;
	int flag = 0x1;
	size_t addr_size = (AF_INET == policy->family ? sizeof(policy->src.addr.in) : sizeof(policy->src.addr));
	for (i = 0; i < MNLXT_XFRM_POLICY_MAX; ++i) {
		if (policy->prop_flags & flag) {
			switch (i) {
			case MNLXT_XFRM_POLICY_FAMILY:
				sel->family = policy->family;
				break;
			case MNLXT_XFRM_POLICY_PROTO:
				sel->proto = policy->proto;
				break;
			case MNLXT_XFRM_POLICY_SRC_PREFIXLEN:
				sel->prefixlen_s = policy->src.prefixlen;
				break;
			case MNLXT_XFRM_POLICY_DST_PREFIXLEN:
				sel->prefixlen_d = policy->dst.prefixlen;
				break;
			case MNLXT_XFRM_POLICY_SRC_ADDR:
				memcpy(&sel->saddr, &policy->src.addr, addr_size);
				break;
			case MNLXT_XFRM_POLICY_DST_ADDR:
				memcpy(&sel->daddr, &policy->dst.addr, addr_size);
				break;
			case MNLXT_XFRM_POLICY_SRC_PORT:
				sel->sport = htons(policy->src.port);
				sel->sport_mask = -1;
				break;
			case MNLXT_XFRM_POLICY_DST_PORT:
				sel->dport = htons(policy->dst.port);
				sel->dport_mask = -1;
				break;
			case MNLXT_XFRM_POLICY_INDEX:
				*index = policy->index;
				break;
			case MNLXT_XFRM_POLICY_IFINDEX:
				sel->ifindex = policy->if_index;
				break;
			case MNLXT_XFRM_POLICY_PRIO:
				if (xpinfo) {
					xpinfo->priority = policy->priority;
				}
				break;
			case MNLXT_XFRM_POLICY_ACTION:
				if (xpinfo) {
					xpinfo->action = policy->action;
				}
				break;
			case MNLXT_XFRM_POLICY_DIR:
				*dir = policy->dir;
				break;
			case MNLXT_XFRM_POLICY_MARK:
				mark.m = policy->mark.mask;
				mark.v = policy->mark.value;
				mnl_attr_put(nlh, XFRMA_MARK, sizeof(struct xfrm_mark), &mark);
				break;
			case MNLXT_XFRM_POLICY_TMPLS:
				/*TODO:*/
				break;
			}
		}
		flag <<= 1;
	}
	rc = 0;
failed:
	return rc;
}

int mnlxt_xfrm_policy_PUT(struct nlmsghdr *nlh, const void *policy, uint16_t nlmsg_type) {
	return mnlxt_xfrm_policy_put(nlh, (mnlxt_xfrm_policy_t *)policy, nlmsg_type);
}

static void mnlxt_xfrm_policy_selector_set(mnlxt_xfrm_policy_t *policy, struct xfrm_selector *sel) {
	if (policy && sel) {
		uint8_t family = sel->family;
		if (AF_UNSPEC == family) {
			family = AF_INET;
		}
		if (sel->proto) {
			mnlxt_xfrm_policy_set_proto(policy, sel->proto);
		}
		if (sel->dport_mask) {
			mnlxt_xfrm_policy_set_dst_port(policy, ntohs(sel->dport));
		}
		if (sel->sport_mask) {
			mnlxt_xfrm_policy_set_src_port(policy, ntohs(sel->sport));
		}
		mnlxt_xfrm_policy_set_src_addr(policy, family, (mnlxt_inet_addr_t *)&sel->saddr);
		mnlxt_xfrm_policy_set_src_prefixlen(policy, sel->prefixlen_s);
		mnlxt_xfrm_policy_set_dst_addr(policy, family, (mnlxt_inet_addr_t *)&sel->daddr);
		mnlxt_xfrm_policy_set_dst_prefixlen(policy, sel->prefixlen_d);
		if (sel->ifindex) {
			mnlxt_xfrm_policy_set_ifindex(policy, sel->ifindex);
		}
	}
}

static int mnlxt_xfrm_policy_tmpl(mnlxt_xfrm_policy_t *policy, uint16_t num, struct xfrm_user_tmpl tmpls[]) {
	int rc = -1;
	if (0 < num && tmpls && policy) {
		mnlxt_xfrm_tmpl_t *conns = calloc(num, sizeof(mnlxt_xfrm_tmpl_t));
		if (conns) {
			int i = 0;
			struct xfrm_user_tmpl *tmpl = tmpls;
			mnlxt_xfrm_tmpl_t *conn = conns;
			for (; i < num; ++i, ++tmpl, ++conn) {
				conn->family = tmpl->family;
				if (AF_INET == tmpl->family || AF_INET6 == tmpl->family) {
					memcpy(&conn->src, &tmpl->saddr, sizeof(conn->src));
					memcpy(&conn->dst, &tmpl->id.daddr, sizeof(conn->dst));
				}
				conn->mode = tmpl->mode;
				conn->proto = tmpl->id.proto;
				conn->spi = tmpl->id.spi;
				conn->share = tmpl->share;
				conn->reqid = tmpl->reqid;
			}
			policy->tmpls = conns;
			policy->tmpl_num = num;
			rc = 0;
		}
	}
	return rc;
}

int mnlxt_xfrm_policy_data(const struct nlmsghdr *nlh, mnlxt_data_t *data) {
	int rc = MNL_CB_ERROR;
	uint16_t attr_len = 0;
	size_t payload_size = 0;
	mnlxt_message_t *msg = NULL;
	mnlxt_xfrm_policy_t *policy = NULL;
	struct xfrm_mark *mark = NULL;
	struct xfrm_user_tmpl *tmpls = NULL;
	struct xfrm_userpolicy_type *upt = NULL;

	if (!data) {
		errno = EINVAL;
		goto end;
	}

	if (!nlh) {
		errno = EINVAL;
		data->error_str = "invalid arguments";
		goto end;
	}

	struct xfrm_userpolicy_info *xpinfo = NULL;
	struct xfrm_userpolicy_id *xpid = NULL;

	if (XFRM_MSG_GETPOLICY == nlh->nlmsg_type || XFRM_MSG_NEWPOLICY == nlh->nlmsg_type
			|| XFRM_MSG_UPDPOLICY == nlh->nlmsg_type) {
		xpinfo = mnl_nlmsg_get_payload(nlh);
		payload_size = sizeof(struct xfrm_userpolicy_info);
	} else if (XFRM_MSG_DELPOLICY == nlh->nlmsg_type) {
		xpid = mnl_nlmsg_get_payload(nlh);
		payload_size = sizeof(struct xfrm_userpolicy_id);
	} else {
		errno = EBADMSG;
		data->error_str = "unsupported message type";
		goto end;
	}

	policy = mnlxt_xfrm_policy_new();
	if (!policy) {
		data->error_str = "mnlxt_xfrm_policy_new failed";
		goto end;
	}

	if (xpinfo) {
		mnlxt_xfrm_policy_selector_set(policy, &xpinfo->sel);
		mnlxt_xfrm_policy_set_index(policy, xpinfo->index);
		mnlxt_xfrm_policy_set_dir(policy, xpinfo->dir);
		mnlxt_xfrm_policy_set_action(policy, xpinfo->action);
		mnlxt_xfrm_policy_set_priority(policy, xpinfo->priority);
	} else if (xpid) {
		mnlxt_xfrm_policy_selector_set(policy, &xpid->sel);
		mnlxt_xfrm_policy_set_index(policy, xpid->index);
		mnlxt_xfrm_policy_set_dir(policy, xpid->dir);
	}

	struct nlattr *attr;
	mnl_attr_for_each(attr, nlh, payload_size) {
		int type = mnl_attr_get_type(attr);
		/* skip unsupported attribute in user-space */
		if (0 > mnl_attr_type_valid(attr, XFRMA_MAX)) {
			continue;
		}
		switch (type) {
		case XFRMA_TMPL:
			attr_len = mnl_attr_get_len(attr);
			if (attr_len % sizeof(struct xfrm_user_tmpl)) {
				data->error_str = "XFRMA_TMPL validation failed";
				goto end;
			}
			tmpls = mnl_attr_get_payload(attr);
			if (0 != mnlxt_xfrm_policy_tmpl(policy, attr_len / sizeof(struct xfrm_user_tmpl), tmpls)) {
				data->error_str = "mnlxt_xfrm_policy_tmpl failed";
				goto end;
			}
			break;
		case XFRMA_POLICY_TYPE:
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, sizeof(struct xfrm_userpolicy_type))) {
				data->error_str = "XFRMA_POLICY_TYPE validation failed";
				goto end;
			}
			upt = mnl_attr_get_payload(attr);
			if (XFRM_POLICY_TYPE_MAIN != upt->type) {
				/* we need just main policies, do we ? */
				goto end;
			}
			break;
		case XFRMA_MARK:
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, sizeof(struct xfrm_mark))) {
				data->error_str = "XFRMA_MARK validation failed";
				goto end;
			}
			mark = mnl_attr_get_payload(attr);
			if (0 != mnlxt_xfrm_policy_set_mark(policy, mark->v, mark->m)) {
				data->error_str = "mnlxt_xfrm_policy_set_mark failed";
				goto end;
			}
			break;
		default:
			break;
		}
	}

	msg = mnlxt_xfrm_message_new(nlh->nlmsg_type, policy);
	if (!msg) {
		data->error_str = "mnlxt_xfrm_message_new failed";
		goto end;
	}

	mnlxt_data_add(data, msg);
	policy = NULL;
	msg = NULL;
	rc = MNL_CB_OK;
end:
	if (policy) {
		mnlxt_xfrm_policy_free(policy);
	}
	if (msg) {
		mnlxt_message_free(msg);
	}
	return rc;
}

int mnlxt_xfrm_policy_DATA(const struct nlmsghdr *nlh, void *data) {
	return mnlxt_xfrm_policy_data(nlh, (mnlxt_data_t *)data);
}
mnlxt_xfrm_policy_t *mnlxt_xfrm_policy_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator) {
	mnlxt_xfrm_policy_t *policy = NULL;
	if (iterator) {
		while ((*iterator = mnlxt_data_iterate(data, *iterator))) {
			if ((policy = mnlxt_xfrm_policy_get(*iterator))) {
				break;
			}
		}
	}
	return policy;
}

int mnlxt_xfrm_policy_dump(mnlxt_data_t *data) {
	return mnlxt_xfrm_data_dump(data, XFRM_MSG_GETPOLICY);
}

int mnlxt_xfrm_policy_request(mnlxt_xfrm_policy_t *xfrm_policy, uint16_t type) {
	int rc = -1;
	mnlxt_message_t *message = mnlxt_xfrm_policy_message(&xfrm_policy, type);
	if (NULL != message) {
		rc = mnlxt_xfrm_message_request(message);
		mnlxt_xfrm_policy_remove(message);
		mnlxt_message_free(message);
	}
	return rc;
}

mnlxt_message_t *mnlxt_xfrm_policy_message(mnlxt_xfrm_policy_t **policy, uint16_t type) {
	mnlxt_message_t *message = NULL;
	if (!policy || !*policy
			|| !(XFRM_MSG_NEWPOLICY == type || XFRM_MSG_UPDPOLICY == type || XFRM_MSG_DELPOLICY == type)) {
		errno = EINVAL;
	} else {
		message = mnlxt_xfrm_message_new(type, *policy);
		if (message) {
			*policy = NULL;
		}
	}
	return message;
}
