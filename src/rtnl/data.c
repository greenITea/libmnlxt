#include <errno.h>
#include <linux/rtnetlink.h>

#include "../../include/libmnlxt/rt.h"

int mnlxt_rt_data_dump(mnlxt_data_t *data, int type, unsigned char family) {
	struct nlmsghdr *nlh;
	struct rtgenmsg *rtg;
	char buf[sizeof(struct nlmsghdr) + sizeof(struct rtgenmsg) + 64];
	nlh = mnl_nlmsg_put_header(buf);
	nlh->nlmsg_type = type;
	rtg = mnl_nlmsg_put_extra_header(nlh, sizeof(struct rtgenmsg));
	rtg->rtgen_family = family;
	return mnlxt_data_dump(data, NETLINK_ROUTE, nlh);
}

int mnlxt_rt_message_request(mnlxt_message_t *message) {
	int rc = -1;
	if (!message) {
		/* TODO: supported message types */
		errno = EINVAL;
	} else {
		rc = mnlxt_message_request(message, NETLINK_ROUTE);
	}

	return rc;
}
