/*
 * rule_data.c		Libmnlxt Routing Rules
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <errno.h>
#include <string.h>

#include "libmnlxt/rt.h"

static int mnlxt_rt_rule_cmp(const mnlxt_rt_rule_t *rt_rule1, const mnlxt_rt_rule_t *rt_rule2,
														 mnlxt_rt_rule_data_t data) {
	int rc = data + 1;
	size_t addr_size;
	switch (data) {
	case MNLXT_RT_RULE_FAMILY:
		if (rt_rule1->family != rt_rule2->family) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_TABLE:
		if (rt_rule1->table != rt_rule2->table) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_TOS:
		if (rt_rule1->tos != rt_rule2->tos) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_ACTION:
		if (rt_rule1->action != rt_rule2->action) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_PRIORITY:
		if (rt_rule1->priority != rt_rule2->priority) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_OIFNAME:
		if (strcmp(rt_rule1->oif_name, rt_rule2->oif_name)) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_IIFNAME:
		if (strcmp(rt_rule1->iif_name, rt_rule2->iif_name)) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_SRC_PREFIX:
		if (rt_rule1->src_prefix != rt_rule2->src_prefix) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_DST_PREFIX:
		if (rt_rule1->dst_prefix != rt_rule2->dst_prefix) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_SRC:
		addr_size = (AF_INET == rt_rule1->family ? sizeof(rt_rule1->src.in) : sizeof(rt_rule1->src));
		if (0 != memcmp(&rt_rule1->src, &rt_rule2->src, addr_size)) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_DST:
		addr_size = (AF_INET == rt_rule1->family ? sizeof(rt_rule1->dst.in) : sizeof(rt_rule1->dst));
		if (0 != memcmp(&rt_rule1->dst, &rt_rule2->dst, addr_size)) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_FWMARK:
		if (rt_rule1->fwmark != rt_rule2->fwmark) {
			goto failed;
		}
		break;
	case MNLXT_RT_RULE_FWMASK:
		if (rt_rule1->fwmask != rt_rule2->fwmask) {
			goto failed;
		}
		break;
	}
	rc = 0;
failed:
	return rc;
}

int mnlxt_rt_rule_match(const mnlxt_rt_rule_t *rt_rule, const mnlxt_rt_rule_t *match) {
	int rc = -1;
	if (NULL == match) {
		errno = EINVAL;
	} else {
		rc = mnlxt_rt_rule_compare(rt_rule, match, match->prop_flags);
	}
	return rc;
}

int mnlxt_rt_rule_compare(const mnlxt_rt_rule_t *rt_rule1, const mnlxt_rt_rule_t *rt_rule2, uint64_t filter) {
	int rc = -1, i;
	if (NULL == rt_rule1 || NULL == rt_rule2) {
		errno = EINVAL;
	} else {
		uint64_t flag = 1;
		for (i = 0; i < MNLXT_RT_RULE_MAX; ++i, flag <<= 1) {
			if (0 == (flag & filter)) {
				continue;
			}
			if (0 == (rt_rule1->prop_flags & flag)) {
				if (0 == (rt_rule2->prop_flags & flag)) {
					/* both not set */
					continue;
				}
				goto failed;
			} else if (0 == (rt_rule2->prop_flags & flag)) {
				goto failed;
			} else if (0 != mnlxt_rt_rule_cmp(rt_rule1, rt_rule2, i)) {
				goto failed;
			}
		}
		return 0;
	failed:
		rc = ++i;
	}
	return rc;
}

mnlxt_rt_rule_t *mnlxt_rt_rule_get(const mnlxt_message_t *message) {
	mnlxt_rt_rule_t *rule = NULL;
	if (message && message->payload
			&& (RTM_NEWRULE == message->nlmsg_type || RTM_GETRULE == message->nlmsg_type
					|| RTM_DELRULE == message->nlmsg_type)) {
		rule = (mnlxt_rt_rule_t *)message->payload;
	}
	return rule;
}

mnlxt_rt_rule_t *mnlxt_rt_rule_remove(mnlxt_message_t *message) {
	mnlxt_rt_rule_t *rule = mnlxt_rt_rule_get(message);
	if (rule) {
		message->payload = NULL;
	}
	return rule;
}

