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
#include <netinet/in.h>

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
	MNLXT_RT_LINK_XFRM_IFINDEX = 0,
	MNLXT_RT_LINK_XFRM_ID
#define MNLXT_RT_LINK_XFRM_MAX MNLXT_RT_LINK_XFRM_ID + 1
} mnlxt_rt_link_xfrm_data_t;

typedef struct {
	uint32_t if_index;
	uint32_t id;
} mnlxt_rt_link_xfrm_t;

typedef enum {
	MNLXT_RT_LINK_INFO_KIND_BR = 0,
	MNLXT_RT_LINK_INFO_KIND_VLAN,
	MNLXT_RT_LINK_INFO_KIND_BOND,
	MNLXT_RT_LINK_INFO_KIND_TUN,
	MNLXT_RT_LINK_INFO_KIND_XFRM,
#define MNLXT_RT_LINK_INFO_KIND_MAX MNLXT_RT_LINK_INFO_KIND_XFRM + 1
} mnlxt_rt_link_info_kind_t;

typedef struct {
	uint16_t prop_flags;
	mnlxt_rt_link_info_kind_t kind;
	union {
		mnlxt_rt_link_vlan_t vlan;
		mnlxt_rt_link_xfrm_t xfrm;
	} data;
} mnlxt_rt_link_info_t;

