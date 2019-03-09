#ifndef INCLUDE_LIBMNLXT_CORE_H_
#define INCLUDE_LIBMNLXT_CORE_H_

#include <libmnl/libmnl.h>

typedef struct mnlxt_data_cb_s mnlxt_data_cb_t;

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
int mnlxt_handel_get_fd(mnlxt_handle_t *handle);

#endif /* INCLUDE_LIBMNLXT_CORE_H_ */
