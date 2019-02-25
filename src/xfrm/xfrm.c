#include <errno.h>
#include <linux/xfrm.h>

#include "../../include/libmnlxt/xfrm.h"

int mnlxt_xfrm_connect(mnlxt_handle_t *handle, int group) {
	return mnlxt_connect(handle, NETLINK_XFRM, group);
}

int mnlxt_xfrm_data_dump(mnlxt_data_t *data, int type) {
	struct nlmsghdr *nlh;
	char buf[sizeof(struct nlmsghdr) + sizeof(struct xfrm_userpolicy_info) + 64];
	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = type;
	return mnlxt_data_dump(data, NETLINK_XFRM, nlh);
}

int mnlxt_xfrm_message_request(mnlxt_message_t *message) {
	int rc = -1;
	if (!message) {
		/* TODO: supported message types */
		errno = EINVAL;
	} else {
		rc = mnlxt_message_request(message, NETLINK_XFRM);
	}

	return rc;
}
