/*
 * libmnlxt/xfrm_policy.h		Libmnlxt Xfrm/IPsec Policy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_XFRM_POLICY_H_
#define LIBMNLXT_XFRM_POLICY_H_

#include <linux/xfrm.h>

#include <libmnlxt/rt_addr.h>

typedef enum {
	MNLXT_XFRM_POLICY_FAMILY = 0,
	MNLXT_XFRM_POLICY_PROTO,
#if 0 /* TODO: ICMP, MH */
	MNLXT_XFRM_POLICY_PTYPE,
	MNLXT_XFRM_POLICY_PCODE,
#endif
	MNLXT_XFRM_POLICY_SRC_PREFIXLEN,
	MNLXT_XFRM_POLICY_DST_PREFIXLEN,
	MNLXT_XFRM_POLICY_SRC_ADDR,
	MNLXT_XFRM_POLICY_DST_ADDR,
	MNLXT_XFRM_POLICY_SRC_PORT,
	MNLXT_XFRM_POLICY_DST_PORT,
	MNLXT_XFRM_POLICY_INDEX,
	MNLXT_XFRM_POLICY_IFINDEX,
	MNLXT_XFRM_POLICY_PRIO,
	MNLXT_XFRM_POLICY_ACTION,
	MNLXT_XFRM_POLICY_DIR,
	MNLXT_XFRM_POLICY_MARK,
	MNLXT_XFRM_POLICY_TMPLS
#define MNLXT_XFRM_POLICY_MAX MNLXT_XFRM_POLICY_TMPLS + 1
} mnlxt_xfrm_policy_data_t;

typedef struct {
	/** AF_INET6 or AF_INET */
	uint8_t family;
	/** IPPROTO_ESP, IPPROTO_AH, IPPROTO_COMP, IPPROTO_ROUTING, IPPROTO_DSTOPTS or IPSEC_PROTO_ANY */
	uint8_t proto;
	/** XFRM_MODE_* see linux/xfrm.h */
	uint8_t mode;
	/** XFRM_SHARE_* linux/xfrm.h */
	uint8_t share;
	uint32_t reqid;
	uint32_t spi;
	/** source peer */
	inet_addr_t src;
	/** destination peer */
	inet_addr_t dst;
} mnlxt_xfrm_tmpl_t;

typedef struct {
	/** IPv4/IPv6 prefix length */
	uint8_t prefixlen;
	uint8_t padding;
	/** only for IPPROTO_TCP, IPPROTO_UDP, IPPROTO_SCTP or IPPROTO_DCCP */
	uint16_t port;
	inet_addr_t addr;
} mnlxt_xfrm_site_t;

typedef struct {
	uint32_t value;
	uint32_t mask;
} mnlxt_xfrm_mark_t;

typedef struct {
	/** Properties flags */
	uint16_t prop_flags;
	/** AF_INET6 or AF_INET */
	uint8_t family;
	/** IPPROTO_* see netinet/in.h */
	uint8_t proto;
	/** XFRM_POLICY_ALLOW or XFRM_POLICY_BLOCK */
	uint8_t action;
	/** traffic direction: XFRM_POLICY_IN, XFRM_POLICY_OUT or XFRM_POLICY_FWD see linux/xfrm.h */
	uint8_t dir;
	uint16_t tmpl_num;
	uint32_t user;
	uint32_t index;
	/** input interface index */
	uint32_t if_index;
	uint32_t priority;
	mnlxt_xfrm_site_t src;
	mnlxt_xfrm_site_t dst;
	mnlxt_xfrm_mark_t mark;
	mnlxt_xfrm_tmpl_t *tmpls;
} mnlxt_xfrm_policy_t;

/**
 * Creates a new xfrm policy instance
 * @return pointer at new dynamically allocated link information structure
 */
mnlxt_xfrm_policy_t *mnlxt_xfrm_policy_new();
/**
 * Frees memory allocated by dynamic allocated a xfrm policy structure
 * @param policy pointer at xfrm policy structure to free
 */
void mnlxt_xfrm_policy_free(mnlxt_xfrm_policy_t *policy);
/**
 * Callback wrapper for mnlxt_xfrm_policy_free
 * @param policy void pointer at xfrm policy structure to free
 */
