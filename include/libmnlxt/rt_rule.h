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

#include <linux/fib_rules.h>

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
} mnlxt_rt_rule_data_t;

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
	/** Rule actions, see linux/fib_rules.h
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
	mnlxt_inet_addr_t src;
	/** Route destination address */
	mnlxt_inet_addr_t dst;
	/** Name of the input interface */
	char *iif_name;
	/** Name of the output interface */
	char *oif_name;
} mnlxt_rt_rule_t;

/**
 * Creates a new rule information
 * @return pointer to new dynamically allocated rule information structure
 */
mnlxt_rt_rule_t *mnlxt_rt_rule_new();
/**
 * Makes a copy of a rule information structure
 * @param rule source address to copy from
 * @param filter data filter. In case of 0, the function is equal to @mnlxt_rt_rule_new().
 * Use macro MNLXT_FLAG to create filter from @mnlxt_rt_rule_data_t.
 * @return pointer to copy on success, else NULL
 */
mnlxt_rt_rule_t *mnlxt_rt_rule_clone(const mnlxt_rt_rule_t *rule, uint64_t filter);
/**
 * Frees memory allocated by a rule information structure
 * @param rule pointer to rule information structure to free
 */
void mnlxt_rt_rule_free(mnlxt_rt_rule_t *rule);
/**
 * Callback wrapper for mnlxt_rt_rule_free
 * @param rule rule information structure to free given by void pointer
 */
void mnlxt_rt_rule_FREE(void *rule);
/**
 * Sets rule family on rule information
 * @param rule pointer to rule information structure
 * @param family rule family
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_family(mnlxt_rt_rule_t *rule, uint8_t family);
/**
 * Gets rule family from rule information
 * @param rule pointer to rule information structure
 * @param family pointer to buffer to save rule family
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_family(const mnlxt_rt_rule_t *rule, uint8_t *family);
/**
 * Sets rule source prefix on rule information
 * @param rule pointer to rule information structure
 * @param prefixlen source prefix length
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_src_prefix(mnlxt_rt_rule_t *rule, uint8_t prefixlen);
/**
 * Gets rule source prefix from rule information
 * @param rule pointer to rule information structure
 * @param prefixlen pointer to buffer to save source prefix length
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_src_prefix(const mnlxt_rt_rule_t *rule, uint8_t *prefixlen);
/**
 * Sets rule destination prefix on route information
 * @param rule pointer to rule information structure
 * @param prefixlen destination prefix length
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_dst_prefix(mnlxt_rt_rule_t *rule, uint8_t prefixlen);
/**
 * Gets rule destination prefix from rule information
 * @param rule pointer to rule information structure
 * @param prefixlen pointer to buffer to save destination prefix length
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_dst_prefix(const mnlxt_rt_rule_t *rule, uint8_t *prefixlen);
/**
 * Sets rule source address on route information
 * @param rule pointer to rule information structure
 * @param family address family
 * @param buf pointer to source address buffer
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_src(mnlxt_rt_rule_t *rule, uint8_t family, const mnlxt_inet_addr_t *buf);
/**
 * Gets rule source address from rule information
 * @param rule pointer to rule information structure
 * @param family pointer to buffer to save address family
 * @param buf pointer to buffer to save pointer to source address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_src(const mnlxt_rt_rule_t *rule, uint8_t *family, const mnlxt_inet_addr_t **buf);
/**
 * Sets rule destination address on route information
 * @param rule pointer to rule information structure
 * @param family address family
 * @param buf pointer to destination address buffer
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_dst(mnlxt_rt_rule_t *rule, uint8_t family, const mnlxt_inet_addr_t *buf);
/**
 * Gets rule destination address from rule information
 * @param rule pointer to rule information structure
 * @param family pointer to buffer to save address family
 * @param buf pointer to buffer to save pointer to destination address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_dst(const mnlxt_rt_rule_t *rule, uint8_t *family, const mnlxt_inet_addr_t **buf);
/**
 * Sets rule table on rule information
 * @param rule pointer to rule information structure
 * @param table rule table number
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_table(mnlxt_rt_rule_t *rule, uint8_t table);
/**
 * Gets rule table from rule information
 * @param rule pointer to rule information structure
 * @param table pointer to buffer to save rule table
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_table(const mnlxt_rt_rule_t *rule, uint8_t *table);
/**
 * Sets rule action on rule information
 * @param rule pointer to rule information structure
 * @param action rule action
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_action(mnlxt_rt_rule_t *rule, uint8_t action);
/**
 * Gets rule table from rule information
 * @param rule pointer to rule information structure
 * @param action pointer to buffer to save rule action
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_action(const mnlxt_rt_rule_t *rule, uint8_t *action);
/**
 * Sets tos on rule information
 * @param rule pointer to rule information structure
 * @param tos tos
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_tos(mnlxt_rt_rule_t *rule, uint8_t tos);
/**
 * Gets tos from rule information
 * @param rule pointer to rule information structure
 * @param tos pointer to buffer to save tos
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_tos(const mnlxt_rt_rule_t *rule, uint8_t *tos);
/**
 * Sets rule priority on rule information
 * @param rule pointer to rule information structure
 * @param priority rule priority
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_priority(mnlxt_rt_rule_t *rule, uint32_t priority);
/**
 * Gets rule priority from rule information
 * @param rule pointer to rule information structure
 * @param priority pointer to buffer to save rule priority
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_priority(const mnlxt_rt_rule_t *rule, uint32_t *priority);
/**
 * Sets name of input interface on rule information
 * @param rule pointer to rule information structure
 * @param iif_name name of input interface
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_iif_name(mnlxt_rt_rule_t *rule, const char *iif_name);
/**
 * Gets name of input interface from rule information
 * @param rule pointer to rule information structure
 * @param iif_name pointer to buffer to save pointer to input interface name
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_iif_name(const mnlxt_rt_rule_t *rule, const char **iif_name);
/**
 * Sets name of output interface on rule information
 * @param rule pointer to rule information structure
 * @param oif_name name of output interface
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_oif_name(mnlxt_rt_rule_t *rule, const char *oif_name);
/**
 * Gets name of output interface from rule information
 * @param rule pointer to rule information structure
 * @param oif_name pointer to buffer to save pointer to output interface name
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_oif_name(const mnlxt_rt_rule_t *rule, const char **oif_name);
/**
 * Sets firewall mark on rule information
 * @param rule pointer to rule information structure
 * @param fwmark mark to set
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_fwmark(mnlxt_rt_rule_t *rule, uint32_t fwmark);
/**
 * Gets firewall mark from rule information
 * @param rule pointer to rule information structure
 * @param fwmark pointer to buffer to save mark
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_fwmark(const mnlxt_rt_rule_t *rule, uint32_t *fwmark);
/**
 * Sets firewall mask on rule information
 * @param rule pointer to rule information structure
 * @param fwmask mask to set
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_set_fwmask(mnlxt_rt_rule_t *rule, uint32_t fwmask);
/**
 * Gets firewall mask from rule information
 * @param rule pointer to rule information structure
 * @param fwmask pointer to buffer to save mask
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_rule_get_fwmask(const mnlxt_rt_rule_t *rule, uint32_t *fwmask);

/**
 * Checks if a rule information matches another one
 * @param rule pointer to rule information to check
 * @param match pointer to rule information to match
 * @return 0 for matching, else MNLXT_RT_RULE_* + 1 for property which does not match
 */
