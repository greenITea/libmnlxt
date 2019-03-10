/*
 * xfrm_policy_mod.c		Libmnlxt Xfrm/IPsec Test - add/del xfrm policy
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/libmnlxt/mnlxt.h"

static void usage(const char *progname) {
	printf("usage: %s add|del\n", progname);
}

int main(int argc, char **argv) {
	int family = AF_INET;
	int job = 0, ret;
	mnlxt_xfrm_policy_t *policy = NULL;
	mnlxt_message_t *message = NULL;
	inet_addr_t addr_buf = {};
	int rc = EXIT_FAILURE;

	if (2 != argc) {
		usage(argv[0]);
		goto err;
	}

	if (!strcmp("add", argv[1])) {
		job = XFRM_MSG_NEWPOLICY;
	} else if (!strcmp("del", argv[1])) {
		job = XFRM_MSG_DELPOLICY;
	} else {
		fprintf(stderr, "Invalid argument: %s\n", argv[1]);
		usage(argv[0]);
		goto err;
	}

	/*ip xfrm policy add dir fwd src 192.168.100.1/32 dst 192.168.200.1/32 proto tcp sport 1234 action bypass priority
	 * 10*/

	policy = mnlxt_xfrm_policy_new();
	if (!policy) {
		perror("mnlxt_xfrm_policy_new");
		goto err;
	}

	/* init policy */
	inet_pton(family, "192.168.100.1", &addr_buf);
	mnlxt_xfrm_policy_set_src_addr(policy, (uint8_t)family, &addr_buf);
	mnlxt_xfrm_policy_set_src_prefixlen(policy, 32);
	mnlxt_xfrm_policy_set_src_port(policy, 1234);

	inet_pton(family, "192.168.200.1", &addr_buf);
	mnlxt_xfrm_policy_set_dst_addr(policy, (uint8_t)family, &addr_buf);
	mnlxt_xfrm_policy_set_dst_prefixlen(policy, 32);

	mnlxt_xfrm_policy_set_proto(policy, IPPROTO_TCP);

	mnlxt_xfrm_policy_set_action(policy, XFRM_POLICY_ALLOW);

	mnlxt_xfrm_policy_set_priority(policy, 10);

	mnlxt_xfrm_policy_set_dir(policy, XFRM_POLICY_FWD);

	message = mnlxt_xfrm_policy_message(&policy, (uint16_t)job);
	if (!message) {
		perror("mnlxt_xfrm_policy_message");
		goto err;
	}

	ret = mnlxt_xfrm_message_request(message);
	if (-1 == ret) {
		perror("mnlxt_xfrm_message_request");
		goto err;
	}

	rc = EXIT_SUCCESS;
err:
	if (policy) {
		mnlxt_xfrm_policy_free(policy);
	}
	if (message) {
		mnlxt_message_free(message);
	}
	return rc;
}