typedef struct {
	/** Properties flags */
	uint16_t prop_flags;
	/** link type, see linux/if_arp.h
	 * ARPHRD_PPP
	 * ARPHRD_ETHER
	 * ARPHRD_LOOPBACK
	 * ...
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
	char name[IFNAMSIZ];
	/** Interface index */
	uint32_t index;
	/** link flags, see linux/if.h and linux/if_tun.h
	 * IFF_UP
	 * IFF_BROADCAST
	 * IFF_DEBUG
	 * IFF_LOOPBACK
	 * IFF_RUNNING
	 * ...
	 */
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
 * @return pointer to new dynamically allocated link instance
 */
mnlxt_rt_link_t *mnlxt_rt_link_new();
/**
 * Makes a copy of a link instance
 * @param rt_link source address to copy from
 * @param filter data filter. In case of 0, the function is equal to @mnlxt_rt_link_new().
 * Use macro MNLXT_FLAG to create filter from @mnlxt_rt_link_data_t.
 * @return pointer to copy on success, else NULL
 */
mnlxt_rt_link_t *mnlxt_rt_link_clone(const mnlxt_rt_link_t *rt_link, uint64_t filter);
/**
 * Frees memory allocated by a link instance
 * @param link pointer to link instance to free
 */
void mnlxt_rt_link_free(mnlxt_rt_link_t *link);
/**
 * Callback wrapper for mnlxt_rt_link_free
 * @param link void pointer to link instance to free
 */
void mnlxt_rt_link_FREE(void *link);
/**
 * Revoke link type data on link instance
 * @param link pointer to link instance
 * @param index one of MNLXT_RT_LINK_*
 */
void mnlxt_rt_link_data_revoke(mnlxt_rt_link_t *link, mnlxt_rt_link_data_t index);
/**
 * Gets link type from link instance
 * @param link pointer to link instance
 * @param type pointer to buffer to save link type
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_type(const mnlxt_rt_link_t *link, uint16_t *type);
/**
 * Sets link type on link instance
 * @param link pointer to link instance
 * @param type link type
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_type(mnlxt_rt_link_t *link, uint16_t type);
/**
 * Gets link family from link instance
 * @param link pointer to link instance
 * @param family pointer to buffer to save link family
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_family(const mnlxt_rt_link_t *link, uint8_t *family);
/**
 * Sets family on link instance
 * @param link pointer to link instance
 * @param family link family
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_family(mnlxt_rt_link_t *link, uint8_t family);
/**
 * Gets link name from link instance
 * @param link pointer to link instance
 * @param name pointer to buffer to store pointer to name
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_name(const mnlxt_rt_link_t *link, const char **name);
/**
 * Sets link name on link instance by copying the string. The maximal length of the name is limited to 15 symbols.
 * @param link pointer to link instance
 * @param name pointer to link name
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_name(mnlxt_rt_link_t *link, const char *name);
/**
 * Gets link index from link instance
 * @param link pointer to link instance
 * @param index pointer to buffer to save link index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_index(const mnlxt_rt_link_t *link, uint32_t *index);
/**
 * Sets link index on link instance
 * @param link pointer to link instance
 * @param index link index
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_index(mnlxt_rt_link_t *link, uint32_t index);
/**
 * Gets link flags from link instance
 * @param link pointer to link instance
 * @param flags pointer to buffer to save link flags
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_flags(const mnlxt_rt_link_t *link, uint32_t *flags);
/**
 * Sets all link flags at once on link instance.
 * For setting some particular flags on network device use @mnlxt_rt_link_set_flags_up
 * or @mnlxt_rt_link_set_flags_down instead.
 * @param link pointer to link instance
 * @param flags link flags
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_flags(mnlxt_rt_link_t *link, uint32_t flags);
/**
 * Sets selected link flags to on/1 on link instance.
 * @param link pointer to link instance
 * @param flags flags to be set
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_flags_on(mnlxt_rt_link_t *link, uint32_t flags);
/**
 * Sets selected link flags to off/0 on link instance.
 * @param link pointer to link instance
 * @param flags flags to be set
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_flags_off(mnlxt_rt_link_t *link, uint32_t flags);
#if 0 /* not in use yet */
/**
 * Resets (previously set to 1 or 0) link flags on link instance.
 * @param link pointer to link instance
 * @param flags flags to reset
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_reset_flags(mnlxt_rt_link_t *link, uint32_t flags);
#endif
/**
 * Gets mtu from link instance
 * @param link pointer to link instance
 * @param mtu pointer to buffer to save link mtu
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_mtu(const mnlxt_rt_link_t *link, uint32_t *mtu);
/**
 * Sets mtu on link instance
 * @param link pointer to link instance
 * @param mtu link mtu
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_mtu(mnlxt_rt_link_t *link, uint32_t mtu);
/**
 * Gets link master from link instance
 * @param link pointer to link instance
 * @param master pointer to buffer to save link master's index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_master(const mnlxt_rt_link_t *link, uint32_t *master);
/**
 * Sets master on link instance
 * @param link pointer to link instance
 * @param master link master's index
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_master(mnlxt_rt_link_t *link, uint32_t master);
/**
 * Gets link state from link instance
 * @param link pointer to link instance
 * @param state pointer to buffer to save link state
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_state(const mnlxt_rt_link_t *link, uint8_t *state);
/**
 * Sets state on link instance
 * @param link pointer to link instance
 * @param state link state
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_state(mnlxt_rt_link_t *link, uint8_t state);
/**
 * Gets link address from link instance
 * @param link pointer to link instance
 * @param mac pointer to buffer to save link address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_hwaddr(const mnlxt_rt_link_t *link, eth_addr_t *mac);
/**
 * Sets address on link instance
 * @param link pointer to link instance
 * @param mac link address
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_hwaddr(mnlxt_rt_link_t *link, eth_addr_t mac);
/**
 * Gets link UP/DOWN state from link instance
 * @param link pointer to link instance
 * @param up pointer to buffer to save link state
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_updown(const mnlxt_rt_link_t *link, int *up);
/**
 * Sets UP/DOWN state on link instance
 * @param link pointer to link instance
 * @param up link state (0 for DOWN, else UP)
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_updown(mnlxt_rt_link_t *link, int up);
/**
 * Gets link parent from link instance
 * @param link pointer to link instance
 * @param index pointer to buffer to save link parent's index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_parent(const mnlxt_rt_link_t *link, uint32_t *index);
/**
 * Sets parent on link instance
 * @param link pointer to link instance
 * @param index link parent's index
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_parent(mnlxt_rt_link_t *link, uint32_t index);
/**
 * Gets information kind from link instance
 * @param link pointer to link instance
 * @param info_kind pointer to buffer to save information kind
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_info_kind(const mnlxt_rt_link_t *link, mnlxt_rt_link_info_kind_t *info_kind);
/**
 * Sets information kind on link instance
 * @param link pointer to link instance
 * @param info_kind information kind
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_info_kind(mnlxt_rt_link_t *link, mnlxt_rt_link_info_kind_t info_kind);
/**
 * Gets vlan id from link instance
 * @param link pointer to link instance
 * @param id pointer to buffer to save vlan's id
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_vlan_id(const mnlxt_rt_link_t *link, uint16_t *id);
/**
 * Sets vlan id on link instance
 * @param link pointer to link instance
 * @param id vlan id
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_vlan_id(mnlxt_rt_link_t *link, uint16_t id);
/**
 * Gets underlying interface for XFRM link
 * @param rt_link pointer to link instance
 * @param if_index buffer pointer to save interface index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_xfrm_ifindex(const mnlxt_rt_link_t *rt_link, uint32_t *if_index);
/**
 * Sets underlying interface for XFRM link
 * @param rt_link pointer to link instance
 * @param if_index index of underlying interface
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_xfrm_ifindex(mnlxt_rt_link_t *rt_link, uint32_t if_index);
/**
 * Gets interface ID for XFRM link
 * @param rt_link pointer to link instance
 * @param id buffer pointer to save interface ID
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_xfrm_id(const mnlxt_rt_link_t *rt_link, uint32_t *id);
/**
 * Sets interface ID for XFRM link
 * @param rt_link pointer to link instance
 * @param id interface ID
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_xfrm_id(mnlxt_rt_link_t *rt_link, uint32_t id);

/**
 * Checks if a link matches another one
 * @param link pointer to link to check
 * @param match pointer to link to match
 * @return 0 for matching, -1 for error, else MNLXT_RT_LINK_* + 1 for property which does not match
 */
int mnlxt_rt_link_match(const mnlxt_rt_link_t *link, const mnlxt_rt_link_t *match);
/**
 * Compares two link instances.
 * This function excludes comparison of link info properties. Use @mnlxt_rt_link_info_compare to do it.
 * @param rt_link1 pointer to first link instance
 * @param rt_link2 pointer to second link instance
 * @param filter data filter for selecting link properties to compare. Use macro MNLXT_FLAG to create filter from
 * @mnlxt_rt_link_data_t.
 * @return 0 for equal, -1 for error, else MNLXT_RT_LINK_* + 1 for property which does not match
 */
int mnlxt_rt_link_compare(const mnlxt_rt_link_t *rt_link1, const mnlxt_rt_link_t *rt_link2, uint64_t filter);
/**
 * Compares link info within two link instances
 * @param rt_link1 pointer to first link instance
 * @param rt_link2 pointer to second link instance
 * @param filter data filter for selecting link info properties to compare. Use macro MNLXT_FLAG to create filter from
 * mnlxt_rt_link_<kind>_data_t.
 * @return 0 for equal, -1 for error, else MNLXT_RT_LINK_<KIND>_* + 1 for property which does not match
 */
int mnlxt_rt_link_info_compare(const mnlxt_rt_link_t *rt_link1, const mnlxt_rt_link_t *rt_link2, uint16_t filter);
/**
 * Initializes netlink message from link instance
 * @param nlh pointer to netlink message header to initialize
 * @param link pointer to link instance
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_put(struct nlmsghdr *nlh, const mnlxt_rt_link_t *link);
/**
 * Callback Wrapper for mnlxt_rt_link_put
 * @param nlh pointer to netlink message header to initialize
 * @param link link instance given by void pointer
 * @param nlmsg_type message type (will be ignored)
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_PUT(struct nlmsghdr *nlh, const void *link, uint16_t nlmsg_type);

/**
 * Parses netlink message into link instance and stores it into mnlxt data
 * @param nlh pointer to mnlxt message
 * @param data pointer to mnlxt data
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_link_data(const struct nlmsghdr *nlh, mnlxt_data_t *data);
/**
 * Callback Wrapper for mnlxt_rt_link_data
 * @param nlh pointer to netlink message
 * @param data mnlxt data given by void pointer
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_link_DATA(const struct nlmsghdr *nlh, void *data);

/**
 * Iterates over links stored in mnlxt data
 * @param data pointer to mnlxt data
 * @param iterator pointer to mnlxt message pointer; this pointer have to be initialized with NULL before iteration
 * @return pointer to next link instance or NULL for the end of iteration
 */
mnlxt_rt_link_t *mnlxt_rt_link_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator);

