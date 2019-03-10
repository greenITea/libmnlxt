/*
 * libmnlxt/core.h		Libmnlxt Core
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_CORE_H_
#define LIBMNLXT_CORE_H_

#include <libmnl/libmnl.h>

/**
 * Function to free message payload
 * @param payload structure
 */
typedef void (*mnlxt_data_free_cb_t)(void *);

/**
 * Function to create Netlink message.
 * @param Netlink message header
 * @param message payload structure
 * @param message type
 * @return 0 on success, else -1
 */
typedef int (*mnlxt_data_put_cb_t)(struct nlmsghdr *, const void *, uint16_t);

typedef struct {
	/** symbolic name of message type */
	const char *name;

	/** function to parse Netlink message */
	mnl_cb_t parse;

	/** function to create Netlink message */
	mnlxt_data_put_cb_t put;

	/** function to free resources of message payload */
	mnlxt_data_free_cb_t free;

	/** message flags to append */
	uint16_t flags;
} mnlxt_data_cb_t;

typedef struct {
	int portid;
	char *buf;
	size_t len;
	int seq;
	const mnlxt_data_cb_t *data_handlers;
	size_t data_nhandlers;
} mnlxt_buffer_t;

typedef struct {
	struct mnl_socket *nl;
	uint32_t seq;
	const char *error_str;
	const mnlxt_data_cb_t *data_handlers;
	size_t data_nhandlers;
} mnlxt_handle_t;

/**
 * Connects to netlink socket and initializes mnlxt handle
 * @param handle pointer at mnlxt handle
 * @param bus netlink bus type (NETLINK_* see linux/netlink.h)
 * @param groups netlink multicast groups to subscribe
 * @return 0 on success, else -1
 */
int mnlxt_connect(mnlxt_handle_t *handle, int bus, int groups);
/**
 * Disconnects netlink socket and cleans mnlxt handle
 * @param handle pointer at mnlxt handle
 */
void mnlxt_disconnect(mnlxt_handle_t *handle);
/**
 * Sends a message via netlink
 * @param handle pointer at mnlxt handle
 * @param nlh netlink message header with embedded message to send
 * @return 0 on success, else -1
 */
int mnlxt_send(mnlxt_handle_t *handle, struct nlmsghdr *nlh);
/**
 * Receives netlink message
 * @param handle pointer at mnlxt handle
 * @param buffer pointer at buffer to save netlink message(s) into
 * @return 0 on success, else -1
 */
int mnlxt_receive(mnlxt_handle_t *handle, mnlxt_buffer_t *buffer);
/**
 * Gets netlink file descriptor
 * @param handle pointer at mnlxt handle
 * @return file descriptor on success, or -1
 */
int mnlxt_handel_get_fd(const mnlxt_handle_t *handle);

#endif /* LIBMNLXT_CORE_H_ */
