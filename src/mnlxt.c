#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/libmnlxt/core.h"

int mnlxt_connect(mnlxt_handle_t *handle, int bus, int groups) {
	int rc = -1;
	struct mnl_socket *nl = NULL;

	do {
		if (!handle) {
			errno = EINVAL;
			break;
		} else {
			handle->error_str = NULL;
		}

		memset(handle, 0, sizeof(mnlxt_handle_t));
		nl = mnl_socket_open(bus);
		if (nl == NULL) {
			handle->error_str = "open socket failed";
			break;
		}

		if (mnl_socket_bind(nl, groups, MNL_SOCKET_AUTOPID) < 0) {
			handle->error_str = "bind socket failed";
			break;
		}

		rc = 0;
	} while (0);

	if (rc) {
		if (nl) {
			mnl_socket_close(nl);
		}
	} else {
		handle->nl = nl;
		handle->seq = time(NULL);
	}
	return rc;
}

void mnlxt_disconnect(mnlxt_handle_t *handle) {
	if (handle) {
		if (handle->nl) {
			mnl_socket_close(handle->nl);
			handle->nl = NULL;
		}
	}
}

int mnlxt_send(mnlxt_handle_t *handle, struct nlmsghdr *nlh) {
	int rc = -1;
	nlh->nlmsg_seq = ++handle->seq;
	rc = mnl_socket_sendto(handle->nl, nlh, nlh->nlmsg_len);
	if (0 > rc) {
		handle->error_str = "send failed";
	}
	return rc;
}

int mnlxt_receive(mnlxt_handle_t *handle, mnlxt_buffer_t *buffer) {
	int rc = -1;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	if (!handle || !handle->nl || !buffer) {
		errno = EINVAL;
		goto end;
	}
	int len = 0;
	while (0 > (len = mnl_socket_recvfrom(handle->nl, buf, sizeof(buf)))) {
		if (EWOULDBLOCK == errno) {
			/* would block on non blocking socket */
			rc = 0;
			goto end;
		}
		if (EINTR != errno && EAGAIN != errno) {
			/* an error by receiving message */
			handle->error_str = "receive failed";
			goto end;
		}
	}

	if (0 < len) {
		buffer->buf = malloc(len);
		if (!buffer->buf) {
			handle->error_str = "malloc failed";
			goto end;
		}

		memcpy(buffer->buf, buf, len);
		buffer->len = len;
		buffer->portid = mnl_socket_get_portid(handle->nl);
		buffer->seq = handle->seq;
	}
	rc = len;
end:
	return rc;
}

int mnlxt_handel_get_fd(mnlxt_handle_t *handle) {
	int rc = -1;
	if (handle && handle->nl) {
		rc = mnl_socket_get_fd(handle->nl);
	}
	return rc;
}
