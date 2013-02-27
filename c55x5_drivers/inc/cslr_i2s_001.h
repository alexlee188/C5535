/*
 * $$$MODULE_NAME cslr_i2s_001.h
 *
 * $$$MODULE_DESC cslr_i2s_001.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/


#ifndef _CSLR__I2S_1_H_
#define _CSLR__I2S_1_H_

#include <cslr.h>

/* Minimum unit = 2 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint16 SCRL;
    volatile Uint16 SCRM;
    volatile Uint16 RSVD0[2];
    volatile Uint16 SRGRL;
    volatile Uint16 SRGRM;
    volatile Uint16 RSVD1[2];
    volatile Uint16 TRW0L;
    volatile Uint16 TRW0M;
    volatile Uint16 RSVD2[2];
    volatile Uint16 TRW1L;
    volatile Uint16 TRW1M;
    volatile Uint16 RSVD3[2];
    volatile Uint16 IRL;
    volatile Uint16 IRM;
    volatile Uint16 RSVD4[2];
    volatile Uint16 ICMRL;
    volatile Uint16 ICMRM;
    volatile Uint16 RSVD5[18];
    volatile Uint16 RRW0L;
    volatile Uint16 RRW0M;
    volatile Uint16 RSVD6[2];
    volatile Uint16 RRW1L;
    volatile Uint16 RRW1M;
} CSL_I2sDrvRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* SCRL */

#define CSL_I2S_SCRL_ENABLE_MASK (0x8000u)
#define CSL_I2S_SCRL_ENABLE_SHIFT (0x000Fu)
#define CSL_I2S_SCRL_ENABLE_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_MONO_MASK (0x1000u)
#define CSL_I2S_SCRL_MONO_SHIFT (0x000Cu)
#define CSL_I2S_SCRL_MONO_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_LOOPBACK_MASK (0x0800u)
#define CSL_I2S_SCRL_LOOPBACK_SHIFT (0x000Bu)
#define CSL_I2S_SCRL_LOOPBACK_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_FSPOL_MASK (0x0400u)
#define CSL_I2S_SCRL_FSPOL_SHIFT (0x000Au)
#define CSL_I2S_SCRL_FSPOL_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_CLOCKPOL_MASK (0x0200u)
#define CSL_I2S_SCRL_CLOCKPOL_SHIFT (0x0009u)
#define CSL_I2S_SCRL_CLOCKPOL_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_DATADELAY_MASK (0x0100u)
#define CSL_I2S_SCRL_DATADELAY_SHIFT (0x0008u)
#define CSL_I2S_SCRL_DATADELAY_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_PACK_MASK (0x0080u)
#define CSL_I2S_SCRL_PACK_SHIFT (0x0007u)
#define CSL_I2S_SCRL_PACK_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_SIGNEXT_MASK (0x0040u)
#define CSL_I2S_SCRL_SIGNEXT_SHIFT (0x0006u)
#define CSL_I2S_SCRL_SIGNEXT_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_WORDLENGTH_MASK (0x003Cu)
#define CSL_I2S_SCRL_WORDLENGTH_SHIFT (0x0002u)
#define CSL_I2S_SCRL_WORDLENGTH_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_MODE_MASK (0x0002u)
#define CSL_I2S_SCRL_MODE_SHIFT (0x0001u)
#define CSL_I2S_SCRL_MODE_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_FORMAT_MASK (0x0001u)
#define CSL_I2S_SCRL_FORMAT_SHIFT (0x0000u)
#define CSL_I2S_SCRL_FORMAT_RESETVAL (0x0000u)

#define CSL_I2S_SCRL_RESETVAL (0x0000u)

/* SCRM */


#define CSL_I2S_SCRM_RESETVAL (0x0000u)

/* SRGRL */


#define CSL_I2S_SRGRL_FSDIV_MASK (0x0038u)
#define CSL_I2S_SRGRL_FSDIV_SHIFT (0x0003u)
#define CSL_I2S_SRGRL_FSDIV_RESETVAL (0x0000u)

#define CSL_I2S_SRGRL_CLOCKDIV_MASK (0x0007u)
#define CSL_I2S_SRGRL_CLOCKDIV_SHIFT (0x0000u)
#define CSL_I2S_SRGRL_CLOCKDIV_RESETVAL (0x0000u)

#define CSL_I2S_SRGRL_RESETVAL (0x0000u)

/* SRGRM */



#define CSL_I2S_SRGRM_RESETVAL (0x0000u)

/* TRW0L */

#define CSL_I2S_TRW0L_DATA_MASK (0xFFFFu)
#define CSL_I2S_TRW0L_DATA_SHIFT (0x0000u)
#define CSL_I2S_TRW0L_DATA_RESETVAL (0x0000u)

#define CSL_I2S_TRW0L_RESETVAL (0x0000u)

/* TRW0M */

#define CSL_I2S_TRW0M_DATA_MASK (0xFFFFu)
#define CSL_I2S_TRW0M_DATA_SHIFT (0x0000u)
#define CSL_I2S_TRW0M_DATA_RESETVAL (0x0000u)

#define CSL_I2S_TRW0M_RESETVAL (0x0000u)

/* TRW1L */

