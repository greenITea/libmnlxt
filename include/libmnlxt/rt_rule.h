/*
 * libmnlxt/rt_rule.h		Libmnlxt Routing Rules
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_RT_RULE_H_
#define LIBMNLXT_RT_RULE_H_

#include <sys/socket.h>

#include <libmnlxt/rt_addr.h>

typedef enum {
	MNLXT_RT_RULE_FAMILY = 0,
	MNLXT_RT_RULE_SRC_PREFIX,
	MNLXT_RT_RULE_DST_PREFIX,
	MNLXT_RT_RULE_SRC,
	MNLXT_RT_RULE_DST,
	MNLXT_RT_RULE_TABLE,
	MNLXT_RT_RULE_IIFNAME,
	MNLXT_RT_RULE_OIFNAME,
	MNLXT_RT_RULE_TOS,
	MNLXT_RT_RULE_ACTION,
	MNLXT_RT_RULE_PRIORITY,
	MNLXT_RT_RULE_FWMARK,
	MNLXT_RT_RULE_FWMASK
#define MNLXT_RT_RULE_MAX MNLXT_RT_RULE_FWMASK + 1
} nl_rule_data_t;

typedef struct {
	/** Properties flags */
	uint16_t prop_flags;
	/** Address family of the rule */
	uint8_t family;
	/** Routing table ID, @see rtnetlink(7);
	 * RT_TABLE_UNSPEC		an unspecified routing table
	 * RT_TABLE_DEFAULT		the default table
	 * RT_TABLE_MAIN		the main table
	 * RT_TABLE_LOCAL		the local table
	 * */
	uint8_t table;
	uint8_t tos;
	/** Rule actions
	 * FR_ACT_UNSPEC
	 * FR_ACT_TO_TBL			pass to fixed table
	 * FR_ACT_GOTO				jump to another rule
	 * FR_ACT_NOP				no operation
	 * FR_ACT_RES3
	 * FR_ACT_RES4
	 * FR_ACT_BLACKHOLE			drop without notification
	 * FR_ACT_UNREACHABLE		drop with ENETUNREACH
	 * FR_ACT_PROHIBIT			drop with EACCES
	 * */
	uint8_t action;
	uint8_t src_prefix;
	uint8_t dst_prefix;
	/** Priority of rule */
	uint32_t priority;
	uint32_t fwmark;
	uint32_t fwmask;
	/** Route source address */
	inet_addr_t src;
	/** Route destination address */
	inet_addr_t dst;
	/** Name of the input interface */
	char *iif_name;
	/** Name of the output interface */
	char *oif_name;
} mnlxt_rt_rule_t;

/**
 * Creates a new rule information
 * @return pointer at new dynamically allocated rule information structure
 */
mnlxt_rt_rule_t *mnlxt_rt_rule_new();
/**
 * Frees memory allocated by a rule information structure
 * @param rule pointer at rule information structure to free
 */
void mnlxt_rt_rule_free(mnlxt_rt_rule_t *rule);
/**
 * Callback wrapper for mnlxt_rt_rule_free
 * @param rule rule information structure to free given by void pointer
 */
