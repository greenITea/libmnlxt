/*
 * rtnl_listen.c		Libmnlxt Routing Test - listen for events
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libmnlxt/mnlxt.h>

#include "rtnl_common.h"

static int running = 1;

static void sigfunc(int sig) {
	running = 0;
}

int main(int argc, char **argv) {
	mnlxt_handle_t handle = {};
	mnlxt_buffer_t buffer;
	int rc = EXIT_FAILURE;
	mnlxt_message_t *msg;
	mnlxt_data_t data;
	struct pollfd pfd;
	int groups = 0;
	int r;

	do {
		signal(SIGINT, sigfunc);

		groups |= RTMGRP_LINK;
		// groups |= RTMGRP_NOTIFY;
		// groups |= RTMGRP_NEIGH;
		// groups |= RTMGRP_TC;

		groups |= RTMGRP_IPV4_IFADDR;
		// groups |= RTMGRP_IPV4_MROUTE;
		groups |= RTMGRP_IPV4_ROUTE;
		groups |= RTMGRP_IPV4_RULE;

		groups |= RTMGRP_IPV6_IFADDR;
		// groups |= RTMGRP_IPV6_MROUTE;
		groups |= RTMGRP_IPV6_ROUTE;
		groups |= RTMGRP_IPV6_IFINFO;

		// groups |= RTMGRP_IPV6_PREFIX;

		if (-1 == mnlxt_rt_connect(&handle, groups)) {
			perror("mnlxt_rt_connect");
			break;
		}

		pfd.fd = mnlxt_handel_get_fd(&handle);
		pfd.events = POLLIN;

		while (running) {
			r = poll(&pfd, 1, 1000);

			if (0 == r)
				continue;

			if (1 != r || !(pfd.revents & POLLIN))
				break;

			memset(&buffer, 0, sizeof(buffer));

			r = mnlxt_receive(&handle, &buffer);

			if (0 < r) {
				memset(&data, 0, sizeof(data));

				/* reset portid and seq otherwise we will miss ifaddr messages */
				buffer.portid = buffer.seq = 0;

				r = mnlxt_data_parse(&data, &buffer);

				if (0 == r) {
					msg = NULL;
					while (NULL != (msg = mnlxt_data_iterate(&data, msg))) {
						printf("====> received: %s(%hu)\n", mnlxt_message_type(msg), msg->nlmsg_type);

						mnlxt_rt_link_print(mnlxt_rt_link_get(msg));
						mnlxt_rt_addr_print(mnlxt_rt_addr_get(msg));
						mnlxt_rt_route_print(mnlxt_rt_route_get(msg));
						mnlxt_rt_rule_print(mnlxt_rt_rule_get(msg));
					}

					mnlxt_data_clean(&data);

				} else {
					perror("mnlxt_data_parse");
				}

				mnlxt_buffer_clean(&buffer);

			} else if (0 == r) {
				/* empy read, continue */

			} else {
				perror("mnlxt_receive");
				break;
			}
		}

		rc = EXIT_SUCCESS;
	} while (0);

	mnlxt_disconnect(&handle);

	return rc;
}
