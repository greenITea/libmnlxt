#ifndef INCLUDE_LIBMNLXT_RT_ADDR_H_
#define INCLUDE_LIBMNLXT_RT_ADDR_H_

#include <netinet/in.h>
#include <sys/socket.h>

#include "data.h"

typedef enum {
	MNLXT_RT_ADDR_FAMILY = 0,
	MNLXT_RT_ADDR_PREFIXLEN,
	MNLXT_RT_ADDR_FLAGS,
	MNLXT_RT_ADDR_SCOPE,
	MNLXT_RT_ADDR_IFINDEX,
	MNLXT_RT_ADDR_ADDR,
	MNLXT_RT_ADDR_LOCAL,
	MNLXT_RT_ADDR_LABEL
#define MNLXT_RT_ADDR_MAX MNLXT_RT_ADDR_LABEL + 1
} mnlxt_rt_addr_data_t;

typedef union {
	struct in6_addr in6;
	struct in_addr in;
} inet_addr_t;

typedef struct {
	/** Properties flags */
	uint8_t prop_flags;
	/** AF_INET6 or AF_INET */
	uint8_t family;
	/** IPv4/IPv6 prefix length */
	uint8_t prefixlen;
	/** Interface address flags
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
	inet_addr_t addr;
	/** Local IP address, equal to addr except ppp-like interfaces */
	/* does it exists for IPv6? */
	inet_addr_t addr_local;
	/** Address label, IPv4 only */
	char *label;
} mnlxt_rt_addr_t;

/**
 * Creates new address information structure
 * @return pointer at dynamically allocated mnlxt_rt_addr_t on success, else NULL
 */
mnlxt_rt_addr_t *mnlxt_rt_addr_new();
/**
 * Frees memory allocated by netlink address information structure
 * @param nl_addr pointer at netlink address information structure to free
 */
void mnlxt_rt_addr_free(mnlxt_rt_addr_t *nl_addr);
/**
 * Frees memory allocated by an address information structure (callback version)
 * @param addr address information structure to free given by a void pointer
 */
void mnlxt_rt_addr_FREE(void *addr);
/**
 * Sets address family on address information
 * @param addr pointer at address information structure
 * @param family address family to set
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_family(mnlxt_rt_addr_t *addr, uint8_t family);
/**
 * Gets address family from address information
 * @param addr pointer at address information structure
 * @param family pointer at buffer to copy address family into
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_family(mnlxt_rt_addr_t *addr, uint8_t *family);
/**
 * Sets prefix length on address information
 * @param addr pointer at address information structure
 * @param prefixlen prefix length
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_prefixlen(mnlxt_rt_addr_t *addr, uint8_t prefixlen);
/**
 * Gets prefix length from address information
 * @param addr pointer at address information structure
 * @param prefixlen pointer at buffer to copy prefix length into
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_prefixlen(mnlxt_rt_addr_t *addr, uint8_t *prefixlen);
/**
 * Set flags on address information
 * @param addr pointer at address information structure
 * @param flags flags to set
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_flags(mnlxt_rt_addr_t *addr, uint32_t flags);
/**
 * Get flags from address information
 * @param addr pointer at address information structure
 * @param flags pointer at buffer to copy flags into
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_flags(mnlxt_rt_addr_t *addr, uint32_t *flags);
/**
 * Sets scope on address information
 * @param addr pointer at address information structure
 * @param scope address scope
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_scope(mnlxt_rt_addr_t *addr, uint8_t scope);
/**
 * Gets scope from address information
 * @param addr pointer at address information structure
 * @param scope pointer at buffer to copy address scope into
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_scope(mnlxt_rt_addr_t *addr, uint8_t *scope);
/**
 * Sets interface index on address information
 * @param addr pointer at address information structure
 * @param if_index interface index
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_ifindex(mnlxt_rt_addr_t *addr, uint32_t if_index);
/**
 * Gets interface index from address information
 * @param addr pointer at address information structure
 * @param if_index pointer at buffer to copy interface index into
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_ifindex(mnlxt_rt_addr_t *addr, uint32_t *if_index);
/**
 * Sets IP address on address information
 * @param addr pointer at address information structure
 * @param family address family
 * @param buf pointer at address buffer
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_addr(mnlxt_rt_addr_t *addr, uint8_t family, inet_addr_t *buf);
/**
 * Gets IP address from address information
 * @param addr pointer at address information structure
 * @param family pointer at buffer to store address family
 * @param buf pointer to store pointer at IP address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_addr(mnlxt_rt_addr_t *addr, uint8_t *family, inet_addr_t **buf);
/**
 * Sets local IP address on address information
 * @param addr pointer at address information structure
 * @param family address family
 * @param buf pointer at IP address buffer
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_local(mnlxt_rt_addr_t *addr, uint8_t family, inet_addr_t *buf);
/**
 * Gets local IP address from address information
 * @param addr pointer at address information structure
 * @param family pointer at buffer to store address family
 * @param buf pointer to store pointer at address
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_local(mnlxt_rt_addr_t *addr, uint8_t *family, inet_addr_t **buf);
/**
 * Sets label on address information by copying the string
 * @param addr pointer at address information structure
 * @param label label to set
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_set_label(mnlxt_rt_addr_t *addr, const char *label);
/**
 * Gets label from address information
 * @param addr pointer at address information structure
 * @param label pointer to store pointer at label
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_addr_get_label(mnlxt_rt_addr_t *addr, char **label);

/**
 * Checks if an address information matches another one
 * @param addr pointer at address to check
 * @param match pointer at address to match
 * @return 0 for matching, else MNLXT_RT_ADDR_* + 1 for property which does not match
 */
