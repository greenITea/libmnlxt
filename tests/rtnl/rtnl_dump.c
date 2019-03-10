/*
 * rtnl_dump.c		Libmnlxt Routing Address Test - Dump addresses, routes, rules and links information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

#include "../../include/libmnlxt/mnlxt.h"
#include "rtnl_common.h"

static int test_addr_dump() {
	printf("\nmnlxt_rt_addr_dump test\n");
	int rc = 1;
	mnlxt_data_t data = {};
	if (0 != mnlxt_rt_addr_dump(&data, AF_UNSPEC)) {
		printf("mnlxt_rt_addr_dump failed, %m\n");
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		}
	} else {
		mnlxt_message_t *it = NULL;
		mnlxt_rt_addr_t *addr = NULL;
		// printf("number of datasets: %d\n",);
		while ((addr = mnlxt_rt_addr_iterate(&data, &it))) {
			mnlxt_rt_addr_print(addr);
		}
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		} else {
			rc = 0;
		}
		mnlxt_data_clean(&data);
	}
	return rc;
}

static int test_route_dump() {
	printf("\nmnlxt_rt_route_dump test\n");
	int rc = -1;
	mnlxt_data_t data = {};
	if (0 != mnlxt_rt_route_dump(&data, AF_UNSPEC)) {
		printf("mnlxt_rt_route_dump failed, %m\n");
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		}
	} else {
		mnlxt_message_t *it = NULL;
		mnlxt_rt_route_t *route = NULL;
		// printf("number of datasets: %d\n", ?);
		while ((route = mnlxt_rt_route_iterate(&data, &it))) {
			mnlxt_rt_route_print(route);
		}
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		} else {
			rc = 0;
		}
		mnlxt_data_clean(&data);
	}
	return rc;
}

static int test_rule_dump() {
	printf("\nmnlxt_rt_rule_dump test\n");
	int rc = -1;
	mnlxt_data_t data = {};
	if (0 != mnlxt_rt_rule_dump(&data, AF_UNSPEC)) {
		printf("mnlxt_rt_rule_dump failed, %m\n");
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		}
	} else {
		mnlxt_message_t *it = NULL;
		mnlxt_rt_rule_t *rule = NULL;
		// printf("number of datasets: %d\n", ?);
		while ((rule = mnlxt_rt_rule_iterate(&data, &it))) {
			mnlxt_rt_rule_print(rule);
		}
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		} else {
			rc = 0;
		}
		mnlxt_data_clean(&data);
	}
	return rc;
}

static int test_link_dump() {
	printf("\nmnlxt_rt_link_dump test\n");
	int rc = -1;
	mnlxt_data_t data = {};
	if (0 != mnlxt_rt_link_dump(&data)) {
		printf("mnlxt_rt_link_dump failed, %m\n");
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		}
	} else {
		mnlxt_message_t *it = NULL;
		mnlxt_rt_link_t *link = NULL;
		// printf("number of datasets: %d\n", ?);
		while ((link = mnlxt_rt_link_iterate(&data, &it))) {
			mnlxt_rt_link_print(link);
		}
	}
	if (data.error_str) {
		printf("error: %s\n", data.error_str);
	} else {
		rc = 0;
	}
	mnlxt_data_clean(&data);
	return rc;
}

int main(int argc, char **argv) {
	int rc = 1, ret = 0;
	ret |= test_addr_dump();
	ret |= test_route_dump();
	ret |= test_rule_dump();
	ret |= test_link_dump();
	if (0 == ret) {
		rc = 0;
	}
	return rc;
}
