/*
 * libmnlxt/rt_addr.h		Libmnlxt Routing Address
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_RT_ADDR_H_
#define LIBMNLXT_RT_ADDR_H_

#include <netinet/in.h>

#include <linux/rtnetlink.h>

#include <libmnlxt/data.h>
#include <libmnlxt/rt_link.h>

#define MNLXT_RT_ADDR_LIFE_TIME_INFINITY 0xFFFFFFFFU

typedef enum {
	MNLXT_RT_ADDR_FAMILY = 0,
	MNLXT_RT_ADDR_PREFIXLEN,
	MNLXT_RT_ADDR_FLAGS,
	MNLXT_RT_ADDR_SCOPE,
	MNLXT_RT_ADDR_IFINDEX,
	MNLXT_RT_ADDR_ADDR,
	MNLXT_RT_ADDR_LOCAL,
	MNLXT_RT_ADDR_LABEL,
	MNLXT_RT_ADDR_CACHEINFO
#define MNLXT_RT_ADDR_MAX MNLXT_RT_ADDR_CACHEINFO + 1
} mnlxt_rt_addr_data_t;

typedef union {
	struct in6_addr in6;
	struct in_addr in;
} mnlxt_inet_addr_t;

typedef struct {
	/** Properties flags */
	uint16_t prop_flags;
	/** AF_INET6 or AF_INET */
	uint8_t family;
	/** IPv4/IPv6 prefix length */
	uint8_t prefixlen;
	/** Interface address flags, see linux/if_addr.h
	 * IFA_F_TEMPORARY
	 * IFA_F_DEPRECATED
	 * IFA_F_TENTATIVE
	 * IFA_F_PERMANENT
	 */
	uint32_t flags;
	/**
	 * 1 - 199 are user defined
	 * RT_SCOPE_UNIVERSE
	 * RT_SCOPE_SITE
	 * RT_SCOPE_LINK
	 * RT_SCOPE_HOST
	 * RT_SCOPE_NOWHERE
	 */
	uint8_t scope;
	/** Interface index */
	uint32_t if_index;
	/** IPv6 address, or IPv6 mapped IPv4 address */
	mnlxt_inet_addr_t addr;
	/** Local IP address, equal to addr except ppp-like interfaces */
	/* does it exists for IPv6? */
	mnlxt_inet_addr_t addr_local;
	/** Address label, IPv4 only */
	char *label;
	/** Cache Info like valid life time */
	struct ifa_cacheinfo cacheinfo;
} mnlxt_rt_addr_t;

/**
 * Creates new address information structure
 * @return pointer to dynamically allocated mnlxt_rt_addr_t on success, else NULL
 */
mnlxt_rt_addr_t *mnlxt_rt_addr_new();
/**
 * Makes a copy of an address information structure
 * @param addr source address to copy from
 * @param filter data filter. In case of 0, the function is equal to @mnlxt_rt_addr_new().
 * Use macro MNLXT_FLAG to create filter from @mnlxt_rt_addr_data_t.
 * @return pointer to copy on success, else NULL
 */
mnlxt_rt_addr_t *mnlxt_rt_addr_clone(const mnlxt_rt_addr_t *addr, uint64_t filter);
/**
 * Frees memory allocated by netlink address information structure
 * @param addr pointer to netlink address information structure to free
 */
void mnlxt_rt_addr_free(mnlxt_rt_addr_t *addr);
/**
 * Frees memory allocated by an address information structure (callback version)
 * @param addr address information structure to free given by a void pointer
 */
