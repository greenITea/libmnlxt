/*
 * libmnlxt/rt_link_vlan.h		Libmnlxt Routing Link, VLAN interfaces
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_RT_LINK_VLAN_H_
#define LIBMNLXT_RT_LINK_VLAN_H_

#include <libmnlxt/rt_link.h>

typedef enum {
	MNLXT_RT_LINK_VLAN_ID = 0
#define MNLXT_RT_LINK_VLAN_MAX MNLXT_RT_LINK_VLAN_ID + 1
} mnlxt_rt_link_vlan_data_t;

/**
 * Gets vlan id from link instance
 * @param rt_link pointer to link instance
 * @param id pointer to buffer to save vlan's id
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_vlan_id(const mnlxt_rt_link_t *rt_link, uint16_t *id);
/**
 * Sets vlan id on link instance
 * @param rt_link pointer to link instance
 * @param id vlan id
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_vlan_id(mnlxt_rt_link_t *rt_link, uint16_t id);

#endif /* LIBMNLXT_RT_LINK_VLAN_H_ */
