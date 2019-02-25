#include <arpa/inet.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>

#include <libmnlxt/mnlxt.h>

int main(int argc, char **argv) {
	int if_index, family = AF_INET;
	mnlxt_rt_addr_t *filter = NULL;
	mnlxt_data_t data = {};
	int rc = EXIT_FAILURE;
	char *ifname = "eth0";
	if (argc == 2) {
		ifname = argv[1];
	}

	/* get interface index */
	if_index = if_nametoindex(ifname);
	if (!if_index) {
		perror("if_nametoindex");
		goto err;
	}

	filter = mnlxt_rt_addr_new();
	if (!filter) {
		perror("mnlxt_rt_addr_new");
		goto err;
	}

	/* init filter */
	mnlxt_rt_addr_set_family(filter, (uint8_t)family);
	mnlxt_rt_addr_set_ifindex(filter, (uint32_t)if_index);

	if (0 != mnlxt_rt_addr_dump(&data, family)) {
		printf("mnlxt_rt_addr_dump failed, %m\n");
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		}
	} else {
		mnlxt_rt_addr_t *addr = NULL;
		mnlxt_message_t *iterator = NULL;
		char addr_str[INET6_ADDRSTRLEN];
		while ((addr = mnlxt_rt_addr_iterate(&data, &iterator))) {
			if (0 == mnlxt_rt_addr_match(addr, filter)) {
				uint8_t pref;
				int ret = mnlxt_rt_addr_get_prefixlen(addr, &pref);
				if (-1 == ret) {
					printf("mnlxt_rt_addr_get_prefixlen failed, %m\n");
					continue;
				}
				inet_addr_t *buf = NULL;
				ret = mnlxt_rt_addr_get_addr(addr, NULL, &buf);
				if (0 == ret) {
					inet_ntop(family, buf, addr_str, sizeof(addr_str));
					printf("addr: %s/%d\n", addr_str, pref);
				} else if (-1 == ret) {
					printf("mnlxt_rt_addr_get_addr failed, %m\n");
				}
			}
		}
	}

	rc = EXIT_SUCCESS;
err:
	if (filter) {
		mnlxt_rt_addr_free(filter);
	}
	mnlxt_data_clean(&data);
	return rc;
}
