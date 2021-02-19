/*
 * rtnl_link_tun_mod.c		Libmnlxt Routing TUN/TAP Link Test - Add/Del Link
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

#include <linux/if_tun.h>
#include <net/if.h>

#include <libmnlxt/mnlxt.h>

#include "config.h"

static void usage(const char *progname) {
	printf("usage: %s add|del|set name type mtu\n"
				 "\t name	interface name\n"
				 "\t mode		interface mode: tap|tun\n"
				 "\t mtu		interface mtu\n",
				 progname);
}

int main(int argc, char **argv) {
	int rc = EXIT_FAILURE;
	unsigned if_index = 0;
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

	if (RTM_SETLINK == action) {
		if_index = if_nametoindex(argv[2]);
		if (0 == if_index) {
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
		if (5 <= argc) {
			unsigned mtu = strtoul(argv[4], NULL, 10);
			mnlxt_rt_link_set_mtu(rt_link, mtu);
		}
		mnlxt_rt_link_set_flags_on(rt_link, IFF_UP);
	}
	if (RTM_NEWLINK == action || RTM_DELLINK == action) {
		if (-1 == mnlxt_rt_link_set_name(rt_link, argv[2])) {
			fprintf(stderr, "Invalid interface name: %s, %m\n", argv[2]);
			goto err;
		}
		if (4 > argc) {
			usage(argv[0]);
			goto err;
		}

		if (!strcmp(argv[3], "tap")) {
			mnlxt_rt_link_set_tun_type(rt_link, IFF_TAP);
		} else if (!strcmp(argv[3], "tun")) {
			mnlxt_rt_link_set_tun_type(rt_link, IFF_TUN);
		} else {
			fprintf(stderr, "Invalid argument: %s\n", argv[3]);
			usage(argv[0]);
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
