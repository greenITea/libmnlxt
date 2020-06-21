/*
 * internal.h		Libmnlxt Internal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef MNLXT_INTERNAL_H_
#define MNLXT_INTERNAL_H_

#include <stddef.h>

#include <libmnlxt/core.h>

#define MNLXT_SET_PROP_FLAG(p, bit) p->prop_flags |= MNLXT_FLAG(bit)
#define MNLXT_UNSET_PROP_FLAG(p, bit) p->prop_flags &= ~MNLXT_FLAG(bit)
#define MNLXT_GET_PROP_FLAG(p, bit) (p->prop_flags & MNLXT_FLAG(bit))

struct access_data {
	uint8_t offset;
	uint8_t size;
};

#define msizeof(type, member) sizeof(((type *)0)->member)
#define ad_init(type, member) \
	{ .offset = offsetof(type, member), .size = msizeof(type, member) }

#endif /* MNLXT_INTERNAL_H_ */
