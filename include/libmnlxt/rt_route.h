/*
 * libmnlxt/rt_route.h		Libmnlxt Routing Routes
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_RT_ROUTE_H_
#define LIBMNLXT_RT_ROUTE_H_

#include <sys/socket.h>

#include <libmnlxt/rt_addr.h>

typedef enum {
	MNLXT_RT_ROUTE_FAMILY = 0,
	MNLXT_RT_ROUTE_TABLE,
	MNLXT_RT_ROUTE_TYPE,
	MNLXT_RT_ROUTE_PROTOCOL,
	MNLXT_RT_ROUTE_SCOPE,
	MNLXT_RT_ROUTE_PRIORITY,
	MNLXT_RT_ROUTE_IIFINDEX,
	MNLXT_RT_ROUTE_OIFINDEX,
	MNLXT_RT_ROUTE_SRC_PREFIX,
	MNLXT_RT_ROUTE_DST_PREFIX,
	MNLXT_RT_ROUTE_SRC,
	MNLXT_RT_ROUTE_DST,
	MNLXT_RT_ROUTE_GATEWAY
#define MNLXT_RT_ROUTE_MAX MNLXT_RT_ROUTE_GATEWAY + 1
} nl_route_data_t;

typedef struct {
	/** Properties flags */
	uint32_t prop_flags;
	/** Address family of route */
	uint8_t family;
	/** Routing table ID, @see rtnetlink(7);
	 * RT_TABLE_UNSPEC    an unspecified routing table
	 * RT_TABLE_DEFAULT   the default table
	 * RT_TABLE_MAIN      the main table
	 * RT_TABLE_LOCAL     the local table
	 * */
	uint8_t table;
	/**
	 * Type of route, @see rtnetlink(7):
	 * RTN_UNSPEC
	 * RTN_UNICAST			Gateway or direct route
	 * RTN_LOCAL				Accept locally
	 * RTN_BROADCAST		Accept locally as broadcast, send as broadcast
	 * RTN_ANYCAST			Accept locally as broadcast, but send as unicast
	 * RTN_MULTICAST		Multicast route
	 * RTN_BLACKHOLE		Drop
	 * RTN_UNREACHABLE	Destination is unreachable
	 * RTN_PROHIBIT			Administratively prohibited
	 * RTN_THROW				Not in this table
	 * RTN_NAT					Translate this address
	 * RTN_XRESOLVE
	 */
	uint8_t type;
	/** Routing protocol; @see rtnetlink(7) or linux/rtnetlink.h
	 * RTPROT_UNSPEC     unknown
	 * RTPROT_REDIRECT   ICMP redirect (currently unused)
	 * RTPROT_KERNEL     Installed kernel
	 * RTPROT_BOOT       Installed during boot
	 * RTPROT_STATIC     Installed by the administrator
	 * RTPROT_GATED      Apparently, GateD
	 * RTPROT_RA         RDISC/ND router advertisements
	 * RTPROT_MRT        Merit MRT
	 * RTPROT_ZEBRA      Zebra
	 * RTPROT_BIRD       BIRD
	 * RTPROT_DNROUTED   DECnet routing daemon
	 * RTPROT_XORP       XORP
	 * RTPROT_NTK        Netsukuku
	 * RTPROT_DHCP       DHCP client
	 * RTPROT_MROUTED    Multicast daemon
	 * RTPROT_BABEL      Babel daemon
	 * */
	uint8_t protocol;
	/**
	 * 1 - 199 are user defined
	 * RT_SCOPE_UNIVERSE
	 * RT_SCOPE_SITE
	 * RT_SCOPE_LINK
	 * RT_SCOPE_HOST
	 * RT_SCOPE_NOWHERE
	 */
	uint8_t scope;
	uint8_t src_prefix;
	uint8_t dst_prefix;
	uint8_t padding;
	/** Priority of route */
	uint32_t priority;
	/** Input interface index. */
	uint32_t iif_index;
	/** Output interface index. */
	uint32_t oif_index;
	/** Route source address */
	inet_addr_t src;
	/** Route destination address */
	inet_addr_t dst;
	/** Route gateway address */
	inet_addr_t gateway;
} mnlxt_rt_route_t;

/**
 * Creates a new route information
 * @return pointer at new dynamically allocated route information structure
 */
