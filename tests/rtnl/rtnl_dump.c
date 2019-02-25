#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>

#include "../include/libmnlxt/mnlxt.h"

static int test_addr_dump() {
	printf("\nmnlxt_rt_addr_dump test\n");
	int rc = 1;
	unsigned char family = AF_UNSPEC, prefixlen;
	mnlxt_data_t data = {};
	char addr_str[INET6_ADDRSTRLEN];
	if (0 != mnlxt_rt_addr_dump(&data, family)) {
		printf("mnlxt_rt_addr_dump failed, %m\n");
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		}
	} else {
		mnlxt_message_t *it = NULL;
		mnlxt_rt_addr_t *addr = NULL;
		// printf("number of datasets: %d\n",);
		while ((addr = mnlxt_rt_addr_iterate(&data, &it))) {
			uint8_t u8;
			uint32_t u32;
			int ret;
			inet_addr_t *buf = NULL;
			ret = mnlxt_rt_addr_get_ifindex(addr, &u32);
			if (0 == ret) {
				printf("\nif_index: %d\n", u32);
			} else {
				printf("\nmissing if_index, %m\n");
				continue;
			}
			ret = mnlxt_rt_addr_get_family(addr, &u8);
			if (0 == ret) {
				printf("af_family: %d\n", u8);
			} else {
				printf("missing af_family, %m\n");
				continue;
			}
			family = u8;
			char *ipver = "4";
			if (AF_INET6 == u8)
				ipver = "6";
			printf("ip version: %s\n", ipver);

			ret = mnlxt_rt_addr_get_prefixlen(addr, &u8);
			if (-1 == ret) {
				printf("mnlxt_rt_addr_get_prefixlen failed, %m\n");
				continue;
			}
			prefixlen = u8;
			ret = mnlxt_rt_addr_get_scope(addr, &u8);
			if (0 == ret) {
				printf("scope: %d\n", u8);
			} else if (-1 == ret) {
				printf("mnlxt_rt_addr_get_scope failed, %m\n");
			}

			ret = mnlxt_rt_addr_get_local(addr, NULL, &buf);
			if (0 == ret) {
				inet_ntop(family, buf, addr_str, sizeof(addr_str));
				printf("local addr: %s/%d\n", addr_str, prefixlen);
			} else if (-1 == ret) {
				printf("mnlxt_rt_addr_get_local failed, %m\n");
			}

			ret = mnlxt_rt_addr_get_addr(addr, NULL, &buf);
			if (0 == ret) {
				inet_ntop(family, buf, addr_str, sizeof(addr_str));
				printf("addr: %s/%d\n", addr_str, prefixlen);
			} else if (-1 == ret) {
				printf("mnlxt_rt_addr_get_addr failed, %m\n");
			}

			char *label;
			ret = mnlxt_rt_addr_get_label(addr, &label);
			if (0 == ret) {
				printf("label: %s\n", label);
			} else if (-1 == ret) {
				printf("mnlxt_rt_addr_get_label failed, %m\n");
			}
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
	char addr_str[INET6_ADDRSTRLEN];
	unsigned char family = AF_UNSPEC;
	if (0 != mnlxt_rt_route_dump(&data, family)) {
		printf("mnlxt_rt_route_dump failed, %m\n");
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		}
	} else {
		mnlxt_message_t *it = NULL;
		mnlxt_rt_route_t *route = NULL;
		// printf("number of datasets: %d\n", ?);
		while ((route = mnlxt_rt_route_iterate(&data, &it))) {
			uint8_t u8;
			uint32_t u32;
			inet_addr_t *buf = NULL;
			int ret;
			ret = mnlxt_rt_route_get_oifindex(route, &u32);
			if (0 == ret) {
				printf("\noif_index: %d\n", u32);
			} else {
				printf("\nmissing if_index, %m\n");
				continue;
			}
			ret = mnlxt_rt_route_get_iifindex(route, &u32);
			if (0 == ret) {
				printf("\niif_index: %d\n", u32);
			} else if (-1 == ret) {
				printf("error getting iif_index, %m\n");
			} else {
				printf("no iif_index\n");
			}
			ret = mnlxt_rt_route_get_family(route, &u8);
			if (0 == ret) {
				printf("af_family: %d\n", u8);
			} else {
				printf("missing af_family\n");
			}
			family = u8;
			ret = mnlxt_rt_route_get_protocol(route, &u8);
			if (0 == ret) {
				printf("protocol: %d\n", u8);
			} else if (-1 == ret) {
				printf("error getting protocol, %m\n");
			} else {
				printf("no protocol \n");
			}
			ret = mnlxt_rt_route_get_priority(route, &u32);
			if (0 == ret) {
				printf("priority: %d\n", u32);
			} else if (-1 == ret) {
				printf("error getting priority, %m\n");
			} else {
				printf("no priority \n");
			}
			ret = mnlxt_rt_route_get_table(route, &u8);
			if (0 == ret) {
				printf("table: %d\n", u8);
			} else if (-1 == ret) {
				printf("error getting table, %m\n");
			} else {
				printf("no table \n");
			}
			ret = mnlxt_rt_route_get_scope(route, &u8);
			if (0 == ret) {
				printf("scope: %d\n", u8);
			} else if (-1 == ret) {
				printf("error getting scope, %m\n");
			} else {
				printf("no scope \n");
			}
			ret = mnlxt_rt_route_get_type(route, &u8);
			if (0 == ret) {
				printf("type: %d\n", u8);
			} else if (-1 == ret) {
				printf("error getting type, %m\n");
			} else {
				printf("no type \n");
			}
			ret = mnlxt_rt_route_get_src(route, NULL, &buf);
			if (0 == ret) {
				inet_ntop(family, buf, addr_str, sizeof(addr_str));
				printf("src: %s", addr_str);
				ret = mnlxt_rt_route_get_src_prefix(route, &u8);
				if (0 == ret) {
					printf("/%u\n", u8);
				} else {
					printf("\n");
				}
			} else if (-1 == ret) {
				printf("error getting src, %m\n");
			} else {
				printf("no src \n");
			}
			ret = mnlxt_rt_route_get_dst(route, NULL, &buf);
			if (0 == ret) {
				inet_ntop(family, buf, addr_str, sizeof(addr_str));
				printf("dst: %s", addr_str);
				ret = mnlxt_rt_route_get_dst_prefix(route, &u8);
				if (0 == ret) {
					printf("/%u\n", u8);
				} else {
					printf("\n");
				}
			} else if (-1 == ret) {
				printf("error getting dst, %m\n");
			} else {
				printf("no dst \n");
			}
			ret = mnlxt_rt_route_get_gateway(route, NULL, &buf);
			if (0 == ret) {
				inet_ntop(family, buf, addr_str, sizeof(addr_str));
				printf("gateway: %s\n", addr_str);
			} else if (-1 == ret) {
				printf("error getting gateway, %m\n");
			} else {
				printf("no gateway \n");
			}
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
	char addr_str[INET6_ADDRSTRLEN];
	unsigned char family = AF_UNSPEC /* AF_INET */;
	if (0 != mnlxt_rt_rule_dump(&data, family)) {
		printf("mnlxt_rt_rule_dump failed, %m\n");
		if (data.error_str) {
			printf("error: %s\n", data.error_str);
		}
	} else {
		mnlxt_message_t *it = NULL;
		mnlxt_rt_rule_t *rule = NULL;
		// printf("number of datasets: %d\n", ?);
		while ((rule = mnlxt_rt_rule_iterate(&data, &it))) {
			uint8_t u8;
			uint32_t u32;
			inet_addr_t *buf = NULL;
			char *str = NULL;
			int ret;
			printf("\n");
			ret = mnlxt_rt_rule_get_family(rule, &u8);
			if (0 == ret) {
				printf("af_family: %d\n", u8);
			} else {
				printf("missing af_family\n");
				continue;
			}
			family = u8;
			ret = mnlxt_rt_rule_get_iif_name(rule, &str);
			if (0 == ret) {
				printf("\niif: %s\n", str);
			} else if (-1 == ret) {
				printf("error getting iif, %m\n");
			}
			ret = mnlxt_rt_rule_get_oif_name(rule, &str);
			if (0 == ret) {
				printf("\noif: %s\n", str);
			} else if (-1 == ret) {
				printf("error getting iif, %m\n");
			}
			ret = mnlxt_rt_rule_get_priority(rule, &u32);
			if (0 == ret) {
				printf("priority: %d\n", u32);
			} else if (-1 == ret) {
				printf("error getting priority, %m\n");
			}
			ret = mnlxt_rt_rule_get_table(rule, &u8);
			if (0 == ret) {
				printf("table: %d\n", u8);
			} else if (-1 == ret) {
				printf("error getting table, %m\n");
			}
			ret = mnlxt_rt_rule_get_tos(rule, &u8);
			if (0 == ret) {
				printf("tos: 0x%x\n", u8);
			} else if (-1 == ret) {
				printf("error getting tos, %m\n");
			}
			ret = mnlxt_rt_rule_get_action(rule, &u8);
			if (0 == ret) {
				printf("action: %d\n", u8);
			} else if (-1 == ret) {
				printf("error getting action, %m\n");
			}
			ret = mnlxt_rt_rule_get_src(rule, NULL, &buf);
			if (0 == ret) {
				inet_ntop(family, buf, addr_str, sizeof(addr_str));
				printf("src: %s", addr_str);
				ret = mnlxt_rt_rule_get_src_prefix(rule, &u8);
				if (0 == ret) {
					printf("/%u\n", u8);
				} else {
					printf("\n");
				}
			} else if (-1 == ret) {
				printf("error getting src, %m\n");
			} else {
				printf("no src \n");
			}
			ret = mnlxt_rt_rule_get_dst(rule, NULL, &buf);
			if (0 == ret) {
				inet_ntop(family, buf, addr_str, sizeof(addr_str));
				printf("dst: %s", addr_str);
				ret = mnlxt_rt_rule_get_dst_prefix(rule, &u8);
				if (0 == ret) {
					printf("/%u\n", u8);
				} else {
					printf("\n");
				}
			} else if (-1 == ret) {
				printf("error getting dst, %m\n");
			} else {
				printf("no dst \n");
			}
			ret = mnlxt_rt_rule_get_fwmark(rule, &u32);
			if (0 == ret) {
				printf("fwmark: 0x%x\n", u32);
			} else if (-1 == ret) {
				printf("error getting fwmark, %m\n");
			}
			ret = mnlxt_rt_rule_get_fwmask(rule, &u32);
			if (0 == ret) {
				printf("fwmask: 0x%x\n", u32);
			} else if (-1 == ret) {
				printf("error getting fwmask, %m\n");
			}
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
	eth_addr_t hwaddr;
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
			uint8_t u8;
			uint16_t u16;
			uint32_t u32;
			inet_addr_t *buf = NULL;
			const char *str = NULL;
			int ret;
			ret = mnlxt_rt_link_get_index(link, &u32);
			if (0 == ret) {
				printf("\nif_index: %d\n", u32);
			} else {
				printf("\nmissing if_index, %m\n");
				continue;
			}
			ret = mnlxt_rt_link_get_name(link, &str);
			if (0 == ret) {
				printf("name: %s\n", str);
			} else {
				printf("missing name, %m\n");
				continue;
			}
			ret = mnlxt_rt_link_get_master(link, &u32);
			if (0 == ret) {
				printf("master: %d\n", u32);
			} else if (-1 == ret) {
				printf("error getting master, %m\n");
			} else {
				printf("no master \n");
			}
			ret = mnlxt_rt_link_get_type(link, &u16);
			if (0 == ret) {
				printf("type: %d\n", u16);
			} else if (-1 == ret) {
				printf("error getting type, %m\n");
			} else {
				printf("no type \n");
			}
			ret = mnlxt_rt_link_get_family(link, &u8);
			if (0 == ret) {
				printf("family: %d\n", u8);
			} else if (-1 == ret) {
				printf("error getting family, %m\n");
			} else {
				printf("no family \n");
			}
			ret = mnlxt_rt_link_get_hwaddr(link, &hwaddr);
			if (0 == ret) {
				printf("ifla_addr: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3],
							 hwaddr[4], hwaddr[5]);
			} else if (-1 == ret) {
				printf("error getting hwaddr, %m\n");
			} else {
				printf("no hwaddr \n");
			}
			ret = mnlxt_rt_link_get_mtu(link, &u32);
			if (0 == ret) {
				printf("mtu: %d\n", u32);
			} else if (-1 == ret) {
				printf("error getting mtu, %m\n");
			} else {
				printf("no mtu \n");
			}
			ret = mnlxt_rt_link_get_flags(link, &u32);
			if (0 == ret) {
				printf("flags: %d\n", u32);
			} else if (-1 == ret) {
				printf("error getting flags, %m\n");
			} else {
				printf("no flags \n");
			}
			ret = mnlxt_rt_link_get_state(link, &u8);
			if (0 == ret) {
				printf("state: %d\n", u8);
			} else if (-1 == ret) {
				printf("error getting state, %m\n");
			} else {
				printf("no state \n");
			}
			ret = mnlxt_rt_link_get_parent(link, &u32);
			if (0 == ret) {
				printf("parent: %d\n", u32);
			} else if (-1 == ret) {
				printf("error getting parent, %m\n");
			} else {
				printf("no parent \n");
			}
			mnlxt_rt_link_info_kind_t info_kind;
			ret = mnlxt_rt_link_get_info_kind(link, &info_kind);
			if (0 == ret) {
				printf("kind: %d\n", (int)info_kind);
				if (MNLXT_RT_LINK_INFO_KIND_VLAN == info_kind) {
					ret = mnlxt_rt_link_get_vlan_id(link, &u16);
					if (0 == ret) {
						printf("vlan id: %d\n", u16);
					} else if (-1 == ret) {
						printf("error getting vlan id, %m\n");
					} else {
						printf("no vlan id \n");
					}
				}
			} else if (-1 == ret) {
				printf("error getting info kind, %m\n");
			} else {
				printf("no info \n");
			}
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
