/*
 * libmnlxt/rt_link.h		Libmnlxt Routing Link
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_RT_LINK_H_
#define LIBMNLXT_RT_LINK_H_

#include <net/ethernet.h>
#include <net/if.h>
#include <net/if_arp.h>

#include <linux/if.h>

#include <libmnlxt/data.h>

typedef enum {
	MNLXT_RT_LINK_TYPE = 0,
	MNLXT_RT_LINK_FAMILY,
	MNLXT_RT_LINK_NAME,
	MNLXT_RT_LINK_INDEX,
	MNLXT_RT_LINK_FLAGS,
	MNLXT_RT_LINK_HWADDR,
	MNLXT_RT_LINK_MTU,
	MNLXT_RT_LINK_MASTER,
	MNLXT_RT_LINK_STATE,
	MNLXT_RT_LINK_PARENT,
	MNLXT_RT_LINK_INFO
#define MNLXT_RT_LINK_MAX MNLXT_RT_LINK_INFO + 1
} mnlxt_rt_link_data_t;

typedef uint8_t eth_addr_t[ETH_ALEN];

typedef enum {
	MNLXT_RT_LINK_VLAN_ID = 0
#define MNLXT_RT_LINK_VLAN_MAX MNLXT_RT_LINK_VLAN_ID + 1
} mnlxt_rt_link_vlan_data_t;

typedef struct {
	uint16_t id;
} mnlxt_rt_link_vlan_t;

typedef enum {
	MNLXT_RT_LINK_INFO_KIND_BR = 0,
	MNLXT_RT_LINK_INFO_KIND_VLAN,
	MNLXT_RT_LINK_INFO_KIND_BOND
#define MNLXT_RT_LINK_INFO_KIND_MAX MNLXT_RT_LINK_INFO_KIND_BOND + 1
} mnlxt_rt_link_info_kind_t;

typedef struct {
	uint16_t prop_flags;
	mnlxt_rt_link_info_kind_t kind;
	union {
		mnlxt_rt_link_vlan_t vlan;
	} data;
} mnlxt_rt_link_info_t;

typedef struct {
	/** Properties flags */
	uint16_t prop_flags;
	/** link type, see linux/if_arp.h
	 * ARPHRD_PPP
	 * ARPHRD_ETHER
	 * ARPHRD_LOOPBACK
	 * ARPHRD_*
	 */
	uint16_t type;
	uint8_t family;
	/** link state, see linux/if.h, RFC2863
	 * IF_OPER_UNKNOWN
	 * IF_OPER_NOTPRESENT
	 * IF_OPER_DOWN
	 * IF_OPER_LOWERLAYERDOWN
	 * IF_OPER_TESTING
	 * IF_OPER_DORMANT
	 * IF_OPER_UP
	 */
	uint8_t state;
	/** Hardware address */
	eth_addr_t mac;
	/** Interface name */
	char *name;
	/** Interface index */
	uint32_t index;
	uint32_t flags;
	uint32_t flag_mask;
	/** MTU value */
	uint32_t mtu;
	/** Master interface index */
	uint32_t master;
	uint32_t parent;
	mnlxt_rt_link_info_t info;
} mnlxt_rt_link_t;

/**
 * Creates a new mnlxt_rt_link_t instance
 * @return pointer at new dynamically allocated link information structure
 */
mnlxt_rt_link_t *mnlxt_rt_link_new();
/**
 * Frees memory allocated by a link information structure
 * @param link pointer at link information structure to free
 */
void mnlxt_rt_link_free(mnlxt_rt_link_t *link);
/**
 * Callback wrapper for mnlxt_rt_link_free
 * @param link void pointer at link information structure to free
 */
void mnlxt_rt_link_FREE(void *link);
/**
 * Revoke link type data on link information
 * @param link pointer at link information structure
 * @param index one of MNLXT_RT_LINK_*
 */
