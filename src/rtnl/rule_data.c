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

#include <libmnlxt/rt.h>

int mnlxt_rt_rule_match(const mnlxt_rt_rule_t *rule, const mnlxt_rt_rule_t *match) {
	int i = -1;
	if (rule && match) {
		int flag = 0x1;
		size_t addr_size = (AF_INET == rule->family ? sizeof(rule->src.in) : sizeof(rule->src));
		for (i = 0; i < MNLXT_RT_RULE_MAX; ++i) {
			if (match->prop_flags & flag) {
				if (rule->prop_flags & flag) {
					switch (i) {
					case MNLXT_RT_RULE_FAMILY:
						if (rule->family != match->family) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_TABLE:
						if (rule->table != match->table) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_TOS:
						if (rule->tos != match->tos) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_ACTION:
						if (rule->action != match->action) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_PRIORITY:
						if (rule->priority != match->priority) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_OIFNAME:
						if (strcmp(rule->oif_name, match->oif_name)) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_IIFNAME:
						if (strcmp(rule->iif_name, match->iif_name)) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_SRC_PREFIX:
						if (rule->src_prefix != match->src_prefix) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_DST_PREFIX:
						if (rule->dst_prefix != match->dst_prefix) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_SRC:
						if (0 != memcmp(&rule->src, &match->src, addr_size)) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_DST:
						if (0 != memcmp(&rule->dst, &match->dst, addr_size)) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_FWMARK:
						if (rule->fwmark != match->fwmark) {
							goto fail;
						}
						break;
					case MNLXT_RT_RULE_FWMASK:
						if (rule->fwmask != match->fwmask) {
							goto fail;
						}
						break;
					}
				} else {
					goto fail;
				}
			}
			flag <<= 1;
		}
		return 0;
	fail:
		++i;
	} else {
		errno = EINVAL;
	}
	return i;
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
		int flag = 0x1;
		size_t addr_size = (AF_INET == rule->family ? sizeof(rule->src.in) : sizeof(rule->src));
		int i = 0;
		for (; i < MNLXT_RT_RULE_MAX; ++i) {
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
			flag <<= 1;
		}
		rc = 0;
	} else {
		errno = EINVAL;
	}
	return rc;
}

int mnlxt_rt_rule_PUT(struct nlmsghdr *nlh, const void *rule, uint16_t nlmsg_type) {
	(void)nlmsg_type;
	return mnlxt_rt_rule_put(nlh, (mnlxt_rt_rule_t *)rule);
}

int mnlxt_rt_rule_data(const struct nlmsghdr *nlh, mnlxt_data_t *data) {
	int rc = MNL_CB_ERROR;
	mnlxt_rt_rule_t *rule = NULL;
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
	if (!rule) {
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

	msg = mnlxt_rt_message_new(nlh->nlmsg_type, rule);
	if (!msg) {
		data->error_str = "mnlxt_rt_message_new failed";
		goto end;
	}

	mnlxt_data_add(data, msg);
	rule = NULL;
	msg = NULL;
	rc = MNL_CB_OK;
end:
	if (rule) {
		mnlxt_rt_rule_free(rule);
	}
	if (msg) {
		mnlxt_message_free(msg);
	}
	return rc;
}

int mnlxt_rt_rule_DATA(const struct nlmsghdr *nlh, void *data) {
	return mnlxt_rt_rule_data(nlh, (mnlxt_data_t *)data);
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

mnlxt_message_t *mnlxt_rt_rule_message(mnlxt_rt_rule_t **rule, uint16_t type) {
	mnlxt_message_t *message = NULL;
	if (!rule || !*rule || !(RTM_NEWRULE == type || RTM_DELRULE == type)) {
		errno = EINVAL;
	} else {
		message = mnlxt_rt_message_new(type, *rule);
		if (message) {
			*rule = NULL;
		}
	}
	return message;
}
