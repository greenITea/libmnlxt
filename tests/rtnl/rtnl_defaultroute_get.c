/*
 * rtnl_defaultroute_get.c		Libmnlxt Routing Address Test - Default Route
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

#include <libmnlxt/mnlxt.h>

int main(int argc, char **argv) {
	int family = AF_INET;
	mnlxt_rt_route_t *filter = NULL;
	mnlxt_data_t data = {};
	int rc = EXIT_FAILURE;

	filter = mnlxt_rt_route_new();
	if (!filter) {
		perror("mnlxt_rt_route_new");
		goto err;
	}

	/* init filter */
	mnlxt_rt_route_set_family(filter, (uint8_t)family);
	mnlxt_rt_route_set_table(filter, RT_TABLE_MAIN);
	mnlxt_rt_route_set_type(filter, RTN_UNICAST);

	if (0 != mnlxt_rt_route_dump(&data, family)) {
		printf("mnlxt_rt_addr_dump failed, %m\n");
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		}
	} else {
		mnlxt_rt_route_t *route = NULL;
		mnlxt_message_t *iterator = NULL;
		char ifname[IFNAMSIZ];
		char addr_str[INET6_ADDRSTRLEN];
		while ((route = mnlxt_rt_route_iterate(&data, &iterator))) {
			inet_addr_t *buf = NULL;
			if (0 == mnlxt_rt_route_match(route, filter)) {
				int ret = mnlxt_rt_route_get_dst(route, NULL, &buf);
				if (1 != ret) {
					continue;
				}
				uint32_t if_index;
				ret = mnlxt_rt_route_get_oifindex(route, &if_index);
				if (0 != ret) {
					continue;
				}
				ret = mnlxt_rt_route_get_gateway(route, NULL, &buf);
				if (0 == ret) {
					inet_ntop(family, buf, addr_str, sizeof(addr_str));
					printf("route default via %s dev %s\n", addr_str, if_indextoname(if_index, ifname));
				}
			}
		}
	}

	rc = EXIT_SUCCESS;
err:
	if (filter) {
		mnlxt_rt_route_free(filter);
	}
	mnlxt_data_clean(&data);
	return rc;
}