void mnlxt_rt_link_data_revoke(mnlxt_rt_link_t *link, mnlxt_rt_link_data_t index);
/**
 * Gets link type from link information
 * @param link pointer at link information structure
 * @param type pointer at buffer to save link type
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_type(const mnlxt_rt_link_t *link, uint16_t *type);
/**
 * Sets link type on link information
 * @param link pointer at link information structure
 * @param type link type
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_type(mnlxt_rt_link_t *link, uint16_t type);
/**
 * Gets link family from link information
 * @param link pointer at link information structure
 * @param family pointer at buffer to save link family
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_family(const mnlxt_rt_link_t *link, uint8_t *family);
/**
 * Sets family on link information
 * @param link pointer at link information structure
 * @param family link family
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_family(mnlxt_rt_link_t *link, uint8_t family);
/**
 * Gets link name from link information
 * @param link pointer at link information structure
 * @param name pointer at buffer to store pointer at name
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_name(const mnlxt_rt_link_t *link, const char **name);
/**
 * Sets link name on link information by copying the string
 * @param link pointer at link information structure
 * @param name pointer at link name
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_name(mnlxt_rt_link_t *link, const char *name);
/**
 * Gets link index from link information
 * @param link pointer at link information structure
 * @param index pointer at buffer to save link index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_index(const mnlxt_rt_link_t *link, uint32_t *index);
/**
 * Sets link index on link information
 * @param link pointer at link information structure
 * @param index link index
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_index(mnlxt_rt_link_t *link, uint32_t index);
/**
 * Gets link flags from link information
 * @param link pointer at link information structure
 * @param flags pointer at buffer to save link flags
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_flags(const mnlxt_rt_link_t *link, uint32_t *flags);
/**
 * Sets link flags on link information
 * @param link pointer at link information structure
 * @param flags link flags
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_flags(mnlxt_rt_link_t *link, uint32_t flags);
/**
 * Gets mtu from link information
 * @param link pointer at link information structure
 * @param mtu pointer at buffer to save link mtu
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_mtu(const mnlxt_rt_link_t *link, uint32_t *mtu);
/**
 * Sets mtu on link information
 * @param link pointer at link information structure
 * @param mtu link mtu
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_mtu(mnlxt_rt_link_t *link, uint32_t mtu);
/**
 * Gets link master from link information
 * @param link pointer at link information structure
 * @param master pointer at buffer to save link master's index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_master(const mnlxt_rt_link_t *link, uint32_t *master);
/**
 * Sets master on link information
 * @param link pointer at link information structure
 * @param master link master's index
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_master(mnlxt_rt_link_t *link, uint32_t master);
/**
 * Gets link state from link information
 * @param link pointer at link information structure
 * @param state pointer at buffer to save link state
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_state(const mnlxt_rt_link_t *link, uint8_t *state);
/**
 * Sets state on link information
 * @param link pointer at link information structure
 * @param state link state
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_state(mnlxt_rt_link_t *link, uint8_t state);
/**
 * Gets link address from link information
 * @param link pointer at link information structure
 * @param mac pointer at buffer to save link address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_hwaddr(const mnlxt_rt_link_t *link, eth_addr_t *mac);
/**
 * Sets address on link information
 * @param link pointer at link information structure
 * @param mac link address
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_hwaddr(mnlxt_rt_link_t *link, eth_addr_t mac);
/**
 * Gets link UP/DOWN state from link information
 * @param link pointer at link information structure
 * @param up pointer at buffer to save link state
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_updown(const mnlxt_rt_link_t *link, int *up);
/**
 * Sets UP/DOWN state on link information
 * @param link pointer at link information structure
 * @param up link state (0 for DOWN, else UP)
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_updown(mnlxt_rt_link_t *link, int up);
/**
 * Gets link parent from link information
 * @param link pointer at link information structure
 * @param index pointer at buffer to save link parent's index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_parent(const mnlxt_rt_link_t *link, uint32_t *index);
/**
 * Sets parent on link information
 * @param link pointer at link information structure
 * @param index link parent's index
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_parent(mnlxt_rt_link_t *link, uint32_t index);
/**
 * Gets information kind from link information
 * @param link pointer at link information structure
 * @param info_kind pointer at buffer to save information kind
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_info_kind(const mnlxt_rt_link_t *link, mnlxt_rt_link_info_kind_t *info_kind);
/**
 * Sets information kind on link information
 * @param link pointer at link information structure
 * @param info_kind information kind
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_info_kind(mnlxt_rt_link_t *link, mnlxt_rt_link_info_kind_t info_kind);
/**
 * Gets vlan id from link information
 * @param link pointer at link information structure
 * @param id pointer at buffer to save vlan's id
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_vlan_id(const mnlxt_rt_link_t *link, uint16_t *id);
/**
 * Sets vlan id on link information
 * @param link pointer at link information structure
 * @param id vlan id
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_vlan_id(mnlxt_rt_link_t *link, uint16_t id);

/**
 * Checks if a link information matches another one
 * @param link pointer at link information to check
 * @param match pointer at link information to match
 * @return 0 for matching, -1 for error, else MNLXT_RT_LINK_* + 1 for property which does not match
 */
int mnlxt_rt_link_match(const mnlxt_rt_link_t *link, const mnlxt_rt_link_t *match);

/**
 * Initializes netlink message from link information
 * @param nlh pointer at netlink message header to initialize
 * @param link pointer at link information structure
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_put(struct nlmsghdr *nlh, const mnlxt_rt_link_t *link);
/**
 * Callback Wrapper for mnlxt_rt_link_put
 * @param nlh pointer at netlink message header to initialize
 * @param link link information structure given by void pointer
 * @param nlmsg_type message type (will be ignored)
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_PUT(struct nlmsghdr *nlh, const void *link, uint16_t nlmsg_type);

/**
 * Parses netlink message into link information and stores it into mnlxt data
 * @param nlh pointer at mnlxt message
 * @param data pointer at mnlxt data
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_link_data(const struct nlmsghdr *nlh, mnlxt_data_t *data);
/**
 * Callback Wrapper for mnlxt_rt_link_data
 * @param nlh pointer at netlink message
 * @param data mnlxt data given by void pointer
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_link_DATA(const struct nlmsghdr *nlh, void *data);

/**
 * Iterates over link informations stored in mnlxt data
 * @param data pointer at mnlxt data
 * @param iterator pointer at mnlxt message pointer; this pointer have to be initialized with NULL before iteration
 * @return pointer at next link information or NULL for the end of iteration
 */
mnlxt_rt_link_t *mnlxt_rt_link_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator);

/**
 * Gets link information from mnlxt message
 * @param message pointer at mnlxt message
 * @return pointer at link information structure on success, else NULL
 */
mnlxt_rt_link_t *mnlxt_rt_link_get(const mnlxt_message_t *message);
/**
 * Creates a mnlxt message and stores the given link information into it
 * @param link double pointer at a link information structure mnlxt_rt_link_t
 * @param type message type (RTM_NEWLINK, RTM_DELLINK or RTM_SETLINK)
 * @return pointer at mnlxt message on success (pointer at the given mnlxt_rt link will be reset) else NULL
 */
mnlxt_message_t *mnlxt_rt_link_message(mnlxt_rt_link_t **link, uint16_t type);

/**
 * Gets information of all links configured on system
 * @param data pointer at mnlxt data to store information into
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_dump(mnlxt_data_t *data);

#endif /* LIBMNLXT_RT_LINK_H_ */
