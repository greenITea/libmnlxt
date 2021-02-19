/*
 * libmnlxt/rt_link_xfrm.h		Libmnlxt Routing Link, XFRM interfaces
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_RT_LINK_XFRM_H_
#define LIBMNLXT_RT_LINK_XFRM_H_

#include <libmnlxt/rt_link.h>

typedef enum {
	MNLXT_RT_LINK_XFRM_IFINDEX = 0,
	MNLXT_RT_LINK_XFRM_ID
#define MNLXT_RT_LINK_XFRM_MAX MNLXT_RT_LINK_XFRM_ID + 1
} mnlxt_rt_link_xfrm_data_t;

/**
 * Gets underlying interface for XFRM link
 * @param rt_link pointer to link instance
 * @param if_index buffer pointer to save interface index
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_xfrm_ifindex(const mnlxt_rt_link_t *rt_link, uint32_t *if_index);
/**
 * Sets underlying interface for XFRM link
 * @param rt_link pointer to link instance
 * @param if_index index of underlying interface
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_xfrm_ifindex(mnlxt_rt_link_t *rt_link, uint32_t if_index);
/**
 * Gets interface ID for XFRM link
 * @param rt_link pointer to link instance
 * @param id buffer pointer to save interface ID
 * @return 0 on success, 1 on not set, else -1
 */
int mnlxt_rt_link_get_xfrm_id(const mnlxt_rt_link_t *rt_link, uint32_t *id);
/**
 * Sets interface ID for XFRM link
 * @param rt_link pointer to link instance
 * @param id interface ID
 * @return 0 on success, else -1
 */
int mnlxt_rt_link_set_xfrm_id(mnlxt_rt_link_t *rt_link, uint32_t id);

#endif /* LIBMNLXT_RT_LINK_XFRM_H_ */