int mnlxt_rt_rule_put(struct nlmsghdr *nlh, const mnlxt_rt_rule_t *rule) {
	int rc = -1;
	if (rule && nlh) {
		struct fib_rule_hdr *rule_hdr = mnl_nlmsg_put_extra_header(nlh, sizeof(struct fib_rule_hdr));
		uint16_t flag = 1;
		size_t addr_size = (AF_INET == rule->family ? sizeof(rule->src.in) : sizeof(rule->src));
		int i = 0;
		for (; i < MNLXT_RT_RULE_MAX; ++i, flag <<= 1) {
			if (rule->prop_flags & flag) {
				switch (i) {
				case MNLXT_RT_RULE_FAMILY:
					rule_hdr->family = rule->family;
					break;
				case MNLXT_RT_RULE_TABLE:
					rule_hdr->table = rule->table;
					break;
				case MNLXT_RT_RULE_ACTION:
					rule_hdr->action = rule->action;
					break;
				case MNLXT_RT_RULE_TOS:
					rule_hdr->tos = rule->tos;
					break;
				case MNLXT_RT_RULE_PRIORITY:
					mnl_attr_put_u32(nlh, FRA_PRIORITY, rule->priority);
					break;
				case MNLXT_RT_RULE_OIFNAME:
					mnl_attr_put_str(nlh, FRA_OIFNAME, rule->oif_name);
					break;
				case MNLXT_RT_RULE_IIFNAME:
					mnl_attr_put_str(nlh, FRA_IIFNAME, rule->iif_name);
					break;
				case MNLXT_RT_RULE_SRC_PREFIX:
					rule_hdr->src_len = rule->src_prefix;
					break;
				case MNLXT_RT_RULE_DST_PREFIX:
					rule_hdr->dst_len = rule->dst_prefix;
					break;
				case MNLXT_RT_RULE_SRC:
					mnl_attr_put(nlh, RTA_SRC, addr_size, &rule->src);
					break;
				case MNLXT_RT_RULE_DST:
					mnl_attr_put(nlh, RTA_DST, addr_size, &rule->dst);
					break;
				case MNLXT_RT_RULE_FWMARK:
					mnl_attr_put_u32(nlh, FRA_FWMARK, rule->fwmark);
					break;
				case MNLXT_RT_RULE_FWMASK:
					mnl_attr_put_u32(nlh, FRA_FWMASK, rule->fwmask);
					break;
				}
			}
		}
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_rule_data(const struct nlmsghdr *nlh, mnlxt_data_t *data) {
	int rc = MNL_CB_ERROR;
	mnlxt_rt_rule_t *rule = NULL;
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

	if (RTM_GETRULE != nlh->nlmsg_type && RTM_NEWRULE != nlh->nlmsg_type && RTM_DELRULE != nlh->nlmsg_type) {
		errno = EBADMSG;
		data->error_str = "unsupported message type";
		goto end;
	}

	struct fib_rule_hdr *rule_hdr = mnl_nlmsg_get_payload(nlh);

	/* IPv6 and IPv4 support only */
	size_t len = 0;
	if (AF_INET == rule_hdr->family) {
		len = sizeof(struct in_addr);
	} else if (AF_INET6 == rule_hdr->family) {
		len = sizeof(struct in6_addr);
	} else {
		rc = MNL_CB_OK;
		goto end;
	}

	rule = mnlxt_rt_rule_new();
	if (NULL == rule) {
		data->error_str = "mnlxt_rt_rule_new failed";
		goto end;
	}

	mnlxt_rt_rule_set_table(rule, rule_hdr->table);
	mnlxt_rt_rule_set_action(rule, rule_hdr->action);
	mnlxt_rt_rule_set_tos(rule, rule_hdr->tos);
	mnlxt_rt_rule_set_family(rule, rule_hdr->family);

	struct nlattr *attr;
	mnl_attr_for_each(attr, nlh, sizeof(*rule_hdr)) {
		int type = mnl_attr_get_type(attr);
		/* skip unsupported attribute in user-space */
		if (0 > mnl_attr_type_valid(attr, FRA_MAX)) {
			continue;
		}
		switch (type) {
		case FRA_UNSPEC:
			break;
		case FRA_DST:
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, len)) {
				data->error_str = "RTA_DST validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_rule_set_dst(rule, rule_hdr->family, mnl_attr_get_payload(attr))) {
				data->error_str = "mnlxt_rt_rule_set_dst failed";
				goto end;
			}
			mnlxt_rt_rule_set_dst_prefix(rule, rule_hdr->dst_len);
			break;
		case FRA_SRC:
			if (0 > mnl_attr_validate2(attr, MNL_TYPE_BINARY, len)) {
				data->error_str = "RTA_SRC validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_rule_set_src(rule, rule_hdr->family, mnl_attr_get_payload(attr))) {
				data->error_str = "mnlxt_rt_rule_set_src failed";
				goto end;
			}
			mnlxt_rt_rule_set_src_prefix(rule, rule_hdr->src_len);
			break;
		case FRA_FWMARK:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "FRA_FWMARK validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_rule_set_fwmark(rule, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_rule_set_femark failed";
				goto end;
			}
			break;
		case FRA_FWMASK:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "FRA_FWMASK validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_rule_set_fwmask(rule, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_rule_set_femask failed";
				goto end;
			}
			break;
		case FRA_PRIORITY:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_U32)) {
				data->error_str = "RTA_PRIORITY validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_rule_set_priority(rule, mnl_attr_get_u32(attr))) {
				data->error_str = "mnlxt_rt_rule_set_priority failed";
				goto end;
			}
			break;
		case FRA_OIFNAME:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_STRING)) {
				data->error_str = "FRA_OIFNAME validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_rule_set_oif_name(rule, mnl_attr_get_str(attr))) {
				data->error_str = "mnlxt_rt_rule_set_oif_name failed";
				goto end;
			}
			break;
		case FRA_IIFNAME:
			if (0 > mnl_attr_validate(attr, MNL_TYPE_STRING)) {
				data->error_str = "FRA_IIFNAME validation failed";
				goto end;
			}
			if (-1 == mnlxt_rt_rule_set_iif_name(rule, mnl_attr_get_str(attr))) {
				data->error_str = "mnlxt_rt_rule_set_iif_name failed";
				goto end;
			}
			break;
		default:
			break;
		}
	}

	msg = mnlxt_rt_message_new(nlh->nlmsg_type, 0, rule);
	if (NULL == msg) {
		data->error_str = "mnlxt_rt_message_new failed";
		goto end;
	}

	mnlxt_data_add(data, msg);
	rule = NULL;
	msg = NULL;
	rc = MNL_CB_OK;