mnlxt_rt_route_t *mnlxt_rt_route_new();
/**
 * Frees memory allocated by a route information structure
 * @param rtnl_route pointer at route information structure to free
 */
void mnlxt_rt_route_free(mnlxt_rt_route_t *rtnl_route);
/**
 * Callback wrapper for mnlxt_rt_route_free
 * @param route route information structure to free given by void pointer
 */
void mnlxt_rt_route_FREE(void *route);
/**
 * Sets route family on route information
 * @param route pointer at route information structure
 * @param family route family
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_family(mnlxt_rt_route_t *route, uint8_t family);
/**
 * Gets route family from route information
 * @param route pointer at route information structure
 * @param family pointer at buffer to save route family
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_family(const mnlxt_rt_route_t *route, uint8_t *family);
/**
 * Sets route protocol on route information
 * @param route pointer at route information structure
 * @param protocol route protocol
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_protocol(mnlxt_rt_route_t *route, uint8_t protocol);
/**
 * Gets route protocol from route information
 * @param route pointer at route information structure
 * @param protocol pointer at buffer to save route protocol
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_protocol(const mnlxt_rt_route_t *route, uint8_t *protocol);
/**
 * Sets route table on route information
 * @param route pointer at route information structure
 * @param table route table number
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_table(mnlxt_rt_route_t *route, uint8_t table);
/**
 * Gets route table from route information
 * @param route pointer at route information structure
 * @param table pointer at buffer to save route table number
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_table(const mnlxt_rt_route_t *route, uint8_t *table);
/**
 * Sets route scope on route information
 * @param route pointer at route information structure
 * @param scope route scope
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_scope(mnlxt_rt_route_t *route, uint8_t scope);
/**
 * Gets route scope from route information
 * @param route pointer at route information structure
 * @param scope pointer at buffer to save route scope
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_scope(const mnlxt_rt_route_t *route, uint8_t *scope);
/**
 * Sets route type on route information
 * @param route pointer at route information structure
 * @param type route type
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_type(mnlxt_rt_route_t *route, uint8_t type);
/**
 * Gets route type from route information
 * @param route pointer at route information structure
 * @param type pointer at buffer to save route type
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_type(const mnlxt_rt_route_t *route, uint8_t *type);
/**
 * Sets route source prefix on route information
 * @param route pointer at route information structure
 * @param prefixlen source prefix length
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_src_prefix(mnlxt_rt_route_t *route, uint8_t prefixlen);
/**
 * Gets route source prefix from route information
 * @param route pointer at route information structure
 * @param prefixlen pointer at buffer to save route source prefix length
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_src_prefix(const mnlxt_rt_route_t *route, uint8_t *prefixlen);
/**
 * Sets route destination prefix on route information
 * @param route pointer at route information structure
 * @param prefixlen destination prefix length
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_dst_prefix(mnlxt_rt_route_t *route, uint8_t prefixlen);
/**
 * Gets route destination prefix from route information
 * @param route pointer at route information structure
 * @param prefixlen pointer at buffer to save route destination prefix length
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_dst_prefix(const mnlxt_rt_route_t *route, uint8_t *prefixlen);
/**
 * Sets route source address on route information
 * @param route pointer at route information structure
 * @param family address family
 * @param buf pointer at route buffer with source address
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_src(mnlxt_rt_route_t *route, uint8_t family, inet_addr_t *buf);
/**
 * Gets route source address from route information
 * @param route pointer at route information structure
 * @param family pointer at buffer to save address family
 * @param buf pointer at buffer to save pointer at route source
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_src(const mnlxt_rt_route_t *route, uint8_t *family, const inet_addr_t **buf);
/**
 * Sets route destination address on route information
 * @param route pointer at route information structure
 * @param family address family
 * @param buf pointer at route buffer with destination address
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_dst(mnlxt_rt_route_t *route, uint8_t family, inet_addr_t *buf);
/**
 * Gets route destination address from route information
 * @param route pointer at route information structure
 * @param family pointer at buffer to save address family
 * @param buf pointer at buffer to save pointer at route destination
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_dst(const mnlxt_rt_route_t *route, uint8_t *family, const inet_addr_t **buf);
/**
 * Sets route gateway address on route information
 * @param route pointer at route information structure
 * @param family address family
 * @param buf pointer at route buffer with gateway address
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_gateway(mnlxt_rt_route_t *route, uint8_t family, inet_addr_t *buf);
/**
 * Gets route gateway address from route information
 * @param route pointer at route information structure
 * @param family pointer at buffer to save address family
 * @param buf pointer at buffer to save pointer at route gateway
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_gateway(const mnlxt_rt_route_t *route, uint8_t *family, const inet_addr_t **buf);
/**
 * Sets route priority prefix on route information
 * @param route pointer at route information structure
 * @param priority route priority
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_priority(mnlxt_rt_route_t *route, uint32_t priority);
/**
 * Gets route priority from route information
 * @param route pointer at route information structure
 * @param priority pointer at buffer to save address priority
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_priority(const mnlxt_rt_route_t *route, uint32_t *priority);
/**
 * Sets route output interface on route information
 * @param route pointer at route information structure
 * @param oif_index output interface index
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_oifindex(mnlxt_rt_route_t *route, uint32_t oif_index);
/**
 * Sets route output interface on route information
 * @param route pointer at route information structure
 * @param oif_index pointer at buffer to save output interface index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_oifindex(const mnlxt_rt_route_t *route, uint32_t *oif_index);
/**
 * Sets route input interface on route information
 * @param route pointer at route information structure
 * @param iif_index input interface index
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_set_iifindex(mnlxt_rt_route_t *route, uint32_t iif_index);
/**
 * Sets route output interface on route information
 * @param route pointer at route information structure
 * @param iif_index pointer at buffer to save input interface index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_route_get_iifindex(const mnlxt_rt_route_t *route, uint32_t *iif_index);

/**
 * Checks if a route information matches another one
 * @param route pointer at route information to check
 * @param match pointer at route information to match
 * @return 0 for matching, else MNLXT_RT_ROUTE_* + 1 for property which does not match
 */
