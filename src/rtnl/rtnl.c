#include "../../include/libmnlxt/rt.h"

int mnlxt_rt_connect(mnlxt_handle_t *handle, int group) {
	return mnlxt_connect(handle, NETLINK_ROUTE, group);
}