end:
	mnlxt_rt_rule_free(rule);
	mnlxt_message_free(msg);

	return rc;
}

mnlxt_rt_rule_t *mnlxt_rt_rule_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator) {
	mnlxt_rt_rule_t *rule = NULL;
	if (iterator) {
		while ((*iterator = mnlxt_data_iterate(data, *iterator))) {
			if ((rule = mnlxt_rt_rule_get(*iterator))) {
				break;
			}
		}
	}
	return rule;
}

int mnlxt_rt_rule_dump(mnlxt_data_t *data, unsigned char family) {
	int rc = -1;
	if (AF_INET != family && AF_INET6 != family && AF_UNSPEC != family) {
		errno = EAFNOSUPPORT;
	} else {
		rc = mnlxt_rt_data_dump(data, RTM_GETRULE, family);
	}
	return rc;
}

int mnlxt_rt_rule_request(mnlxt_rt_rule_t *rt_rule, uint16_t type, uint16_t flags) {
	int rc = -1;
	mnlxt_message_t *message = mnlxt_rt_rule_message(&rt_rule, type, flags);
	if (NULL != message) {
		rc = mnlxt_rt_message_request(message);
		mnlxt_rt_rule_remove(message);
		mnlxt_message_free(message);
	}
	return rc;
}

mnlxt_message_t *mnlxt_rt_rule_message(mnlxt_rt_rule_t **rule, uint16_t type, uint16_t flags) {
	mnlxt_message_t *message = NULL;
	if (NULL == rule || NULL == *rule || !(RTM_NEWRULE == type || RTM_DELRULE == type)) {
		errno = EINVAL;
	} else if (NULL != (message = mnlxt_rt_message_new(type, flags, *rule))) {
		*rule = NULL;
	}
	return message;
}