void mnlxt_xfrm_policy_FREE(void *policy);
/**
 * Sets IP address family on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param family IP address family (AF_INET or AF_INET6 see sys/socket.h)
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_family(mnlxt_xfrm_policy_t *policy, uint8_t family);
/**
 * Gets IP address family from xfrm policy structure
 * @param policy pointer at xfrm policy structure
 * @param family pointer at buffer to store IP address family
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_family(const mnlxt_xfrm_policy_t *policy, uint8_t *family);
/**
 * Sets protocol on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param proto protocol (IPPROTO_* see netinet/in.h)
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_proto(mnlxt_xfrm_policy_t *policy, uint8_t proto);
/**
 * Gets protocol from xfrm policy structure
 * @param policy pointer at xfrm policy structure
 * @param proto pointer at buffer to store protocol
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_proto(const mnlxt_xfrm_policy_t *policy, uint8_t *proto);
/**
 * Sets source prefix length for source IP address on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param prefixlen prefix length
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_src_prefixlen(mnlxt_xfrm_policy_t *policy, uint8_t prefixlen);
/**
 * Gets prefix length for source IP address from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param prefixlen pointer at buffer to store prefix length
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_src_prefixlen(const mnlxt_xfrm_policy_t *policy, uint8_t *prefixlen);
/**
 * Sets prefix length for destination IP address on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param prefixlen prefix length
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_dst_prefixlen(mnlxt_xfrm_policy_t *policy, uint8_t prefixlen);
/**
 * Gets prefix length for destination IP address from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param prefixlen pointer at buffer to store prefix length
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_dst_prefixlen(const mnlxt_xfrm_policy_t *policy, uint8_t *prefixlen);
/**
 * Sets source IP address on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param family IP address family (AF_INET or AF_INET6 see sys/socket.h)
 * @param buf pointer at IP address buffer
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_src_addr(mnlxt_xfrm_policy_t *policy, uint8_t family, inet_addr_t *buf);
/**
 * Gets source IP address from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param family pointer at buffer to store IP address family or NULL
 * @param buf pointer at buffer to store IP address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_src_addr(const mnlxt_xfrm_policy_t *policy, uint8_t *family, const inet_addr_t **buf);
/**
 * Sets destination IP address on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param family IP address family (AF_INET or AF_INET6 see sys/socket.h)
 * @param buf pointer at IP address buffer
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_dst_addr(mnlxt_xfrm_policy_t *policy, uint8_t family, inet_addr_t *buf);
/**
 * Gets destination IP address from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param family pointer at buffer to store IP address family or NULL
 * @param buf pointer at buffer to store IP address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_dst_addr(const mnlxt_xfrm_policy_t *policy, uint8_t *family, const inet_addr_t **buf);
/**
 * Sets source port on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param port port number
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_src_port(mnlxt_xfrm_policy_t *policy, uint16_t port);
/**
 * Gets source port from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param port pointer at buffer to store port number
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_src_port(const mnlxt_xfrm_policy_t *policy, uint16_t *port);
/**
 * Sets destination port on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param port port number
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_dst_port(mnlxt_xfrm_policy_t *policy, uint16_t port);
/**
 * Gets destination port from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param port pointer at buffer to store port number
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_dst_port(const mnlxt_xfrm_policy_t *policy, uint16_t *port);
/**
 * Sets input interface index on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param ifindex interface index
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_ifindex(mnlxt_xfrm_policy_t *policy, uint32_t ifindex);
/**
 * Gets input interface index from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param ifindex pointer at buffer to store interface index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_ifindex(const mnlxt_xfrm_policy_t *policy, uint32_t *ifindex);
/**
 * Sets policy action on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param action policy action (XFRM_POLICY_ALLOW or XFRM_POLICY_BLOCK see linux/xfrm.h)
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_action(mnlxt_xfrm_policy_t *policy, uint8_t action);
/**
 * Gets policy action from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param action pointer at buffer to store policy action
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_action(const mnlxt_xfrm_policy_t *policy, uint8_t *action);
/**
 * Sets policy direction on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param dir policy direction
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_dir(mnlxt_xfrm_policy_t *policy, uint8_t dir);
/**
 * Gets  from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param dir pointer at buffer to store policy direction
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_dir(const mnlxt_xfrm_policy_t *policy, uint8_t *dir);
/**
 * Sets priority on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param priority policy's priority
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_priority(mnlxt_xfrm_policy_t *policy, uint32_t priority);
/**
 * Gets priority from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param priority pointer at buffer to store priority
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_priority(const mnlxt_xfrm_policy_t *policy, uint32_t *priority);
/**
 * Sets mark on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param mark mark
 * @param mask mark's mask
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_mark(mnlxt_xfrm_policy_t *policy, uint32_t mark, uint32_t mask);
/**
 * Gets mark from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param mark pointer at buffer to store mark or NULL
 * @param mask pointer at buffer to store mark's mask or NULL
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_mark(const mnlxt_xfrm_policy_t *policy, uint32_t *mark, uint32_t *mask);
/**
 * Sets policy index on xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param index policy index
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_set_index(mnlxt_xfrm_policy_t *policy, uint32_t index);
/**
 * Gets policy index from xfrm policy
 * @param policy pointer at xfrm policy structure
 * @param index pointer at buffer to store policy index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_xfrm_policy_get_index(const mnlxt_xfrm_policy_t *policy, uint32_t *index);

/**
 * Checks if a xfrm policy matches another one
 * @param policy pointer at xfrm policy structure
 * @param match pointer at policy to match
 * @return 0 for matching, else MNLXT_XFRM_POLICY_* + 1 for property which does not match
 */
