/*
 * libmnlxt/mnlxt.h - a user space library to exchange (network) configuration via Netlink
 *
 * Copyright (C) 2019 Andrey Alexandrenko
 * Copyright (C) 2019 Daniel Kautz
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * A copy of the LGPL v2.1 can be found in the file "COPYING" in this distribution.
 */

#ifndef LIBMNLXT_MNLXT_H_
#define LIBMNLXT_MNLXT_H_

#include <libmnlxt/features.h>

#include <libmnlxt/core.h>
#include <libmnlxt/data.h>

#ifdef LIBMNLXT_WITH_RTM
#include <libmnlxt/rt.h>
#endif

#ifdef LIBMNLXT_WITH_XFRM
#include <libmnlxt/xfrm.h>
#endif

#endif /* LIBMNLXT_MNLXT_H_ */