int mnlxt_rt_addr_match(mnlxt_rt_addr_t *addr, mnlxt_rt_addr_t *match);

/**
 * Initializes netlink message from address information
 * @param nlh pointer at netlink message header to initialize
 * @param addr pointer at address information
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_put(struct nlmsghdr *nlh, mnlxt_rt_addr_t *addr);
/**
 * Callback wrapper for mnlxt_rt_addr_put
 * @param nlh pointer at netlink message header to initialize
 * @param addr address information given by void pointer
 * @param nlmsg_type message type (will be ignored)
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_PUT(struct nlmsghdr *nlh, void *addr, uint16_t nlmsg_type);

/**
 * Parses netlink message into address information structure and stores it into mnlxt data
 * @param nlh pointer at netlink message
 * @param data pointer at mnlxt data
 * @return return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_addr_data(const struct nlmsghdr *nlh, mnlxt_data_t *data);
/**
 * Callback wrapper for mnlxt_rt_addr_data
 * @param nlh pointer at netlink message
 * @param data mnlxt data given by void pointer
 * @return return MNL_CB_OK on success, else MNL_CB_ERROR
 */
int mnlxt_rt_addr_DATA(const struct nlmsghdr *nlh, void *data);

/**
 * Iterates over address informations stored in mnlxt data
 * @param data pointer at mnlxt data
 * @param iterator pointer at mnlxt message pointer; this pointer have to be initialized with NULL before iteration
 * @return pointer at the next address information or NULL for the end of iteration
 */
mnlxt_rt_addr_t *mnlxt_rt_addr_iterate(mnlxt_data_t *data, mnlxt_message_t **iterator);

/**
 * Gets address information from mnlxt message
 * @param message pointer at mnlxt message
 * @return pointer at address information structure on success, else NULL
 */
mnlxt_rt_addr_t *mnlxt_rt_addr_get(mnlxt_message_t *message);
/**
 * Creates a mnxt message and stores the given address information into it
 * @param addr double pointer at an address information structure mnlxt_rt_addr_t
 * @param type message type (RTM_NEWADDR or RTM_DELADDR)
 * @return pointer at mnlxt message on success (pointer at the given mnlxt_rt address will be reset) else NULL
 */
mnlxt_message_t *mnlxt_rt_addr_message(mnlxt_rt_addr_t **addr, uint16_t type);

/**
 * Gets information of all addresses configured on system
 * @param data pointer at mnlxt data to store information into
 * @param family address family to get the information for
 * @return 0 on success, else -1
 */
int mnlxt_rt_addr_dump(mnlxt_data_t *data, unsigned char family);

#endif /* INCLUDE_LIBMNLXT_RT_ADDR_H_ */
