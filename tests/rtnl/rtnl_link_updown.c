/*
 * rtnl_link_updown.c		Libmnlxt Routing Link Test - setting link to up or down
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libmnlxt/mnlxt.h>

void usage(const char *basename) {
	printf("Usage: %s <ifname> <up|down>\n", basename);
}

int main(int argc, char **argv) {
	int up = 0, rc = EXIT_FAILURE;
	mnlxt_message_t *message = NULL;
	mnlxt_rt_link_t *rt_link = NULL;

	char *progname = strrchr(argv[0], '/');
	if (progname) {
		++progname;
	} else {
		progname = argv[0];
	}

	if (argc != 3) {
		usage(progname);
		goto err;
	}
	if (0 == strcmp(argv[2], "up")) {
		up = 1;
	} else if (strcmp(argv[2], "down")) {
		usage(progname);
		goto err;
	}

	/* get interface index */
	unsigned int if_index = if_nametoindex(argv[1]);
	if (!if_index) {
		printf("failed to get index of %s, %m\n", argv[1]);
		goto err;
	}

	rt_link = mnlxt_rt_link_new();
	if (NULL == rt_link) {
		perror("mnlxt_rt_link_new");
		goto err;
	}

	mnlxt_rt_link_set_index(rt_link, if_index);
#if 0 /* the old way */
	mnlxt_rt_link_set_updown(rt_link, up);
#else /* universal approach */
	if (up) {
		mnlxt_rt_link_set_flags_on(rt_link, IFF_UP);
	} else {
		mnlxt_rt_link_set_flags_off(rt_link, IFF_UP);
	}
#endif
	message = mnlxt_rt_link_message(&rt_link, RTM_SETLINK, 0);
	if (NULL == message) {
		perror("mnlxt_rt_addr_message");
		goto err;
	}

	int ret = mnlxt_rt_message_request(message);
	if (-1 == ret) {
		perror("mnlxt_rt_message_request");
		goto err;
	}

	rc = EXIT_SUCCESS;
err:
	mnlxt_rt_link_free(rt_link);
	mnlxt_message_free(message);

	return rc;
}
