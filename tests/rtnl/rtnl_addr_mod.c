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

int validate_ip_prefix(mnlxt_inet_addr_t *ipaddr, int *prefix, int *family, const char *ipstr, const char *prefixstr) {
	int rc = -1;
	struct in6_addr addr = {};
	int fam = 0, pref = 0;
	if (prefixstr) {
		char *endptr = NULL;
		pref = strtol(prefixstr, &endptr, 10);
		if (!endptr || '\0' != *endptr) {
			fprintf(stderr, "Invalid prefix: '%s'", prefixstr);
			goto end;
		}
	}
	int ret = inet_pton(AF_INET6, ipstr, &addr);
	if (1 == ret) {
		/* IPv6 address format */
		if (IN6_IS_ADDR_V4MAPPED(&addr)) {
			/* IPv4 address */
			fam = AF_INET;
			addr.__in6_u.__u6_addr32[0] = addr.__in6_u.__u6_addr32[3];
			if (!pref) {
				pref = 32;
			} else if (96 <= pref && 128 >= pref) {
				pref -= 96;
			} else {
				fprintf(stderr, "Invalid IPv4 in IPv6 mapped prefix: '%s'", prefixstr);
				goto end;
			}
		} else {
			/* IPV6 address */
			fam = AF_INET6;
			if (!pref) {
				pref = 128;
			} else if (0 > pref || 128 < pref) {
				fprintf(stderr, "Invalid IPv6 prefix: '%s'", prefixstr);
				goto end;
			}
		}
	} else if (0 == ret) {
		/* IPv4 address format */
		if (1 == inet_pton(AF_INET, ipstr, &addr)) {
			fam = AF_INET;
			if (!pref) {
				pref = 32;
			} else if (0 <= pref && 32 >= pref) {
				/* TODO: check IPv4-address/prefix combination */
			} else {
				fprintf(stderr, "Invalid IPv4 prefix: '%s'", prefixstr);
				goto end;
			}
		} else {
			/* invalid IP address format */
			fprintf(stderr, "Invalid IP address: '%s'", ipstr);
			goto end;
		}
	} else {
		/* invalid IP address format */
		fprintf(stderr, "Invalid IP address: '%s'", ipstr);
		goto end;
	}
	if (ipaddr) {
		if (AF_INET == fam && INADDR_ANY == addr.__in6_u.__u6_addr32[0]) {
			/* 0.0.0.0/0 */
			pref = 0;
		}
		memcpy(ipaddr, &addr, (AF_INET == fam ? sizeof(ipaddr->in) : sizeof(ipaddr->in6)));
	}
	if (family) {
		*family = fam;
	}
	if (prefix) {
		*prefix = pref;
	}
	rc = 0;
end:
	return rc;
}

int validate_ip(mnlxt_inet_addr_t *ipaddr, int *prefix, int *family, const char *ipstr) {
	int rc = -1;
	if (ipstr) {
		char *slash = strchr(ipstr, '/');
		if (slash) {
			size_t len = slash - ipstr;
			char buf[len + 1];
			memcpy(buf, ipstr, len);
			buf[len] = '\0';
			rc = validate_ip_prefix(ipaddr, prefix, family, buf, slash + 1);
		} else {
			rc = validate_ip_prefix(ipaddr, prefix, family, ipstr, NULL);
		}
	}
	return rc;
}

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
	message = mnlxt_rt_addr_message(&addr, (uint16_t)action);
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
	if (-1 == mnlxt_rt_addr_request(addr, (uint16_t)action)) {
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
