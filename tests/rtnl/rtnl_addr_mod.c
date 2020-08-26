/*
 * rtnl_addr_mod.c		Libmnlxt Routing Address Test - Add/Del address
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <net/if.h>

#include <libmnlxt/mnlxt.h>

#include "rtnl_common.h"

static void usage(const char *progname) {
	printf("usage: %s add|del addr iface\n"
				 "\t addr	IP address with prefix length\n"
				 "\t iface	interface name\n",
				 progname);
}

int main(int argc, char **argv) {
	int family, prefix, if_index;
	int action = 0, ret;
	mnlxt_rt_addr_t *addr = NULL;
	mnlxt_message_t *message = NULL;
	mnlxt_inet_addr_t addr_buf = {};
	int rc = EXIT_FAILURE;

	if (4 != argc) {
		usage(argv[0]);
		goto err;
	}

	if (!strcmp("add", argv[1])) {
		action = RTM_NEWADDR;
	} else if (!strcmp("del", argv[1])) {
		action = RTM_DELADDR;
	} else {
		fprintf(stderr, "Invalid argument: %s\n", argv[1]);
		usage(argv[0]);
		goto err;
	}

	ret = validate_ip(&addr_buf, &prefix, &family, argv[2]);
	if (-1 == ret) {
		usage(argv[0]);
		goto err;
	}

	if_index = if_nametoindex(argv[3]);
	if (!if_index) {
		fprintf(stderr, "Invalid interface name: %s, %m\n", argv[3]);
		usage(argv[0]);
		goto err;
	}

	addr = mnlxt_rt_addr_new();
	if (!addr) {
		perror("mnlxt_rt_addr_new");
		goto err;
	}

	/* init address */
	mnlxt_rt_addr_set_addr(addr, (uint8_t)family, &addr_buf);
	mnlxt_rt_addr_set_ifindex(addr, (uint32_t)if_index);
	mnlxt_rt_addr_set_prefixlen(addr, (uint8_t)prefix);

#if 0 /* more control */
	message = mnlxt_rt_addr_message(&addr, (uint16_t)action, 0);
	if (!message) {
		perror("mnlxt_rt_addr_message");
		goto err;
	}

	ret = mnlxt_rt_message_request(message);
	if (-1 == ret) {
		perror("mnlxt_rt_message_request");
		goto err;
	}
#else /* simple way */
	if (-1 == mnlxt_rt_addr_request(addr, (uint16_t)action, 0)) {
		perror("mnlxt_rt_addr_request");
		goto err;
	}
#endif
	rc = EXIT_SUCCESS;
err:
	if (addr) {
		mnlxt_rt_addr_free(addr);
	}
	if (message) {
		mnlxt_message_free(message);
	}
	return rc;
}