void mnlxt_rt_rule_FREE(void *rule);
/**
 * Sets rule family on rule information
 * @param rule pointer at rule information structure
 * @param family rule family
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_family(mnlxt_rt_rule_t *rule, uint8_t family);
/**
 * Gets rule family from rule information
 * @param rule pointer at rule information structure
 * @param family pointer at buffer to save rule family
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_family(mnlxt_rt_rule_t *rule, uint8_t *family);
/**
 * Sets rule source prefix on rule information
 * @param rule pointer at rule information structure
 * @param prefixlen source prefix length
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_src_prefix(mnlxt_rt_rule_t *rule, uint8_t prefixlen);
/**
 * Gets rule source prefix from rule information
 * @param rule pointer at rule information structure
 * @param prefixlen pointer at buffer to save source prefix length
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_src_prefix(mnlxt_rt_rule_t *rule, uint8_t *prefixlen);
/**
 * Sets rule destination prefix on route information
 * @param rule pointer at rule information structure
 * @param prefixlen destination prefix length
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_dst_prefix(mnlxt_rt_rule_t *rule, uint8_t prefixlen);
/**
 * Gets rule destination prefix from rule information
 * @param rule pointer at rule information structure
 * @param prefixlen pointer at buffer to save destination prefix length
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_dst_prefix(mnlxt_rt_rule_t *rule, uint8_t *prefixlen);
/**
 * Sets rule source address on route information
 * @param rule pointer at rule information structure
 * @param family address family
 * @param buf pointer at source address buffer
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_src(mnlxt_rt_rule_t *rule, uint8_t family, inet_addr_t *buf);
/**
 * Gets rule source address from rule information
 * @param rule pointer at rule information structure
 * @param family pointer at buffer to save address family
 * @param buf pointer at buffer to save pointer at source address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_src(mnlxt_rt_rule_t *rule, uint8_t *family, inet_addr_t **buf);
/**
 * Sets rule destination address on route information
 * @param rule pointer at rule information structure
 * @param family address family
 * @param buf pointer at destination address buffer
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_dst(mnlxt_rt_rule_t *rule, uint8_t family, inet_addr_t *buf);
/**
 * Gets rule destination address from rule information
 * @param rule pointer at rule information structure
 * @param family pointer at buffer to save address family
 * @param buf pointer at buffer to save pointer at destination address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_dst(mnlxt_rt_rule_t *rule, uint8_t *family, inet_addr_t **buf);
/**
 * Sets rule table on rule information
 * @param rule pointer at rule information structure
 * @param table rule table number
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_table(mnlxt_rt_rule_t *rule, uint8_t table);
/**
 * Gets rule table from rule information
 * @param rule pointer at rule information structure
 * @param table pointer at buffer to save rule table
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_table(mnlxt_rt_rule_t *rule, uint8_t *table);
/**
 * Sets rule action on rule information
 * @param rule pointer at rule information structure
 * @param action rule action
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_action(mnlxt_rt_rule_t *rule, uint8_t action);
/**
 * Gets rule table from rule information
 * @param rule pointer at rule information structure
 * @param action pointer at buffer to save rule action
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_action(mnlxt_rt_rule_t *rule, uint8_t *action);
/**
 * Sets tos on rule information
 * @param rule pointer at rule information structure
 * @param tos tos
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_tos(mnlxt_rt_rule_t *rule, uint8_t tos);
/**
 * Gets tos from rule information
 * @param rule pointer at rule information structure
 * @param tos pointer at buffer to save tos
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_tos(mnlxt_rt_rule_t *rule, uint8_t *tos);
/**
 * Sets rule priority on rule information
 * @param rule pointer at rule information structure
 * @param priority rule priority
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_priority(mnlxt_rt_rule_t *rule, uint32_t priority);
/**
 * Gets rule priority from rule information
 * @param rule pointer at rule information structure
 * @param priority pointer at buffer to save rule priority
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_priority(mnlxt_rt_rule_t *rule, uint32_t *priority);
/**
 * Sets name of input interface on rule information
 * @param rule pointer at rule information structure
 * @param iif_name name of input interface
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_iif_name(mnlxt_rt_rule_t *rule, const char *iif_name);
/**
 * Gets name of input interface from rule information
 * @param rule pointer at rule information structure
 * @param iif_name pointer at buffer to save pointer at input interface name
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_iif_name(mnlxt_rt_rule_t *rule, const char **iif_name);
/**
 * Sets name of output interface on rule information
 * @param rule pointer at rule information structure
 * @param oif_name name of output interface
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_oif_name(mnlxt_rt_rule_t *rule, const char *oif_name);
/**
 * Gets name of output interface from rule information
 * @param rule pointer at rule information structure
 * @param oif_name pointer at buffer to save pointer at output interface name
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_oif_name(mnlxt_rt_rule_t *rule, const char **oif_name);
/**
 * Sets firewall mark on rule information
 * @param rule pointer at rule information structure
 * @param fwmark mark to set
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_fwmark(mnlxt_rt_rule_t *rule, uint32_t fwmark);
/**
 * Gets firewall mark from rule information
 * @param rule pointer at rule information structure
 * @param fwmark pointer at buffer to save mark
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_fwmark(mnlxt_rt_rule_t *rule, uint32_t *fwmark);
/**
 * Sets firewall mask on rule information
 * @param rule pointer at rule information structure
 * @param fwmask mask to set
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_fwmask(mnlxt_rt_rule_t *rule, uint32_t fwmask);
/**
 * Gets firewall mask from rule information
 * @param rule pointer at rule information structure
 * @param fwmask pointer at buffer to save mask
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_fwmask(mnlxt_rt_rule_t *rule, uint32_t *fwmask);

/**
 * Checks if a rule information matches another one
 * @param rule pointer at rule information to check
 * @param match pointer at rule information to match
 * @return 0 for matching, else MNLXT_RT_RULE_* + 1 for property which does not match
 */
int mnlxt_rt_rule_match(mnlxt_rt_rule_t *rule, mnlxt_rt_rule_t *match);

/**
 * Initializes netlink message from rule information
 * @param nlh pointer at netlink message header to initialize
 * @param rule pointer at rule information structure
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_put(struct nlmsghdr *nlh, mnlxt_rt_rule_t *rule);
/**
 * Callback wrapper at mnlxt_rt_rule_put
 * @param nlh nlh pointer at netlink message header to initialize
 * @param rule rule information structure given by void pointer
 * @param nlmsg_type message type (will be ignored)
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_PUT(struct nlmsghdr *nlh, void *rule, uint16_t nlmsg_type);

/**
 * Parses netlink message into rule information and stores it into mnlxt data
 * @param nlh pointer at netlink message
 * @param data pointer at mnlxt data
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_rule_data(const struct nlmsghdr *nlh, mnlxt_data_t *data);
/**
 * Callback wrapper for mnlxt_rt_rule_data
 * @param nlh pointer at netlink message
 * @param data mnlxt data given by void pointer
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_rule_DATA(const struct nlmsghdr *nlh, void *data);

/**
 * Iterates over rule informations stored in mnlxt data
 * @param data pointer at mnlxt data
 * @param iterator data iterator; this pointer have to be initialized with NULL before iteration
 * @return pointer at the next rule information or NULL for the end of iteration
 */
mnlxt_rt_rule_t *mnlxt_rt_rule_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator);
/**
 * Gets rule information from mnlxt message
 * @param message pointer at mnlxt message
 * @return pointer at rule information structure on success, else NULL
 */
mnlxt_rt_rule_t *mnlxt_rt_rule_get(mnlxt_message_t *message);
/**
 * Creates a mnlxt message and stores the given rule information into it
 * @param rule double pointer at a rule information structure mnlxt_rt_rule_t
 * @param type message type (RTM_NEWRULE or RTM_DELRULE)
 * @return pointer at mnlxt message on success (pointer at the given mnlxt_rt rule will be reset) else NULL
 */
mnlxt_message_t *mnlxt_rt_rule_message(mnlxt_rt_rule_t **rule, uint16_t type);

/**
 * Gets information of all network rules configured on system
 * @param data pointer at mnlxt data to store information into
 * @param family rule family get the information for
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_dump(mnlxt_data_t *data, unsigned char family);

#endif /* LIBMNLXT_RT_RULE_H_ */
