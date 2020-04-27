/*
 * libmnlxt/data.h		Libmnlxt Data
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_DATA_H_
#define LIBMNLXT_DATA_H_

#include <libmnlxt/core.h>

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
	char error_buf[512];
} mnlxt_data_t;

/**
 * Creates a mnlxt message
 * @return pointer to dynamic allocated mnlxt_message_t structure
 */
mnlxt_message_t *mnlxt_message_new();
/**
 * Frees memory allocated by mnlxt message
 * @param message pointer to mnlxt_message_t to free
 */
void mnlxt_message_free(mnlxt_message_t *message);
/**
 * Gets string representation of mnlxt message type
 * @param message pointer to mnlxt message
 * @return pointer to static allocated string with mnlxt message type or NULL on not known
 */
const char *mnlxt_message_type(const mnlxt_message_t *message);
/**
 * Creates a request from a mnlxt message and sends it via netlink
 * @param message pointer to mnlxt message
 * @param bus bus type (NETLINK_* see linux/netlink.h)
 * @return 0 on success, else -1
 */
int mnlxt_message_request(const mnlxt_message_t *message, int bus);
/**
 * Iterates over mnlxt data
 * @param data pointer to mnlxt data to iterate
 * @param message pointer to the previous mnlxt message or NULL for the first one
 * @return pointer to next mnlxt message or NULL for the end of iteration
 */
mnlxt_message_t *mnlxt_data_iterate(mnlxt_data_t *data, mnlxt_message_t *message);
/**
 * Removes mnlxt message from mnlxt data
 * @param data pointer to mnlxt data to remove from
 * @param message pointer to the mnlxt message to remove NULL for the first one
 * @return pointer to removed message or NULL for empty mnlxt data
 */
mnlxt_message_t *mnlxt_data_remove(mnlxt_data_t *data, mnlxt_message_t *message);
/**
 * Adds mnlxt message to mnlxt data
 * @param data pointer to mnlxt data to add a mnlxt message to
 * @param message pointer to the mnlxt message to add
 */
void mnlxt_data_add(mnlxt_data_t *data, mnlxt_message_t *message);
/**
 * Creates netlink message header for given mnlxt message
 * @param message pointer to mnlxt message to create netlink message header for
 * @return pointer to dynamic allocated nlmsghdr structure
 */
struct nlmsghdr *mnlxt_msghdr_create(const mnlxt_message_t *message);
/**
 * Frees memory allocated with netlink message header
 * @param msghdr pointer to netlink message header to free
 */
void mnlxt_msghdr_free(struct nlmsghdr *msghdr);
/**
 * Parses netlink message from mnlxt buffer into mnlxt messages and stores it into mnlxt data
 * @param data pointer to mnlxt data to store mnlxt message into
 * @param buffer pointer to mnlxt buffer to get message from
 * @return 0 on success, else -1
 */
int mnlxt_data_parse(mnlxt_data_t *data, mnlxt_buffer_t *buffer);
/**
 * Cleans mnlxt buffer's content
 * @param buffer pointer to mnlxt buffer to clean
 */
void mnlxt_buffer_clean(mnlxt_buffer_t *buffer);
/**
 * Cleans mnlxt data
 * @param data pointer to mnlxt data to clean
 */
void mnlxt_data_clean(mnlxt_data_t *data);
/**
 * Dumps netlink data for given netlink message
 * @param data pointer to mnlxt data to store the result into
 * @param bus bus type (NETLINK_* see linux/netlink.h)
 * @param nlh pointer to netlink message
 * @return 0 on success, else -1
 */
int mnlxt_data_dump(mnlxt_data_t *data, int bus, struct nlmsghdr *nlh);

#endif /* LIBMNLXT_DATA_H_ */