int mnlxt_rt_rule_match(const mnlxt_rt_rule_t *rule, const mnlxt_rt_rule_t *match);
/**
 * Compares two rules structures
 * @param rt_rule1 pointer to first rule information
 * @param rt_rule2 pointer to second rule information
 * @param filter data filter for selecting rule properties to compare. Use macro MNLXT_FLAG to create filter from
 * @mnlxt_rt_rule_data_t.
 * @return 0 for equal, else MNLXT_RT_RULE_* + 1 for property which does not match
 */
int mnlxt_rt_rule_compare(const mnlxt_rt_rule_t *rt_rule1, const mnlxt_rt_rule_t *rt_rule2, uint64_t filter);

/**
 * Initializes netlink message from rule information
 * @param nlh pointer to netlink message header to initialize
 * @param rule pointer to rule information structure
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_put(struct nlmsghdr *nlh, const mnlxt_rt_rule_t *rule);
/**
 * Callback wrapper at mnlxt_rt_rule_put
 * @param nlh nlh pointer to netlink message header to initialize
 * @param rule rule information structure given by void pointer
 * @param nlmsg_type message type (will be ignored)
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_PUT(struct nlmsghdr *nlh, const void *rule, uint16_t nlmsg_type);

/**
 * Parses netlink message into rule information and stores it into mnlxt data
 * @param nlh pointer to netlink message
 * @param data pointer to mnlxt data
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_rule_data(const struct nlmsghdr *nlh, mnlxt_data_t *data);
/**
 * Callback wrapper for mnlxt_rt_rule_data
 * @param nlh pointer to netlink message
 * @param data mnlxt data given by void pointer
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_rule_DATA(const struct nlmsghdr *nlh, void *data);

/**
 * Iterates over rule informations stored in mnlxt data
 * @param data pointer to mnlxt data
 * @param iterator data iterator; this pointer have to be initialized with NULL before iteration
 * @return pointer to the next rule information or NULL for the end of iteration
 */
mnlxt_rt_rule_t *mnlxt_rt_rule_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator);
/**
 * Gets rule information from mnlxt message.
 * In case, if you will handle the rule information independently from the mnlxt message use @mnlxt_rt_rule_remove
 * instead. Or clone it with @mnlxt_rt_rule_clone before calling @mnlxt_message_free.
 * @param message pointer to mnlxt message
 * @return pointer to rule information structure on success, else NULL
 */
mnlxt_rt_rule_t *mnlxt_rt_rule_get(const mnlxt_message_t *message);
/**
 * Removes rule information from mnlxt message.
 * Unlike @mnlxt_rt_rule_get it will detach rule information from the message.
 * @param message pointer to mnlxt message
 * @return pointer to rule information structure on success, else NULL
 */
mnlxt_rt_rule_t *mnlxt_rt_rule_remove(mnlxt_message_t *message);
/**
 * Creates a mnlxt message and stores the given rule information into it
 * @param rule double pointer to a rule information structure mnlxt_rt_rule_t
 * @param type message type (RTM_NEWRULE or RTM_DELRULE)
 * @return pointer to mnlxt message on success (pointer to the given mnlxt_rt rule will be reset) else NULL
 */
mnlxt_message_t *mnlxt_rt_rule_message(mnlxt_rt_rule_t **rule, uint16_t type);

/**
 * Sends a netlink request with the given rule information
 * @param rt_rule pointer to a rule information structure mnlxt_rt_rule_t
 * @param type request type (RTM_NEWRULE or RTM_DELRULE)
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_request(mnlxt_rt_rule_t *rt_rule, uint16_t type);
/**
 * Gets information of all network rules configured on system
 * @param data pointer to mnlxt data to store information into
 * @param family rule family get the information for
 * @return 0 on success, else -1
 */
int mnlxt_rt_rule_dump(mnlxt_data_t *data, unsigned char family);

#endif /* LIBMNLXT_RT_RULE_H_ */