#define CSL_I2S_TRW1L_DATA_MASK (0xFFFFu)
#define CSL_I2S_TRW1L_DATA_SHIFT (0x0000u)
#define CSL_I2S_TRW1L_DATA_RESETVAL (0x0000u)

#define CSL_I2S_TRW1L_RESETVAL (0x0000u)

/* TRW1M */

#define CSL_I2S_TRW1M_DATA_MASK (0xFFFFu)
#define CSL_I2S_TRW1M_DATA_SHIFT (0x0000u)
#define CSL_I2S_TRW1M_DATA_RESETVAL (0x0000u)

#define CSL_I2S_TRW1M_RESETVAL (0x0000u)

/* IRL */


#define CSL_I2S_IRL_XMIT1_MASK (0x0020u)
#define CSL_I2S_IRL_XMIT1_SHIFT (0x0005u)
#define CSL_I2S_IRL_XMIT1_RESETVAL (0x0000u)

#define CSL_I2S_IRL_XMIT0_MASK (0x0010u)
#define CSL_I2S_IRL_XMIT0_SHIFT (0x0004u)
#define CSL_I2S_IRL_XMIT0_RESETVAL (0x0000u)

#define CSL_I2S_IRL_RCV1_MASK (0x0008u)
#define CSL_I2S_IRL_RCV1_SHIFT (0x0003u)
#define CSL_I2S_IRL_RCV1_RESETVAL (0x0000u)

#define CSL_I2S_IRL_RCV0_MASK (0x0004u)
#define CSL_I2S_IRL_RCV0_SHIFT (0x0002u)
#define CSL_I2S_IRL_RCV0_RESETVAL (0x0000u)

#define CSL_I2S_IRL_FERR_MASK (0x0002u)
#define CSL_I2S_IRL_FERR_SHIFT (0x0001u)
#define CSL_I2S_IRL_FERR_RESETVAL (0x0000u)

#define CSL_I2S_IRL_O_U_MASK (0x0001u)
#define CSL_I2S_IRL_O_U_SHIFT (0x0000u)
#define CSL_I2S_IRL_O_U_RESETVAL (0x0000u)

#define CSL_I2S_IRL_RESETVAL (0x0000u)

/* IRM */


#define CSL_I2S_IRM_RESETVAL (0x0000u)

/* ICMRL */


#define CSL_I2S_ICMRL_XMIT1_MASK (0x0020u)
#define CSL_I2S_ICMRL_XMIT1_SHIFT (0x0005u)
#define CSL_I2S_ICMRL_XMIT1_RESETVAL (0x0000u)

#define CSL_I2S_ICMRL_XMIT0_MASK (0x0010u)
#define CSL_I2S_ICMRL_XMIT0_SHIFT (0x0004u)
#define CSL_I2S_ICMRL_XMIT0_RESETVAL (0x0000u)

#define CSL_I2S_ICMRL_RCV1_MASK (0x0008u)
#define CSL_I2S_ICMRL_RCV1_SHIFT (0x0003u)
#define CSL_I2S_ICMRL_RCV1_RESETVAL (0x0000u)

#define CSL_I2S_ICMRL_RCV0_MASK (0x0004u)
#define CSL_I2S_ICMRL_RCV0_SHIFT (0x0002u)
#define CSL_I2S_ICMRL_RCV0_RESETVAL (0x0000u)

#define CSL_I2S_ICMRL_FERR_MASK (0x0002u)
#define CSL_I2S_ICMRL_FERR_SHIFT (0x0001u)
#define CSL_I2S_ICMRL_FERR_RESETVAL (0x0000u)

#define CSL_I2S_ICMRL_O_U_MASK (0x0001u)
#define CSL_I2S_ICMRL_O_U_SHIFT (0x0000u)
#define CSL_I2S_ICMRL_O_U_RESETVAL (0x0000u)

#define CSL_I2S_ICMRL_RESETVAL (0x0000u)

/* ICMRM */


#define CSL_I2S_ICMRM_RESETVAL (0x0000u)

/* RRW0L */

#define CSL_I2S_RRW0L_DATA_MASK (0xFFFFu)
#define CSL_I2S_RRW0L_DATA_SHIFT (0x0000u)
#define CSL_I2S_RRW0L_DATA_RESETVAL (0x0000u)

#define CSL_I2S_RRW0L_RESETVAL (0x0000u)

/* RRW0M */

#define CSL_I2S_RRW0M_DATA_MASK (0xFFFFu)
#define CSL_I2S_RRW0M_DATA_SHIFT (0x0000u)
#define CSL_I2S_RRW0M_DATA_RESETVAL (0x0000u)

#define CSL_I2S_RRW0M_RESETVAL (0x0000u)

/* RRW1L */

#define CSL_I2S_RRW1L_DATA_MASK (0xFFFFu)
#define CSL_I2S_RRW1L_DATA_SHIFT (0x0000u)
#define CSL_I2S_RRW1L_DATA_RESETVAL (0x0000u)

#define CSL_I2S_RRW1L_RESETVAL (0x0000u)

/* RRW1M */

#define CSL_I2S_RRW1M_DATA_MASK (0xFFFFu)
#define CSL_I2S_RRW1M_DATA_SHIFT (0x0000u)
#define CSL_I2S_RRW1M_DATA_RESETVAL (0x0000u)

#define CSL_I2S_RRW1M_RESETVAL (0x0000u)

#endif
