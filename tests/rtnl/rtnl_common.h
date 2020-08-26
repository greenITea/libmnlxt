/*
 * rtnl_common.h		Libmnlxt Routing Test - common functions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef TESTS_RTNL_RTNL_COMMON_H_
#define TESTS_RTNL_RTNL_COMMON_H_

void mnlxt_rt_link_print(mnlxt_rt_link_t *link);
void mnlxt_rt_addr_print(mnlxt_rt_addr_t *addr);
void mnlxt_rt_route_print(mnlxt_rt_route_t *route);
void mnlxt_rt_rule_print(mnlxt_rt_rule_t *rule);

int validate_ip(mnlxt_inet_addr_t *ipaddr, int *prefix, int *family, const char *ipstr);

#endif /* TESTS_RTNL_RTNL_COMMON_H_ */
