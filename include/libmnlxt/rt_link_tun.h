/*
 * libmnlxt/rt_link_tun.h		Libmnlxt Routing Link, TUN/TAP devices
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef LIBMNLXT_RT_LINK_TUN_H_
#define LIBMNLXT_RT_LINK_TUN_H_

#include <libmnlxt/rt_link.h>

typedef enum {
	MNLXT_RT_LINK_TUN_TYPE = 0,
	MNLXT_RT_LINK_TUN_FLAGS,
	MNLXT_RT_LINK_TUN_UID,
	MNLXT_RT_LINK_TUN_GID
#define MNLXT_RT_LINK_TUN_MAX MNLXT_RT_LINK_TUN_GID + 1
} mnlxt_rt_link_tun_data_t;

#define MNLXT_RT_LINK_TUN_FLAG_PI 0x1
#define MNLXT_RT_LINK_TUN_FLAG_VNET_HDR 0x2
#define MNLXT_RT_LINK_TUN_FLAG_PERSIST 0x4
#define MNLXT_RT_LINK_TUN_FLAG_MULTI_QUEUE 0x8

/**
 * Gets type/mode of device (TUN or TAP)
 * @param rt_link pointer to link instance
 * @param type pointer to store value
 * @return 0 on success, 1 on not set, -1 otherwise
 */
int mnlxt_rt_link_get_tun_type(const mnlxt_rt_link_t *rt_link, uint8_t *type);
/**
 * Sets type/mode for device (TUN or TAP)
 * @param rt_link pointer to link instance
 * @param type value to set
 * @return 0 on success, -1 otherwise
 */
int mnlxt_rt_link_set_tun_type(mnlxt_rt_link_t *rt_link, uint8_t type);
/**
 * Gets flags of TUN/TAP device
 * @param rt_link pointer to link instance
 * @param flags pointer to store value
 * @return 0 on success, 1 on not set, -1 otherwise
 */
int mnlxt_rt_link_get_tun_flags(const mnlxt_rt_link_t *rt_link, uint16_t *flags);
/**
 * Sets flags for TUN/TAP device
 * @param rt_link pointer to link instance
 * @param flags value to set
 * @return 0 on success, -1 otherwise
 */
int mnlxt_rt_link_set_tun_flags(mnlxt_rt_link_t *rt_link, uint16_t flags);
/**
 * Gets user ID for TUN/TAP device
 * @param rt_link pointer to link instance
 * @param uid pointer to store value
 * @return 0 on success, 1 on not set, -1 otherwise
 */
int mnlxt_rt_link_get_tun_uid(const mnlxt_rt_link_t *rt_link, uid_t *uid);
/**
 * Sets user ID on TUN/TAP device
 * @param rt_link pointer to link instance
 * @param uid value to set
 * @return 0 on success, -1 otherwise
 */
int mnlxt_rt_link_set_tun_uid(mnlxt_rt_link_t *rt_link, uid_t uid);
/**
 * Gets group ID of TUN/TAP device
 * @param rt_link pointer to link instance
 * @param gid pointer to store value
 * @return 0 on success, 1 on not set, -1 otherwise
 */
int mnlxt_rt_link_get_tun_gid(const mnlxt_rt_link_t *rt_link, gid_t *gid);
/**
 * Sets group ID on TUN/TAP device
 * @param rt_link pointer to link instance
 * @param gid value to set
 * @return 0 on success -1 otherwise
 */
int mnlxt_rt_link_set_tun_gid(mnlxt_rt_link_t *rt_link, gid_t gid);

/**
 * Creates a persistent TUN/TAP device
 * @param rt_link pointer to link instance
 * @return 0 on success, -1 otherwise
 */
int mnlxt_rt_link_tun_create(mnlxt_rt_link_t *rt_link);
/**
 * Deletes an existing TUN/TAP device
 * @param rt_link pointer to link instance
 * @return 0 on success, -1 otherwise
 */
int mnlxt_rt_link_tun_delete(mnlxt_rt_link_t *rt_link);

/**
 * Creates a TUN/TAP device
 * @param name device name
 * @param uid user ID
 * @param gid group ID
 * @param flags TUNSETIFF ifr flags, see linux/if_tun.h
 * @return -1 on error, otherwise file descriptor which controls the tun-device/queue.
 * Closing of the file descriptor, in case of not persistent device, causes disappearing
 *  of the tun-device (or a queue of a multiqueue device)
 */
int mnlxt_tun_create(const char *name, uid_t uid, gid_t gid, uint32_t flags);
/**
 * Deletes a TUN/TAP device
 * @param name device name
 * @param type device type (IFF_TUN or IFF_TAP, see linux/if_tun.h)
 * @return 0 on success, -1 otherwise
 */
int mnlxt_tun_delete(const char *name, uint8_t type);

#endif /* LIBMNLXT_RT_LINK_TUN_H_ */