/**
 * Gets link instance from mnlxt message.
 * In case, if you will handle the link instance independently from the mnlxt message use @mnlxt_rt_link_remove
 * instead. Or clone it with @mnlxt_rt_link_clone before calling @mnlxt_message_free.
 * @param message pointer to mnlxt message
 * @return pointer to link instance on success, else NULL
 */
mnlxt_rt_link_t *mnlxt_rt_link_get(const mnlxt_message_t *message);
/**
 * Remove link instance from mnlxt message.
 * Unlike @mnlxt_rt_link_get it will detach link instance from the message.
 * @param message pointer to mnlxt message
 * @return pointer to link instance on success, else NULL
 */
mnlxt_rt_link_t *mnlxt_rt_link_remove(mnlxt_message_t *message);
/**
 * Creates a mnlxt message and stores the given link instance into it
 * @param link double pointer to a link instance mnlxt_rt_link_t
 * @param type message type (RTM_NEWLINK, RTM_DELLINK or RTM_SETLINK)
 * @return pointer to mnlxt message on success (pointer to the given mnlxt_rt link will be reset) else NULL
 */
mnlxt_message_t *mnlxt_rt_link_message(mnlxt_rt_link_t **link, uint16_t type, uint16_t flags);

/**
 * Sends a netlink request with information from the given link instance
 * @param rt_link pointer to a link instance mnlxt_rt_link_t
 * @param type request type (RTM_NEWLINK, RTM_DELLINK or RTM_SETLINK)
 * @param flags request flags (NLM_F_* see linux/netlink.h). Set to 0 for default.
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_request(mnlxt_rt_link_t *link, uint16_t type, uint16_t flags);
/**
 * Gets information of all links configured on system
 * @param data pointer to mnlxt data to store information into
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_dump(mnlxt_data_t *data);

#endif /* LIBMNLXT_RT_LINK_H_ */
