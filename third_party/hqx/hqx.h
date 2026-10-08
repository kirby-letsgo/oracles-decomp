/*
 * Copyright (C) 2003 Maxim Stepin ( maxst@hiend3d.com )
 *
 * Copyright (C) 2010 Cameron Zemek ( grom@zeminvaders.net)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __HQX_H_
#define __HQX_H_

#include <stdint.h>

/* Local change: upstream picks __stdcall and __declspec(dllimport/dllexport) here, for building hqx
 * as a Windows DLL. This copy is compiled straight into the app, where dllimport on a definition is
 * an error in the MinGW cross-build, so both are empty. */
#define HQX_CALLCONV
#define HQX_API

#ifdef __cplusplus
extern "C" {
#endif

/* Local change: hqxInit is gone with the lookup table it filled (see common.h), and hq4x is not
 * vendored -- the app scales by 2 or 3. */
HQX_API void HQX_CALLCONV hq2x_32( const uint32_t * src, uint32_t * dest, int width, int height );
HQX_API void HQX_CALLCONV hq3x_32( const uint32_t * src, uint32_t * dest, int width, int height );

HQX_API void HQX_CALLCONV hq2x_32_rb( const uint32_t * src, uint32_t src_rowBytes, uint32_t * dest, uint32_t dest_rowBytes, int width, int height );
HQX_API void HQX_CALLCONV hq3x_32_rb( const uint32_t * src, uint32_t src_rowBytes, uint32_t * dest, uint32_t dest_rowBytes, int width, int height );

#ifdef __cplusplus
}
#endif

#endif
