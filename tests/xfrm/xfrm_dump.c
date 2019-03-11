/*
 * xfrm_dump.c		Libmnlxt Xfrm/IPsec Test - dump policies
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

#include <libmnlxt/mnlxt.h>

static const char *xfrm_dir[] = {[XFRM_POLICY_IN] = "in", [XFRM_POLICY_OUT] = "out", [XFRM_POLICY_FWD] = "fwd"};

static const char *xfrm_action[] = {[XFRM_POLICY_ALLOW] = "bypass", [XFRM_POLICY_BLOCK] = "block"};

static int test_policy_dump() {
	printf("\nmnlxt_xfrm_policy_dump test\n");
	int rc = -1;
	mnlxt_data_t data = {};
	if (0 != mnlxt_xfrm_policy_dump(&data)) {
		printf("mnlxt_xfrm_policy_dump failed, %m\n");
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		}
	} else {
		mnlxt_message_t *it = NULL;
		mnlxt_xfrm_policy_t *policy = NULL;
		while ((policy = mnlxt_xfrm_policy_iterate(&data, &it))) {
			uint8_t u8;
			uint16_t u16;
			uint32_t u32;
			const mnlxt_inet_addr_t *buf = NULL;
			char str[INET6_ADDRSTRLEN] = {};
			int ret;
			printf("\n");
			ret = mnlxt_xfrm_policy_get_index(policy, &u32);
			if (0 == ret) {
				printf("index %d ", u32);
			}
			ret = mnlxt_xfrm_policy_get_dir(policy, &u8);
			if (0 == ret && XFRM_POLICY_MAX > u8) {
				printf("dir %s ", xfrm_dir[u8]);
			}
			ret = mnlxt_xfrm_policy_get_src_addr(policy, &u8, &buf);
			if (0 == ret) {
				inet_ntop(u8, buf, str, sizeof(str));
				printf("src %s", str);
			}
			ret = mnlxt_xfrm_policy_get_src_prefixlen(policy, &u8);
			if (0 == ret) {
				printf("/%d", u8);
			}
			ret = mnlxt_xfrm_policy_get_src_port(policy, &u16);
			if (0 == ret) {
				printf(":%d ", u16);
			} else {
				printf(" ");
			}
			ret = mnlxt_xfrm_policy_get_dst_addr(policy, &u8, &buf);
			if (0 == ret) {
				inet_ntop(u8, buf, str, sizeof(str));
				printf("dst %s", str);
			}
			ret = mnlxt_xfrm_policy_get_dst_prefixlen(policy, &u8);
			if (0 == ret) {
				printf("/%d", u8);
			}
			ret = mnlxt_xfrm_policy_get_dst_port(policy, &u16);
			if (0 == ret) {
				printf(":%d ", u16);
			} else {
				printf(" ");
			}
			ret = mnlxt_xfrm_policy_get_proto(policy, &u8);
			if (0 == ret) {
				printf("proto %d ", u8);
			}
			ret = mnlxt_xfrm_policy_get_action(policy, &u8);
			if (0 == ret) {
				if (sizeof(xfrm_action) / sizeof(char *) > u8) {
					printf("action %s ", xfrm_action[u8]);
				} else {
					printf("action %d ", u8);
				}
			}
			ret = mnlxt_xfrm_policy_get_priority(policy, &u32);
			if (0 == ret) {
				printf("priority %d ", u32);
			}
			/* TODO: templates */
			printf("\n");
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
	ret |= test_policy_dump();
	if (0 == ret) {
		rc = 0;
	}
	return rc;
}