int mnlxt_rt_route_match(const mnlxt_rt_route_t *route, const mnlxt_rt_route_t *match);

/**
 * Initializes netlink message from route information
 * @param nlh pointer at netlink message header to initialize
 * @param route pointer at route information structure
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_put(struct nlmsghdr *nlh, const mnlxt_rt_route_t *route);
/**
 * Callback wrapper for mnlxt_rt_route_put
 * @param nlh pointer at netlink message header to initialize
 * @param route route information structure given by void pointer
 * @param nlmsg_type message type (will be ignored)
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_PUT(struct nlmsghdr *nlh, const void *route, uint16_t nlmsg_type);

/**
 * Parses netlink message into route information and stores it into netlink data
 * @param nlh pointer at netlink message
 * @param data pointer at mnlxt data
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_route_data(const struct nlmsghdr *nlh, mnlxt_data_t *data);
/**
 * Callback wrapper for mnlxt_rt_route_data
 * @param nlh pointer at netlink message
 * @param data mnlxt data by void pointer
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_route_DATA(const struct nlmsghdr *nlh, void *data);

/**
 * Iterates over route informations stored in mnlxt data
 * @param data pointer at mnlxt data
 * @param iterator pointer at mnlxt message pointer; this pointer have to be initialized with NULL before iteration
 * @return pointer at the next route information or NULL for the end of iteration
 */
mnlxt_rt_route_t *mnlxt_rt_route_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator);

/**
 * Gets route information from netlink message
 * @param message pointer at mnlxt message
 * @return pointer at route information structure on success, else NULL
 */
mnlxt_rt_route_t *mnlxt_rt_route_get(const mnlxt_message_t *message);
/**
 * Creates a mnlxt message and stores the given route information into it
 * @param route double pointer at a route information structure mnlxt_rt_route_t
 * @param type message type (RTM_NEWROUTE or RTM_DELROUTE)
 * @return pointer at mnlxt message on success (pointer at the given mnlxt_rt route will be reset) else NULL
 */
mnlxt_message_t *mnlxt_rt_route_message(mnlxt_rt_route_t **route, uint16_t type);

/**
 * Gets information of all routes configured on system
 * @param data pointer at mnlxt data to store information into
 * @param family route family get the information for
 * @return 0 on success, else -1
 */
int mnlxt_rt_route_dump(mnlxt_data_t *data, unsigned char family);

#endif /* LIBMNLXT_RT_ROUTE_H_ */
