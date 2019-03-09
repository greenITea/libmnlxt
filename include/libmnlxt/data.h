#ifndef INCLUDE_LIBMNLXT_DATA_H_
#define INCLUDE_LIBMNLXT_DATA_H_

#include "core.h"

typedef struct mnlxt_message_s {
	struct mnlxt_message_s *next;
	/** NETLINK-Message type, which it was part of */
	uint16_t nlmsg_type;
	/** Message body */
	void *payload;
	/** Message data handler */
	const mnlxt_data_cb_t *handler;
} mnlxt_message_t;

typedef struct {
	mnlxt_message_t *first, *last;
	const char *error_str;
	const mnlxt_data_cb_t *handlers;
	size_t nhandlers;
} mnlxt_data_t;

/**
 * Creates a mnlxt message
 * @return pointer at dynamic allocated mnlxt_message_t structure
 */
mnlxt_message_t *mnlxt_message_new();
/**
 * Frees memory allocated by mnlxt message
 * @param message pointer at mnlxt_message_t to free
 */
void mnlxt_message_free(mnlxt_message_t *message);
/**
 * Gets string representation of mnlxt message type
 * @param message pointer at mnlxt message
 * @return pointer at static allocated string with mnlxt message type or NULL on not known
 */
const char *mnlxt_message_type(mnlxt_message_t *message);
/**
 * Creates a request from a mnlxt message and sends it via netlink
 * @param message pointer at mnlxt message
 * @param bus bus type (NETLINK_* see linux/netlink.h)
 * @return 0 on success, else -1
 */
int mnlxt_message_request(mnlxt_message_t *message, int bus);
/**
 * Iterates over mnlxt data
 * @param data pointer at mnlxt data to iterate
 * @param message pointer at the previous mnlxt message or NULL for the first one
 * @return pointer at next mnlxt message or NULL for the end of iteration
 */
mnlxt_message_t *mnlxt_data_iterate(mnlxt_data_t *data, mnlxt_message_t *message);
/**
 * Removes mnlxt message from mnlxt data
 * @param data pointer at mnlxt data to remove from
 * @param message pointer at the mnlxt message to remove NULL for the first one
 * @return pointer at removed message or NULL for empty mnlxt data
 */
mnlxt_message_t *mnlxt_data_remove(mnlxt_data_t *data, mnlxt_message_t *message);
/**
 * Adds mnlxt message to mnlxt data
 * @param data pointer at mnlxt data to add a mnlxt message to
 * @param message pointer at the mnlxt message to add
 */
void mnlxt_data_add(mnlxt_data_t *data, mnlxt_message_t *message);
/**
 * Creates netlink message header for given mnlxt message
 * @param message pointer at mnlxt message to create netlink message header for
 * @return pointer at dynamic allocated nlmsghdr structure
 */
struct nlmsghdr *mnlxt_msghdr_create(mnlxt_message_t *message);
/**
 * Frees memory allocated with netlink message header
 * @param msghdr pointer at netlink message header to free
 */
void mnlxt_msghdr_free(struct nlmsghdr *msghdr);
/**
 * Parses netlink message from mnlxt buffer into mnlxt messages and stores it into mnlxt data
 * @param data pointer at mnlxt data to store mnlxt message into
 * @param buffer pointer at mnlxt buffer to get message from
 * @return 0 on success, else -1
 */
int mnlxt_data_parse(mnlxt_data_t *data, mnlxt_buffer_t *buffer);
/**
 * Cleans mnlxt buffer's content
 * @param buffer pointer at mnlxt buffer to clean
 */
void mnlxt_buffer_clean(mnlxt_buffer_t *buffer);
/**
 * Cleans mnlxt data
 * @param data pointer at mnlxt data to clean
 */
void mnlxt_data_clean(mnlxt_data_t *data);
/**
 * Dumps netlink data for given netlink message
 * @param data pointer at mnlxt data to store the result into
 * @param bus bus type (NETLINK_* see linux/netlink.h)
 * @param nlh pointer at netlink message
 * @return 0 on success, else -1
 */
int mnlxt_data_dump(mnlxt_data_t *data, int bus, struct nlmsghdr *nlh);

#endif /* INCLUDE_LIBMNLXT_DATA_H_ */