int mnlxt_xfrm_policy_match(const mnlxt_xfrm_policy_t *policy, const mnlxt_xfrm_policy_t *match);

/**
 * Initializes netlink message from xfrm policy
 * @param nlh pointer at netlink message
 * @param policy pointer at xfrm policy structure
 * @param nlmsg_type netlink message type (XFRM_MSG_GETPOLICY, XFRM_MSG_NEWPOLICY, XFRM_MSG_UPDPOLICY or
 * XFRM_MSG_DELPOLICY see linux/xfrm.h)
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_put(struct nlmsghdr *nlh, const mnlxt_xfrm_policy_t *policy, uint16_t nlmsg_type);
/**
 * Callback wrapper for mnlxt_xfrm_policy_put
 * @param nlh pointer at netlink message
 * @param policy pointer at xfrm policy structure
 * @param nlmsg_type netlink message type
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_PUT(struct nlmsghdr *nlh, const void *policy, uint16_t nlmsg_type);

/**
 * Parses netlink message into xfrm policy structure and stores it into mnlxt data
 * @param nlh pointer at netlink message
 * @param data pointer at mnlxt data
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_xfrm_policy_data(const struct nlmsghdr *nlh, mnlxt_data_t *data);
/**
 * Callback wrapper for mnlxt_xfrm_policy_data
 * @param nlh pointer at netlink message
 * @param data pointer at mnlxt data
 * @return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_xfrm_policy_DATA(const struct nlmsghdr *nlh, void *data);

/**
 * Iterates over xfrm policies stored in mnlxt data
 * @param data pointer at mnlxt data
 * @param iterator pointer at mnlxt message pointer; this pointer have to be initialized with NULL before iteration
 * @return pointer at the next address information or NULL for the end of iteration
 */
mnlxt_xfrm_policy_t *mnlxt_xfrm_policy_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator);

/**
 * Gets xfrm policy from mnlxt message
 * @param message message pointer at mnlxt message
 * @return pointer at xfrm policy structure on success, else NULL
 */
mnlxt_xfrm_policy_t *mnlxt_xfrm_policy_get(const mnlxt_message_t *message);
/**
 * Creates a mnxt message and stores the given address information into it
 * @param policy double pointer at xfrm policy
 * @param type message type (XFRM_MSG_NEWPOLICY, XFRM_MSG_UPDPOLICY or XFRM_MSG_DELPOLICY see linux/xfrm.h)
 * @return pointer at mnlxt message on success (pointer at the given xfrm policy will be reset) else NULL
 */
mnlxt_message_t *mnlxt_xfrm_policy_message(mnlxt_xfrm_policy_t **policy, uint16_t type);

/**
 * Gets information of all policies configured on system
 * @param data pointer at mnlxt data to store information into
 * @return 0 on success, else -1
 */
int mnlxt_xfrm_policy_dump(mnlxt_data_t *data);

#endif /* LIBMNLXT_XFRM_POLICY_H_ */
