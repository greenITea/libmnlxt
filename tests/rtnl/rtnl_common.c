/*
 * rtnl_common.c		Libmnlxt Routing Test - common functions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#include <arpa/inet.h>
#include <linux/if_tun.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libmnlxt/rt.h>

void mnlxt_rt_link_print(mnlxt_rt_link_t *link) {
	uint32_t u32;
	uint16_t u16;
	uint8_t u8;
	int ret;

	if (NULL == link)
		return;

	printf("%s\n", "-- Link");

	ret = mnlxt_rt_link_get_index(link, &u32);
	if (0 == ret) {
		printf("if_index: %d\n", u32);
	} else {
		printf("missing if_index, %m\n");
		return;
	}

	mnlxt_if_name_t if_name;
	ret = mnlxt_rt_link_get_name(link, if_name);
	if (0 == ret) {
		printf("name: %s\n", if_name);
	} else {
		printf("missing name, %m\n");
		return;
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

	eth_addr_t hwaddr;
	ret = mnlxt_rt_link_get_hwaddr(link, &hwaddr);
	if (0 == ret) {
		printf("ifla_addr: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n", hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4],
					 hwaddr[5]);
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

	ret = mnlxt_rt_link_get_mtu_min(link, &u32);
	if (0 == ret) {
		printf("mtu (min): %d\n", u32);
	} else if (-1 == ret) {
		printf("mtu (min): error '%m'\n");
	} else {
		puts("mtu (min): unknown");
	}

	ret = mnlxt_rt_link_get_mtu_max(link, &u32);
	if (0 == ret) {
		printf("mtu (max): %d\n", u32);
	} else if (-1 == ret) {
		printf("mtu (min): error '%m'\n");
	} else {
		puts("mtu (max): unknown");
	}

	ret = mnlxt_rt_link_get_flags(link, &u32);
	if (0 == ret) {
		printf("flags/mask: 0x%x/0x%x\n", u32, link->flag_mask);
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
		} else if (MNLXT_RT_LINK_INFO_KIND_XFRM == info_kind) {
			ret = mnlxt_rt_link_get_xfrm_ifindex(link, &u32);
			if (0 == ret) {
				printf("xfrm ifindex: %d\n", u32);
			} else if (-1 == ret) {
				printf("error getting xfrm ifindex, %m\n");
			} else {
				printf("no xfrm ifindex \n");
			}
			ret = mnlxt_rt_link_get_xfrm_id(link, &u32);
			if (0 == ret) {
				printf("xfrm id: %d\n", u32);
			} else if (-1 == ret) {
				printf("error getting xfrm id, %m\n");
			} else {
				printf("no xfrm id \n");
			}
		} else if (MNLXT_RT_LINK_INFO_KIND_TUN == info_kind) {
			ret = mnlxt_rt_link_get_tun_type(link, &u8);
			if (0 == ret) {
				printf("mode: ");
				if (u8 & IFF_TUN) {
					printf("tun\n");
				} else if (u8 & IFF_TAP) {
					printf("tap\n");
				} else {
					printf("unknown\n");
				}
			}
			ret = mnlxt_rt_link_get_tun_flags(link, &u16);
			if (0 == ret) {
				printf("tun flags: 0x%X\n", u16);
				printf("mode: ");
			} else if (-1 == ret) {
				printf("error getting tun flags, %m\n");
			} else {
				printf("no tun flags\n");
			}
			ret = mnlxt_rt_link_get_tun_uid(link, &u32);
			if (0 == ret) {
				printf("tun user: %d\n", u32);
			}
			ret = mnlxt_rt_link_get_tun_gid(link, &u32);
			if (0 == ret) {
				printf("tun group: %d\n", u32);
			}
		}
	} else if (-1 == ret) {
		printf("error getting info kind, %m\n");
	} else {
		printf("no info \n");
	}
}

void mnlxt_rt_addr_print(mnlxt_rt_addr_t *addr) {
	uint32_t u32;
	uint8_t u8;
	int ret;

	if (NULL == addr)
		return;

	printf("%s\n", "-- Address");

	ret = mnlxt_rt_addr_get_ifindex(addr, &u32);
	if (0 == ret) {
		printf("if_index: %u\n", u32);
	} else {
		printf("missing if_index, %m\n");
		return;
	}

	ret = mnlxt_rt_addr_get_family(addr, &u8);
	if (0 == ret) {
		printf("af_family: %d\n", u8);
	} else {
		printf("missing af_family, %m\n");
		return;
	}

	int family = u8;
	char *ipver = "4";
	if (AF_INET6 == u8)
		ipver = "6";
	printf("ip version: %s\n", ipver);

	ret = mnlxt_rt_addr_get_prefixlen(addr, &u8);
	if (-1 == ret) {
		printf("mnlxt_rt_addr_get_prefixlen failed, %m\n");
		return;
	}
	uint8_t prefixlen = u8;

	ret = mnlxt_rt_addr_get_flags(addr, &u32);
	if (0 == ret) {
		printf("flags: %u\n", u32);
	} else if (-1 == ret) {
		printf("mnlxt_rt_addr_get_flags failed, %m\n");
	}

	ret = mnlxt_rt_addr_get_scope(addr, &u8);
	if (0 == ret) {
		printf("scope: %d\n", u8);
	} else if (-1 == ret) {
		printf("mnlxt_rt_addr_get_scope failed, %m\n");
	}

	const mnlxt_inet_addr_t *buf;
	char addr_str[INET6_ADDRSTRLEN];
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

	mnlxt_if_name_t label;
	ret = mnlxt_rt_addr_get_label(addr, label);
	if (0 == ret) {
		printf("label: %s\n", label);
	} else if (-1 == ret) {
		printf("mnlxt_rt_addr_get_label failed, %m\n");
	}

	ret = mnlxt_rt_addr_get_valid_lifetime(addr, &u32);
	if (0 == ret) {
		printf("valid_lft: %u\n", u32);
	} else if (-1 == ret) {
		printf("mnlxt_rt_addr_get_valid_lifetime failed, %m\n");
	}

	ret = mnlxt_rt_addr_get_preferred_lifetime(addr, &u32);
	if (0 == ret) {
		printf("preferred_lft: %u\n", u32);
	} else if (-1 == ret) {
		printf("mnlxt_rt_addr_get_preferred_lifetime failed, %m\n");
	}
}

void mnlxt_rt_route_print(mnlxt_rt_route_t *route) {
	uint32_t u32;
	uint8_t u8;
	int ret;

	if (NULL == route)
		return;

	printf("%s\n", "-- Route");

	ret = mnlxt_rt_route_get_oifindex(route, &u32);
	if (0 == ret) {
		printf("oif_index: %d\n", u32);
	} else {
		printf("missing if_index, %m\n");
		return;
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
	int family = u8;

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

	const mnlxt_inet_addr_t *buf;
	char addr_str[INET6_ADDRSTRLEN];
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

void mnlxt_rt_rule_print(mnlxt_rt_rule_t *rule) {
	uint32_t u32;
	uint8_t u8;
	int ret;

	if (NULL == rule)
		return;

	printf("%s\n", "-- Rule");

	ret = mnlxt_rt_rule_get_family(rule, &u8);
	if (0 == ret) {
		printf("af_family: %d\n", u8);
	} else {
		printf("missing af_family\n");
		return;
	}
	int family = u8;

	mnlxt_if_name_t str;
	ret = mnlxt_rt_rule_get_iif_name(rule, str);
	if (0 == ret) {
		printf("\niif: %s\n", str);
	} else if (-1 == ret) {
		printf("error getting iif, %m\n");
	}

	ret = mnlxt_rt_rule_get_oif_name(rule, str);
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

	const mnlxt_inet_addr_t *buf;
	char addr_str[INET6_ADDRSTRLEN];
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

static int validate_ip_prefix(mnlxt_inet_addr_t *ipaddr, int *prefix, int *family, const char *ipstr,
															const char *prefixstr) {
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
