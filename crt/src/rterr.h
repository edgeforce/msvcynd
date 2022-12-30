/***
*rterr.h - runtime errors
*
*       Copyright (c) 1990-1997, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines the C runtime errors
*
*       [Internal]
*
****/

#if _MSC_VER > 1000
#pragma once
#endif  /* _MSC_VER > 1000 */

#ifndef _INC_RTERR
#define _INC_RTERR

#ifndef _CRTBLD
/*
 * This is an internal C runtime header file. It is used when building
 * the C runtimes only. It is not to be used as a public header file.
 */
#error ERROR: Use of C runtime library internal header file.
#endif  /* _CRTBLD */

#define _RT_STACK       0               /* stack overflow */
#define _RT_NULLPTR     1               /* null pointer assignment */
#define _RT_FLOAT       2               /* floating point not loaded */
#define _RT_INTDIV      3               /* integer divide by 0 */

/*
 * the following three errors must be in the given order!
 */
#define _RT_EXECMEM     5               /* not enough memory on exec */
#define _RT_EXECFORM    6               /* bad format on exec */
#define _RT_EXECENV     7               /* bad environment on exec */

#define _RT_SPACEARG    8               /* not enough space for arguments */
#define _RT_SPACEENV    9               /* not enough space for environment */
#define _RT_ABORT       10              /* Abnormal program termination */

#define _RT_NPTR        12              /* illegal near pointer use */
#define _RT_FPTR        13              /* illegal far pointer use */
#define _RT_BREAK       14              /* control-BREAK encountered */
#define _RT_INT         15              /* unexpected interrupt */
#define _RT_THREAD      16              /* not enough space for thread data */
#define _RT_LOCK        17              /* unexpected multi-thread lock error */
#define _RT_HEAP        18              /* unexpected heap error */
#define _RT_OPENCON     19              /* unable to open console device */

/*
 * _RT_QWIN and _RT_NOMAIN are used in 16-bit Windows support
 */
#define _RT_QWIN        20              /* unexpected QuickWin error */
#define _RT_NOMAIN      21              /* no main procedure */


#define _RT_NONCONT     22              /* non-continuable exception */
#define _RT_INVALDISP   23              /* invalid disposition of exception */


/*
 * _RT_ONEXIT is specific to Win32 and Dosx32 platforms
 */
#define _RT_ONEXIT      24              /* insufficient heap to allocate
                                         * initial table of funct. ptrs
                                         * used by _onexit()/atexit(). */

#define _RT_PUREVIRT    25              /* pure virtual function call attempted
                                         * (C++ error) */

#define _RT_STDIOINIT   26              /* not enough space for stdio initial-
                                         * ization */
#define _RT_LOWIOINIT   27              /* not enough space for lowio initial-
                                         * ization */
#define _RT_HEAPINIT    28              /* heap failed to initialize */

/*
 * _RT_DOMAIN, _RT_SING and _RT_TLOSS are generated by the floating point
 * library.
 */
#define _RT_DOMAIN      120
#define _RT_SING        121
#define _RT_TLOSS       122

#define _RT_CRNL        252
#define _RT_BANNER      255

#endif  /* _INC_RTERR */