static inline void mnlxt_rt_addr_FREE(void *addr) {
	mnlxt_rt_addr_free((mnlxt_rt_addr_t *)addr);
}
/**
 * Sets address family on address information
 * @param addr pointer to address information structure
 * @param family address family to set
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_family(mnlxt_rt_addr_t *addr, uint8_t family);
/**
 * Gets address family from address information
 * @param addr pointer to address information structure
 * @param family pointer to buffer to copy address family into
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_family(const mnlxt_rt_addr_t *addr, uint8_t *family);
/**
 * Sets prefix length on address information
 * @param addr pointer to address information structure
 * @param prefixlen prefix length
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_prefixlen(mnlxt_rt_addr_t *addr, uint8_t prefixlen);
/**
 * Gets prefix length from address information
 * @param addr pointer to address information structure
 * @param prefixlen pointer to buffer to copy prefix length into
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_prefixlen(const mnlxt_rt_addr_t *addr, uint8_t *prefixlen);
/**
 * Set flags on address information
 * @param addr pointer to address information structure
 * @param flags flags to set
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_flags(mnlxt_rt_addr_t *addr, uint32_t flags);
/**
 * Get flags from address information
 * @param addr pointer to address information structure
 * @param flags pointer to buffer to copy flags into
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_flags(const mnlxt_rt_addr_t *addr, uint32_t *flags);
/**
 * Sets scope on address information
 * @param addr pointer to address information structure
 * @param scope address scope
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_scope(mnlxt_rt_addr_t *addr, uint8_t scope);
/**
 * Gets scope from address information
 * @param addr pointer to address information structure
 * @param scope pointer to buffer to copy address scope into
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_scope(const mnlxt_rt_addr_t *addr, uint8_t *scope);
/**
 * Sets interface index on address information
 * @param addr pointer to address information structure
 * @param if_index interface index
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_ifindex(mnlxt_rt_addr_t *addr, uint32_t if_index);
/**
 * Gets interface index from address information
 * @param addr pointer to address information structure
 * @param if_index pointer to buffer to copy interface index into
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_ifindex(const mnlxt_rt_addr_t *addr, uint32_t *if_index);
/**
 * Sets IP address on address information
 * @param addr pointer to address information structure
 * @param family address family
 * @param buf pointer to address buffer
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_addr(mnlxt_rt_addr_t *addr, uint8_t family, const mnlxt_inet_addr_t *buf);
/**
 * Gets IP address from address information
 * @param addr pointer to address information structure
 * @param family pointer to buffer to store address family
 * @param buf pointer to store pointer to IP address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_addr(const mnlxt_rt_addr_t *addr, uint8_t *family, const mnlxt_inet_addr_t **buf);
/**
 * Sets local IP address on address information
 * @param addr pointer to address information structure
 * @param family address family
 * @param buf pointer to IP address buffer
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_local(mnlxt_rt_addr_t *addr, uint8_t family, const mnlxt_inet_addr_t *buf);
/**
 * Gets local IP address from address information
 * @param addr pointer to address information structure
 * @param family pointer to buffer to store address family
 * @param buf pointer to store pointer to address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_local(const mnlxt_rt_addr_t *addr, uint8_t *family, const mnlxt_inet_addr_t **buf);
/**
 * Sets label on address information by copying the string. The labels are valid with IPv4 only.
 * The maximal length of the label is limited to 15 symbols.
 * @param addr pointer to address information structure
 * @param label label to set
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_label(mnlxt_rt_addr_t *addr, const mnlxt_if_name_t label);
/**
 * Gets label from address information
 * @param addr pointer to address information structure
 * @param label pointer to buffer to store label
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_label(const mnlxt_rt_addr_t *addr, mnlxt_if_name_t label);

/**
 * Get valid lifetime from address information cache
 * @param addr pointer to address information structure
 * @param valid_lft pointer to buffer to store time
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_get_valid_lifetime(const mnlxt_rt_addr_t *addr, uint32_t *valid_lft);

/**
 * Set valid lifetime on address information cache
 * @param addr pointer to address information structure
 * @param valid_lft time
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_set_valid_lifetime(mnlxt_rt_addr_t *addr, uint32_t valid_lft);

/**
 * Get preferred lifetime from address information cache
 * @param addr pointer to address information structure
 * @param preferred_lft pointer to buffer to store time
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_get_preferred_lifetime(const mnlxt_rt_addr_t *addr, uint32_t *preferred_lft);

/**
 * Set preferred lifetime on address information cache
 * @param addr pointer to address information structure
 * @param preferred_lft time
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_set_preferred_lifetime(mnlxt_rt_addr_t *addr, uint32_t preferred_lft);

/**
 * Get create time from address information cache
 * @param addr pointer to address information structure
 * @param create_time pointer to buffer to store time
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_get_create_time(const mnlxt_rt_addr_t *addr, uint32_t *create_time);

/**
 * Get update time from address information cache
 * @param addr pointer to address information structure
 * @param update_time pointer to buffer to store time
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_get_last_update_time(const mnlxt_rt_addr_t *addr, uint32_t *update_time);

/**
 * Checks if an address information matches another one
 * @param addr pointer to address to check
 * @param match pointer to address to match
 * @return 0 for matching, else MNLXT_RT_ADDR_* + 1 for property which does not match
 */
