/*
 * rtnl_link_xfrm_mod.c		Libmnlxt Routing XFRM Link Test - Add/Del Link
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

#include "config.h"

#ifdef HAVE_IFLA_XFRM
static void usage(const char *progname) {
	printf("usage: %s add|del|set name dev id mtu\n"
				 "\t name	interface name\n"
				 "\t dev		name of underlying interface\n"
				 "\t id		inteface ID\n"
				 "\t mtu		interface mtu\n",
				 progname);
}

int main(int argc, char **argv) {
	int rc = EXIT_FAILURE;
	unsigned if_index = 0, id = 0;
	int action = 0;
	mnlxt_rt_link_t *rt_link = NULL;

	if (3 > argc) {
		usage(argv[0]);
		goto err;
	}

	if (!strcmp("add", argv[1])) {
		action = RTM_NEWLINK;
	} else if (!strcmp("del", argv[1])) {
		action = RTM_DELLINK;
	} else if (!strcmp("set", argv[1])) {
		action = RTM_SETLINK;
	} else {
		fprintf(stderr, "Invalid argument: %s\n", argv[1]);
		usage(argv[0]);
		goto err;
	}

	rt_link = mnlxt_rt_link_new();
	if (!rt_link) {
		perror("mnlxt_rt_link_new");
		goto err;
	}

	if (RTM_DELLINK == action || RTM_SETLINK == action) {
		if_index = if_nametoindex(argv[2]);
		if (!if_index) {
			fprintf(stderr, "Invalid interface name: %s, %m\n", argv[2]);
			usage(argv[0]);
			goto err;
		}
		if (-1 == mnlxt_rt_link_set_index(rt_link, if_index)) {
			fprintf(stderr, "Invalid interface name: %s, %m\n", argv[2]);
			goto err;
		}
	}
	if (RTM_NEWLINK == action || RTM_SETLINK == action) {
		if (5 > argc) {
			usage(argv[0]);
			goto err;
		}

		if_index = if_nametoindex(argv[3]);
		if (!if_index) {
			fprintf(stderr, "Invalid interface name: %s, %m\n", argv[3]);
			usage(argv[0]);
			goto err;
		}

		id = strtoul(argv[4], NULL, 10);
		mnlxt_rt_link_set_xfrm_id(rt_link, id);

		mnlxt_rt_link_set_xfrm_ifindex(rt_link, if_index);

		if (6 <= argc) {
			unsigned mtu = strtoul(argv[5], NULL, 10);
			mnlxt_rt_link_set_mtu(rt_link, mtu);
		}
	}
	if (RTM_NEWLINK == action) {
		if (-1 == mnlxt_rt_link_set_name(rt_link, argv[2])) {
			fprintf(stderr, "Invalid interface name: %s, %m\n", argv[2]);
			goto err;
		}
	}

	if (-1 == mnlxt_rt_link_request(rt_link, (uint16_t)action, 0)) {
		perror("mnlxt_rt_link_request");
		goto err;
	}

	rc = 0;
err:
	mnlxt_rt_link_free(rt_link);

	return rc;
}
#else

int main(int argc, char **argv) {
	fprintf(stderr, "No support for XFRM interfaces\n");
	return EXIT_SUCCESS;
}
#endif
