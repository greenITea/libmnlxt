/*
 * rtnl_defaultroute_get.c		Libmnlxt Routing Address Test - Setting/Deleting Route
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

#include <libmnlxt/mnlxt.h>

#include "rtnl_common.h"

static void usage(const char *progname) {
	printf("usage: %s add|del <dst> via <gw> dev <ifname>\n"
				 "\t dst	IP address of routing destination or 'default'\n"
				 "\t gw		gateway\n"
				 "\t ifname		interface name\n",
				 progname);
}

int main(int argc, char **argv) {
	int rc = EXIT_FAILURE;
	int action = 0;
	const char *progname = argv[0];
	const char *gw = NULL;
	const char *dst = NULL;
	const char *ifname = NULL;
	mnlxt_inet_addr_t addr_buf = {};
	mnlxt_rt_route_t *rt_route = NULL;
	int prefix = 0, family = AF_INET, ifindex = 0;

	argc--, argv++;

	if (2 > argc) {
		usage(progname);
		goto err;
	}

	if (!strcmp("add", *argv)) {
		action = RTM_NEWROUTE;
	} else if (!strcmp("del", *argv)) {
		action = RTM_DELROUTE;
	} else {
		fprintf(stderr, "Invalid argument: %s\n", *argv);
		usage(progname);
		goto err;
	}
	argc--, argv++;

	dst = *argv;
	argc--, argv++;

	while (argc > 0) {
		if (!strcmp("via", *argv)) {
			if (argc < 1) {
				fprintf(stderr, "Missing gateway\n");
				usage(progname);
				goto err;
			} else {
				argc--, argv++;
				gw = *argv;
			}
		} else if (!strcmp("dev", *argv)) {
			if (argc < 1) {
				fprintf(stderr, "Missing network interface\n");
				usage(progname);
				goto err;
			} else {
				argc--, argv++;
				ifname = *argv;
			}
		}
		argc--, argv++;
	}

	if (NULL == (rt_route = mnlxt_rt_route_new())) {
		fprintf(stderr, "mnlxt_rt_route_new failed");
		goto err;
	}

	mnlxt_rt_route_set_table(rt_route, RT_TABLE_MAIN);
	mnlxt_rt_route_set_type(rt_route, RTN_UNICAST);

	if (!strcmp(dst, "default")) {
		/* default raute */
	} else if (0 != validate_ip(&addr_buf, &prefix, &family, dst)) {
		fprintf(stderr, "Invalid route destination: %s\n", dst);
		goto err;
	} else if (0 != mnlxt_rt_route_set_dst(rt_route, (uint8_t)family, &addr_buf)) {
		fprintf(stderr, "mnlxt_rt_route_set_dst failed for destination: %s\n", dst);
		goto err;
	} else if (0 != mnlxt_rt_route_set_dst_prefix(rt_route, prefix)) {
		fprintf(stderr, "mnlxt_rt_route_set_dst_prefix failed for destination: %s\n", dst);
		goto err;
	}

	if (NULL == ifname) {
		fprintf(stderr, "Missing interface name\n");
		usage(progname);
		goto err;
	} else if (0 == (ifindex = if_nametoindex(ifname))) {
		fprintf(stderr, "Invalid interface name: %s\n", ifname);
		goto err;
	} else if (0 != mnlxt_rt_route_set_oifindex(rt_route, ifindex)) {
		fprintf(stderr, "mnlxt_rt_route_set_oifindex failed for interface: %s\n", ifname);
		goto err;
	}

	if (NULL == gw) {
		mnlxt_rt_route_set_scope(rt_route, RT_SCOPE_LINK);
		mnlxt_rt_route_set_protocol(rt_route, RTPROT_BOOT);
		mnlxt_rt_route_set_family(rt_route, (uint8_t)family);
	} else if (0 != validate_ip(&addr_buf, NULL, &family, gw)) {
		fprintf(stderr, "Invalid route gateway: %s\n", dst);
		goto err;
	} else if (0 != mnlxt_rt_route_set_gateway(rt_route, (uint8_t)family, &addr_buf)) {
		fprintf(stderr, "mnlxt_rt_route_set_gateway failed for gateway: %s\n", gw);
		goto err;
	}

	mnlxt_rt_route_print(rt_route);

	if (0 != mnlxt_rt_route_request(rt_route, action, 0)) {
		fprintf(stderr, "mnlxt_rt_route_request failed, %m\n");
		goto err;
	}

err:
	mnlxt_rt_route_free(rt_route);
	return rc;
}