int mnlxt_rt_addr_match(const mnlxt_rt_addr_t *addr, const mnlxt_rt_addr_t *match);
/**
 * Compares two address information structures
 * @param rt_addr1 pointer to first address
 * @param rt_addr2 pointer to second address
 * @param filter data filter for selecting address properties to compare. Use macro MNLXT_FLAG to create filter from
 * @mnlxt_rt_addr_data_t.
 * @return 0 for equal, else MNLXT_RT_ADDR_* + 1 for property which does not match
 */
int mnlxt_rt_addr_compare(const mnlxt_rt_addr_t *rt_addr1, const mnlxt_rt_addr_t *rt_addr2, uint64_t filter);
/**
 * Initializes netlink message from address information
 * @param nlh pointer to netlink message header to initialize
 * @param addr pointer to address information
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_put(struct nlmsghdr *nlh, const mnlxt_rt_addr_t *addr);
/**
 * Callback wrapper for mnlxt_rt_addr_put
 * @param nlh pointer to netlink message header to initialize
 * @param addr address information given by void pointer
 * @param nlmsg_type message type (will be ignored)
 * @return 0 on success, else -1
 */
static inline int mnlxt_rt_addr_PUT(struct nlmsghdr *nlh, const void *addr, uint16_t nlmsg_type) {
	(void)nlmsg_type;
	return mnlxt_rt_addr_put(nlh, (mnlxt_rt_addr_t *)addr);
}
/**
 * Parses netlink message into address information structure and stores it into mnlxt data
 * @param nlh pointer to netlink message
 * @param data pointer to mnlxt data
 * @return return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_addr_data(const struct nlmsghdr *nlh, mnlxt_data_t *data);
/**
 * Callback wrapper for mnlxt_rt_addr_data
 * @param nlh pointer to netlink message
 * @param data mnlxt data given by void pointer
 * @return return MNL_CB_OK on success, else MNL_CB_ERROR
 */
static inline int mnlxt_rt_addr_DATA(const struct nlmsghdr *nlh, void *data) {
	return mnlxt_rt_addr_data(nlh, (mnlxt_data_t *)data);
}

/**
 * Iterates over address informations stored in mnlxt data
 * @param data pointer to mnlxt data
 * @param iterator pointer to mnlxt message pointer; this pointer have to be initialized with NULL before iteration
 * @return pointer to the next address information or NULL for the end of iteration
 */
mnlxt_rt_addr_t *mnlxt_rt_addr_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator);

/**
 * Gets address information from mnlxt message.
 * In case, if you will handle the rule information independently from the mnlxt message use @mnlxt_rt_addr_remove
 * instead. Or clone it with @mnlxt_rt_addr_clone before calling @mnlxt_message_free.
 * @param message pointer to mnlxt message
 * @return pointer to address information structure on success, else NULL
 */
mnlxt_rt_addr_t *mnlxt_rt_addr_get(const mnlxt_message_t *message);
/**
 * Removes address information from mnlxt message.
 * Unlike @mnlxt_rt_addr_get it will detach address information from the message.
 * @param message pointer to mnlxt message
 * @return pointer to address information structure on success, else NULL
 */
mnlxt_rt_addr_t *mnlxt_rt_addr_remove(mnlxt_message_t *message);
/**
 * Creates a mnxt message and stores the given address information into it
 * @param addr double pointer to an address information structure mnlxt_rt_addr_t
 * @param type message type (RTM_NEWADDR or RTM_DELADDR)
 * @param flags request flags (NLM_F_* see linux/netlink.h). Set to 0 for default.
 * @return pointer to mnlxt message on success (pointer to the given mnlxt_rt address will be reset) else NULL
 */
mnlxt_message_t *mnlxt_rt_addr_message(mnlxt_rt_addr_t **addr, uint16_t type, uint16_t flags);

/**
 * Sends a netlink request with the given address information
 * @param rt_addr pointer to an address information structure mnlxt_rt_addr_t
 * @param type request type (RTM_NEWADDR or RTM_DELADDR)
 * @param flags request flags (NLM_F_* see linux/netlink.h). Set to 0 for default.
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_request(mnlxt_rt_addr_t *rt_addr, uint16_t type, uint16_t flags);
/**
 * Gets information of all addresses configured on system
 * @param data pointer to mnlxt data to store information into
 * @param family address family to get the information for
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_dump(mnlxt_data_t *data, unsigned char family);

#endif /* LIBMNLXT_RT_ADDR_H_ */
