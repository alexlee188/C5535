/*
 * $$$MODULE_NAME data_types.h
 *
 * $$$MODULE_DESC data_types.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

//#define FALSE ( 0 )
//#define TRUE  ( 1 )

#ifdef WIN32
#define restrict
#define Int40               Int64
#define Uint40              Uint64

typedef short               Int16;
typedef long                Int32;
typedef unsigned short      Uint16;
typedef unsigned long       Uint32;
typedef float               Float32;
typedef double              Float64;
//typedef float              Float64;
#if (_MSC_VER == 1200)
typedef __int64             Int64;
typedef unsigned __int64    Uint64;
#else
typedef long long           Int64;
typedef unsigned long long  Uint64;
#endif

#else

#ifdef GRACE /* C6x, C62-1, Tesla */
typedef short               Int16;
typedef int                 Int32;
typedef long                Int40;
typedef long long           Int64;
typedef unsigned short      Uint16;
typedef unsigned int        Uint32;
typedef unsigned long       Uint40;
typedef unsigned long long  Uint64;
typedef float               Float32;
typedef double              Float64;

#else

#ifdef __TMS320C55X__
#define Int64               Int40   /* no 64-bit integer on C55x */
#define Uint64              Uint40
#define Float32             Float64 /* no 64-bit float on C55x */

//typedef short               Int16;
//typedef long                Int32;
typedef long long           Int40;
//typedef unsigned short      Uint16;
//typedef unsigned long       Uint32;
typedef unsigned long long  Uint40;
typedef float               Float32;

#else /* Cortex-M3, Merlin */

#if defined (CM3) || defined (MERLIN)
#define Int40               Int64

typedef short               Int16;
typedef int                 Int32;
typedef long long           Int64;
typedef unsigned short      Uint16;
typedef unsigned int        Uint32;
typedef unsigned long long  Uint64;
typedef float               Float32;
typedef double              Float64;

#else
#define restrict
#define Int40               Int64

typedef short               Int16;
typedef int                 Int32;
typedef long long           Int64;
typedef unsigned short      Uint16;
typedef unsigned int        Uint32;
typedef unsigned long long  Uint64;
typedef float               Float32;
typedef double              Float64;

#endif /* CM3, Merlin */

#endif /* C55X */

#endif /* GRACE */

#endif /* WIN32 */

#endif /* __DATA_TYPES_H__ */
