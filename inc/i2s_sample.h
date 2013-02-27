/*
 * $$$MODULE_NAME i2s_sample.h
 *
 * $$$MODULE_DESC i2s_sample.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _I2S_SAMPLE_H_
#define _I2S_SAMPLE_H_

#include "csl_intc.h"
#include "app_globals.h"
#include "ddc_i2s.h"
#include "psp_i2s.h"

/* Playback I2S selection */
//#define USE_I2S0_PB /* define to use I2S0 */
//#define USE_I2S1_PB /* define to use I2S1 */
#define USE_I2S2_PB /* define to use I2S2 */
//#define USE_I2S3_PB /* define to use I2S3 */

/* Record I2S selection */
//#define USE_I2S0_REC /* define to use I2S0 */
//#define USE_I2S1_REC /* define to use I2S1 */
#define USE_I2S2_REC /* define to use I2S2 */
//#define USE_I2S3_REC /* define to use I2S3 */

#ifdef USE_TWO_CODEC
/* 2nd Playback I2S selection */
//#define USE_I2S0_PB_2 /* define to use I2S0 */
#define USE_I2S1_PB_2 /* define to use I2S1 */
//#define USE_I2S2_PB_2 /* define to use I2S2 */
//#define USE_I2S3_PB_2 /* define to use I2S3 */

/* 2nd Record I2S selection */
//#define USE_I2S0_REC_2 /* define to use I2S0 */
#define USE_I2S1_REC_2 /* define to use I2S1 */
//define USE_I2S2_REC_2 /* define to use I2S2 */
//#define USE_I2S3_REC_2 /* define to use I2S3 */
#endif // USE_TWO_CODEC

#ifdef USE_THREE_CODEC
/* 2nd Playback I2S selection */
//#define USE_I2S0_PB_2 /* define to use I2S0 */
#define USE_I2S1_PB_2 /* define to use I2S1 */
//#define USE_I2S2_PB_2 /* define to use I2S2 */
//#define USE_I2S3_PB_2 /* define to use I2S3 */

/* 2nd Record I2S selection */
//#define USE_I2S0_REC_2 /* define to use I2S0 */
#define USE_I2S1_REC_2 /* define to use I2S1 */
//define USE_I2S2_REC_2 /* define to use I2S2 */
//#define USE_I2S3_REC_2 /* define to use I2S3 */

/* 3rd Playback I2S selection */
//#define USE_I2S0_PB_3 /* define to use I2S0 */
//#define USE_I2S1_PB_3 /* define to use I2S1 */
//#define USE_I2S2_PB_3 /* define to use I2S2 */
#define USE_I2S3_PB_3 /* define to use I2S3 */

/* 3rd Record I2S selection */
//#define USE_I2S0_REC_3 /* define to use I2S0 */
//#define USE_I2S1_REC_3 /* define to use I2S1 */
//#define USE_I2S2_REC_3 /* define to use I2S2 */
#define USE_I2S3_REC_3 /* define to use I2S3 */
#endif // USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
/* 2nd Playback I2S selection */
//#define USE_I2S0_PB_2 /* define to use I2S0 */
#define USE_I2S1_PB_2 /* define to use I2S1 */
//#define USE_I2S2_PB_2 /* define to use I2S2 */
//#define USE_I2S3_PB_2 /* define to use I2S3 */

/* 2nd Record I2S selection */
//#define USE_I2S0_REC_2 /* define to use I2S0 */
#define USE_I2S1_REC_2 /* define to use I2S1 */
//define USE_I2S2_REC_2 /* define to use I2S2 */
//#define USE_I2S3_REC_2 /* define to use I2S3 */

/* 3rd Playback I2S selection */
//#define USE_I2S0_PB_3 /* define to use I2S0 */
//#define USE_I2S1_PB_3 /* define to use I2S1 */
//#define USE_I2S2_PB_3 /* define to use I2S2 */
#define USE_I2S3_PB_3 /* define to use I2S3 */

/* 3rd Record I2S selection */
//#define USE_I2S0_REC_3 /* define to use I2S0 */
//#define USE_I2S1_REC_3 /* define to use I2S1 */
//#define USE_I2S2_REC_3 /* define to use I2S2 */
#define USE_I2S3_REC_3 /* define to use I2S3 */

/* 4th Playback I2S selection */
#define USE_I2S0_PB_4 /* define to use I2S0 */
//#define USE_I2S1_PB_4 /* define to use I2S1 */
//#define USE_I2S2_PB_4 /* define to use I2S2 */
//#define USE_I2S3_PB_4 /* define to use I2S3 */

/* 4th Record I2S selection */
#define USE_I2S0_REC_4 /* define to use I2S0 */
//#define USE_I2S1_REC_4 /* define to use I2S1 */
//#define USE_I2S2_REC_4 /* define to use I2S2 */
//#define USE_I2S3_REC_4 /* define to use I2S3 */
#endif // USE_FOUR_CODEC

/* I2S registers */
#define I2S0_I2STXLT0               ( 0x2808 )  /* I2S0 Transmit Left Data 0 Register */
#define I2S0_I2STXLT1               ( 0x2809 )  /* I2S0 Transmit Left Data 1 Register */
#define I2S0_I2STXRT0               ( 0x280C )  /* I2S0 Transmit Right Data 0 Register */
#define I2S0_I2STXRT1               ( 0x280D )  /* I2S0 Transmit Right Data 1 Register */
#define I2S1_I2STXLT0               ( 0x2908 )  /* I2S1 Transmit Left Data 0 Register */
#define I2S1_I2STXLT1               ( 0x2909 )  /* I2S1 Transmit Left Data 1 Register */
#define I2S1_I2STXRT0               ( 0x290C )  /* I2S1 Transmit Right Data 0 Register */
#define I2S1_I2STXRT1               ( 0x290D )  /* I2S1 Transmit Right Data 1 Register */
#define I2S2_I2STXLT0               ( 0x2A08 )  /* I2S2 Transmit Left Data 0 Register */
#define I2S2_I2STXLT1               ( 0x2A09 )  /* I2S2 Transmit Left Data 1 Register */
#define I2S2_I2STXRT0               ( 0x2A0C )  /* I2S2 Transmit Right Data 0 Register */
#define I2S2_I2STXRT1               ( 0x2A0D )  /* I2S2 Transmit Right Data 1 Register */
#define I2S3_I2STXLT0               ( 0x2B08 )  /* I2S3 Transmit Left Data 0 Register */
#define I2S3_I2STXLT1               ( 0x2B09 )  /* I2S3 Transmit Left Data 1 Register */
#define I2S3_I2STXRT0               ( 0x2B0C )  /* I2S3 Transmit Right Data 0 Register */
#define I2S3_I2STXRT1               ( 0x2B0D )  /* I2S3 Transmit Right Data 1 Register */

#define I2S0_I2SRXLT0               ( 0x2828 )  /* I2S0 Receive Left Data 0 Register */
#define I2S0_I2SRXLT1               ( 0x2829 )  /* I2S0 Receive Left Data 1 Register */
#define I2S0_I2SRXRT0               ( 0x282C )  /* I2S0 Receive Right Data 0 Register */
#define I2S0_I2SRXRT1               ( 0x282D )  /* I2S0 Receive Right Data 1 Register */
#define I2S1_I2SRXLT0               ( 0x2928 )  /* I2S1 Receive Left Data 0 Register */
#define I2S1_I2SRXLT1               ( 0x2929 )  /* I2S1 Receive Left Data 1 Register */
#define I2S1_I2SRXRT0               ( 0x292C )  /* I2S1 Receive Right Data 0 Register */
#define I2S1_I2SRXRT1               ( 0x292D )  /* I2S1 Receive Right Data 1 Register */
#define I2S2_I2SRXLT0               ( 0x2A28 )  /* I2S2 Receive Left Data 0 Register */
#define I2S2_I2SRXLT1               ( 0x2A29 )  /* I2S2 Receive Left Data 1 Register */
#define I2S2_I2SRXRT0               ( 0x2A2C )  /* I2S2 Receive Right Data 0 Register */
#define I2S2_I2SRXRT1               ( 0x2A2D )  /* I2S2 Receive Right Data 1 Register */
#define I2S3_I2SRXLT0               ( 0x2B28 )  /* I2S3 Receive Left Data 0 Register */
#define I2S3_I2SRXLT1               ( 0x2B29 )  /* I2S3 Receive Left Data 1 Register */
#define I2S3_I2SRXRT0               ( 0x2B2C )  /* I2S3 Receive Right Data 0 Register */
#define I2S3_I2SRXRT1               ( 0x2B2D )  /* I2S3 Receive Right Data 1 Register */

#ifdef USE_I2S0_PB
#define PSP_I2S_TX_INST_ID  ( PSP_I2S_0 )
#define I2S_TX_EVENT        ( PROG0_EVENT )
#define I2S_I2STXLT1        ( I2S0_I2STXLT1 )
#define I2S_I2STXRT1        ( I2S0_I2STXRT1 )
#elif defined USE_I2S1_PB
#define PSP_I2S_TX_INST_ID  ( PSP_I2S_1 )
#define I2S_TX_EVENT        ( PROG2_EVENT )
#define I2S_I2STXLT1        ( I2S1_I2STXLT1 )
#define I2S_I2STXRT1        ( I2S1_I2STXRT1 )
#elif defined USE_I2S2_PB
#define PSP_I2S_TX_INST_ID  ( PSP_I2S_2 )
#define I2S_TX_EVENT        ( XMT2_EVENT )
#define I2S_I2STXLT1        ( I2S2_I2STXLT1 )
#define I2S_I2STXRT1        ( I2S2_I2STXRT1 )
#elif defined USE_I2S3_PB
#define PSP_I2S_TX_INST_ID  ( PSP_I2S_3 )
#define I2S_TX_EVENT        ( XMT3_EVENT )
#define I2S_I2STXLT1        ( I2S3_I2STXLT1 )
#define I2S_I2STXRT1        ( I2S3_I2STXRT1 )
#endif

#ifdef USE_I2S0_REC
#define PSP_I2S_RX_INST_ID  ( PSP_I2S_0 )
#define I2S_RX_EVENT        ( PROG1_EVENT )
#define I2S_I2SRXLT1        ( I2S0_I2SRXLT1 )
#define I2S_I2SRXRT1        ( I2S0_I2SRXRT1 )
#elif defined USE_I2S1_REC
#define PSP_I2S_RX_INST_ID  ( PSP_I2S_1 )
#define I2S_RX_EVENT        ( PROG3_EVENT )
#define I2S_I2SRXLT1        ( I2S1_I2SRXLT1 )
#define I2S_I2SRXRT1        ( I2S1_I2SRXRT1 )
#elif defined USE_I2S2_REC
#define PSP_I2S_RX_INST_ID  ( PSP_I2S_2 )
#define I2S_RX_EVENT        ( RCV2_EVENT )
#define I2S_I2SRXLT1        ( I2S2_I2SRXLT1 )
#define I2S_I2SRXRT1        ( I2S2_I2SRXRT1 )
#elif defined USE_I2S3_REC
#define PSP_I2S_RX_INST_ID  ( PSP_I2S_3 )
#define I2S_RX_EVENT        ( RCV3_EVENT )
#define I2S_I2SRXLT1        ( I2S3_I2SRXLT1 )
#define I2S_I2SRXRT1        ( I2S3_I2SRXRT1 )
#endif

#ifdef USE_TWO_CODEC
#ifdef USE_I2S0_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_0 )
#define I2S_TX_EVENT_2        ( PROG0_EVENT )
#define I2S_I2STXLT1_2        ( I2S0_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S0_I2STXRT1 )
#elif defined USE_I2S1_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_1 )
#define I2S_TX_EVENT_2        ( PROG2_EVENT )
#define I2S_I2STXLT1_2        ( I2S1_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S1_I2STXRT1 )
#elif defined USE_I2S2_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_2 )
#define I2S_TX_EVENT_2        ( XMT2_EVENT )
#define I2S_I2STXLT1_2        ( I2S2_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S2_I2STXRT1 )
#elif defined USE_I2S3_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_3 )
#define I2S_TX_EVENT_2        ( XMT3_EVENT )
#define I2S_I2STXLT1_2        ( I2S3_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S3_I2STXRT1 )
#endif

#ifdef USE_I2S0_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_0 )
#define I2S_RX_EVENT_2        ( PROG1_EVENT )
#define I2S_I2SRXLT1_2        ( I2S0_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S0_I2SRXRT1 )
#elif defined USE_I2S1_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_1 )
#define I2S_RX_EVENT_2        ( PROG3_EVENT )
#define I2S_I2SRXLT1_2        ( I2S1_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S1_I2SRXRT1 )
#elif defined USE_I2S2_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_2 )
#define I2S_RX_EVENT_2        ( RCV2_EVENT )
#define I2S_I2SRXLT1_2        ( I2S2_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S2_I2SRXRT1 )
#elif defined USE_I2S3_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_3 )
#define I2S_RX_EVENT_2        ( RCV3_EVENT )
#define I2S_I2SRXLT1_2        ( I2S3_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S3_I2SRXRT1 )
#endif
#endif // USE_TWO_CODEC

#ifdef USE_THREE_CODEC
#ifdef USE_I2S0_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_0 )
#define I2S_TX_EVENT_2        ( PROG0_EVENT )
#define I2S_I2STXLT1_2        ( I2S0_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S0_I2STXRT1 )
#elif defined USE_I2S1_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_1 )
#define I2S_TX_EVENT_2        ( PROG2_EVENT )
#define I2S_I2STXLT1_2        ( I2S1_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S1_I2STXRT1 )
#elif defined USE_I2S2_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_2 )
#define I2S_TX_EVENT_2        ( XMT2_EVENT )
#define I2S_I2STXLT1_2        ( I2S2_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S2_I2STXRT1 )
#elif defined USE_I2S3_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_3 )
#define I2S_TX_EVENT_2        ( XMT3_EVENT )
#define I2S_I2STXLT1_2        ( I2S3_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S3_I2STXRT1 )
#endif

#ifdef USE_I2S0_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_0 )
#define I2S_RX_EVENT_2        ( PROG1_EVENT )
#define I2S_I2SRXLT1_2        ( I2S0_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S0_I2SRXRT1 )
#elif defined USE_I2S1_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_1 )
#define I2S_RX_EVENT_2        ( PROG3_EVENT )
#define I2S_I2SRXLT1_2        ( I2S1_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S1_I2SRXRT1 )
#elif defined USE_I2S2_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_2 )
#define I2S_RX_EVENT_2        ( RCV2_EVENT )
#define I2S_I2SRXLT1_2        ( I2S2_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S2_I2SRXRT1 )
#elif defined USE_I2S3_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_3 )
#define I2S_RX_EVENT_2        ( RCV3_EVENT )
#define I2S_I2SRXLT1_2        ( I2S3_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S3_I2SRXRT1 )
#endif

#ifdef USE_I2S0_PB_3
#define PSP_I2S_TX_INST_ID_3  ( PSP_I2S_0 )
#define I2S_TX_EVENT_3        ( PROG0_EVENT )
#define I2S_I2STXLT1_3        ( I2S0_I2STXLT1 )
#define I2S_I2STXRT1_3        ( I2S0_I2STXRT1 )
#elif defined USE_I2S1_PB_3
#define PSP_I2S_TX_INST_ID_3  ( PSP_I2S_1 )
#define I2S_TX_EVENT_3        ( PROG2_EVENT )
#define I2S_I2STXLT1_3        ( I2S1_I2STXLT1 )
#define I2S_I2STXRT1_3        ( I2S1_I2STXRT1 )
#elif defined USE_I2S2_PB_3
#define PSP_I2S_TX_INST_ID_3  ( PSP_I2S_2 )
#define I2S_TX_EVENT_3        ( XMT2_EVENT )
#define I2S_I2STXLT1_3        ( I2S2_I2STXLT1 )
#define I2S_I2STXRT1_3        ( I2S2_I2STXRT1 )
#elif defined USE_I2S3_PB_3
#define PSP_I2S_TX_INST_ID_3  ( PSP_I2S_3 )
#define I2S_TX_EVENT_3        ( XMT3_EVENT )
#define I2S_I2STXLT1_3        ( I2S3_I2STXLT1 )
#define I2S_I2STXRT1_3        ( I2S3_I2STXRT1 )
#endif //USE_I2S0_PB_3

#ifdef USE_I2S0_REC_3
#define PSP_I2S_RX_INST_ID_3  ( PSP_I2S_0 )
#define I2S_RX_EVENT_3        ( PROG1_EVENT )
#define I2S_I2SRXLT1_3        ( I2S0_I2SRXLT1 )
#define I2S_I2SRXRT1_3        ( I2S0_I2SRXRT1 )
#elif defined USE_I2S1_REC_3
#define PSP_I2S_RX_INST_ID_3  ( PSP_I2S_1 )
#define I2S_RX_EVENT_3        ( PROG3_EVENT )
#define I2S_I2SRXLT1_3        ( I2S1_I2SRXLT1 )
#define I2S_I2SRXRT1_3        ( I2S1_I2SRXRT1 )
#elif defined USE_I2S2_REC_3
#define PSP_I2S_RX_INST_ID_3  ( PSP_I2S_2 )
#define I2S_RX_EVENT_3        ( RCV2_EVENT )
#define I2S_I2SRXLT1_3        ( I2S2_I2SRXLT1 )
#define I2S_I2SRXRT1_3        ( I2S2_I2SRXRT1 )
#elif defined USE_I2S3_REC_3
#define PSP_I2S_RX_INST_ID_3  ( PSP_I2S_3 )
#define I2S_RX_EVENT_3        ( RCV3_EVENT )
#define I2S_I2SRXLT1_3        ( I2S3_I2SRXLT1 )
#define I2S_I2SRXRT1_3        ( I2S3_I2SRXRT1 )
#endif //USE_I2S0_REC_3
#endif // USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
#ifdef USE_I2S0_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_0 )
#define I2S_TX_EVENT_2        ( PROG0_EVENT )
#define I2S_I2STXLT1_2        ( I2S0_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S0_I2STXRT1 )
#elif defined USE_I2S1_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_1 )
#define I2S_TX_EVENT_2        ( PROG2_EVENT )
#define I2S_I2STXLT1_2        ( I2S1_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S1_I2STXRT1 )
#elif defined USE_I2S2_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_2 )
#define I2S_TX_EVENT_2        ( XMT2_EVENT )
#define I2S_I2STXLT1_2        ( I2S2_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S2_I2STXRT1 )
#elif defined USE_I2S3_PB_2
#define PSP_I2S_TX_INST_ID_2  ( PSP_I2S_3 )
#define I2S_TX_EVENT_2        ( XMT3_EVENT )
#define I2S_I2STXLT1_2        ( I2S3_I2STXLT1 )
#define I2S_I2STXRT1_2        ( I2S3_I2STXRT1 )
#endif

#ifdef USE_I2S0_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_0 )
#define I2S_RX_EVENT_2        ( PROG1_EVENT )
#define I2S_I2SRXLT1_2        ( I2S0_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S0_I2SRXRT1 )
#elif defined USE_I2S1_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_1 )
#define I2S_RX_EVENT_2        ( PROG3_EVENT )
#define I2S_I2SRXLT1_2        ( I2S1_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S1_I2SRXRT1 )
#elif defined USE_I2S2_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_2 )
#define I2S_RX_EVENT_2        ( RCV2_EVENT )
#define I2S_I2SRXLT1_2        ( I2S2_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S2_I2SRXRT1 )
#elif defined USE_I2S3_REC_2
#define PSP_I2S_RX_INST_ID_2  ( PSP_I2S_3 )
#define I2S_RX_EVENT_2        ( RCV3_EVENT )
#define I2S_I2SRXLT1_2        ( I2S3_I2SRXLT1 )
#define I2S_I2SRXRT1_2        ( I2S3_I2SRXRT1 )
#endif

#ifdef USE_I2S0_PB_3
#define PSP_I2S_TX_INST_ID_3  ( PSP_I2S_0 )
#define I2S_TX_EVENT_3        ( PROG0_EVENT )
#define I2S_I2STXLT1_3        ( I2S0_I2STXLT1 )
#define I2S_I2STXRT1_3        ( I2S0_I2STXRT1 )
#elif defined USE_I2S1_PB_3
#define PSP_I2S_TX_INST_ID_3  ( PSP_I2S_1 )
#define I2S_TX_EVENT_3        ( PROG2_EVENT )
#define I2S_I2STXLT1_3        ( I2S1_I2STXLT1 )
#define I2S_I2STXRT1_3        ( I2S1_I2STXRT1 )
#elif defined USE_I2S2_PB_3
#define PSP_I2S_TX_INST_ID_3  ( PSP_I2S_2 )
#define I2S_TX_EVENT_3        ( XMT2_EVENT )
#define I2S_I2STXLT1_3        ( I2S2_I2STXLT1 )
#define I2S_I2STXRT1_3        ( I2S2_I2STXRT1 )
#elif defined USE_I2S3_PB_3
#define PSP_I2S_TX_INST_ID_3  ( PSP_I2S_3 )
#define I2S_TX_EVENT_3        ( XMT3_EVENT )
#define I2S_I2STXLT1_3        ( I2S3_I2STXLT1 )
#define I2S_I2STXRT1_3        ( I2S3_I2STXRT1 )
#endif //USE_I2S0_PB_3

#ifdef USE_I2S0_REC_3
#define PSP_I2S_RX_INST_ID_3  ( PSP_I2S_0 )
#define I2S_RX_EVENT_3        ( PROG1_EVENT )
#define I2S_I2SRXLT1_3        ( I2S0_I2SRXLT1 )
#define I2S_I2SRXRT1_3        ( I2S0_I2SRXRT1 )
#elif defined USE_I2S1_REC_3
#define PSP_I2S_RX_INST_ID_3  ( PSP_I2S_1 )
#define I2S_RX_EVENT_3        ( PROG3_EVENT )
#define I2S_I2SRXLT1_3        ( I2S1_I2SRXLT1 )
#define I2S_I2SRXRT1_3        ( I2S1_I2SRXRT1 )
#elif defined USE_I2S2_REC_3
#define PSP_I2S_RX_INST_ID_3  ( PSP_I2S_2 )
#define I2S_RX_EVENT_3        ( RCV2_EVENT )
#define I2S_I2SRXLT1_3        ( I2S2_I2SRXLT1 )
#define I2S_I2SRXRT1_3        ( I2S2_I2SRXRT1 )
#elif defined USE_I2S3_REC_3
#define PSP_I2S_RX_INST_ID_3  ( PSP_I2S_3 )
#define I2S_RX_EVENT_3        ( RCV3_EVENT )
#define I2S_I2SRXLT1_3        ( I2S3_I2SRXLT1 )
#define I2S_I2SRXRT1_3        ( I2S3_I2SRXRT1 )
#endif //USE_I2S0_REC_3

#ifdef USE_I2S0_PB_4
#define PSP_I2S_TX_INST_ID_4  ( PSP_I2S_0 )
#define I2S_TX_EVENT_4        ( PROG0_EVENT )
#define I2S_I2STXLT1_4        ( I2S0_I2STXLT1 )
#define I2S_I2STXRT1_4        ( I2S0_I2STXRT1 )
#elif defined USE_I2S1_PB_4
#define PSP_I2S_TX_INST_ID_4  ( PSP_I2S_1 )
#define I2S_TX_EVENT_4        ( PROG2_EVENT )
#define I2S_I2STXLT1_4        ( I2S1_I2STXLT1 )
#define I2S_I2STXRT1_4        ( I2S1_I2STXRT1 )
#elif defined USE_I2S2_PB_4
#define PSP_I2S_TX_INST_ID_4  ( PSP_I2S_2 )
#define I2S_TX_EVENT_4        ( XMT2_EVENT )
#define I2S_I2STXLT1_4        ( I2S2_I2STXLT1 )
#define I2S_I2STXRT1_4        ( I2S2_I2STXRT1 )
#elif defined USE_I2S3_PB_4
#define PSP_I2S_TX_INST_ID_4  ( PSP_I2S_3 )
#define I2S_TX_EVENT_4        ( XMT3_EVENT )
#define I2S_I2STXLT1_4        ( I2S3_I2STXLT1 )
#define I2S_I2STXRT1_4        ( I2S3_I2STXRT1 )
#endif //USE_I2S0_PB_4

#ifdef USE_I2S0_REC_4
#define PSP_I2S_RX_INST_ID_4  ( PSP_I2S_0 )
#define I2S_RX_EVENT_4        ( PROG1_EVENT )
#define I2S_I2SRXLT1_4        ( I2S0_I2SRXLT1 )
#define I2S_I2SRXRT1_4        ( I2S0_I2SRXRT1 )
#elif defined USE_I2S1_REC_4
#define PSP_I2S_RX_INST_ID_4  ( PSP_I2S_1 )
#define I2S_RX_EVENT_4        ( PROG3_EVENT )
#define I2S_I2SRXLT1_4        ( I2S1_I2SRXLT1 )
#define I2S_I2SRXRT1_4        ( I2S1_I2SRXRT1 )
#elif defined USE_I2S2_REC_4
#define PSP_I2S_RX_INST_ID_4  ( PSP_I2S_2 )
#define I2S_RX_EVENT_4        ( RCV2_EVENT )
#define I2S_I2SRXLT1_4        ( I2S2_I2SRXLT1 )
#define I2S_I2SRXRT1_4        ( I2S2_I2SRXRT1 )
#elif defined USE_I2S3_REC_4
#define PSP_I2S_RX_INST_ID_4  ( PSP_I2S_3 )
#define I2S_RX_EVENT_4        ( RCV3_EVENT )
#define I2S_I2SRXLT1_4        ( I2S3_I2SRXLT1 )
#define I2S_I2SRXRT1_4        ( I2S3_I2SRXRT1 )
#endif //USE_I2S0_REC_4
#endif // USE_FOUR_CODEC

/* Status return codes */
#define I2SSAMPLE_SOK               ( 0 )
#define I2SSAMPLE_INV_PRMS          ( 1 )
#define I2SSAMPLE_I2SINIT_PB_FAIL   ( 2 )
#define I2SSAMPLE_I2SINIT_REC_FAIL  ( 3 )
#define I2SSAMPLE_DMAINIT_PB_FAIL   ( 4 )
#define I2SSAMPLE_DMAINIT_REC_FAIL  ( 5 )

/* I2S initialization parameters */
typedef struct I2sInitPrms 
{
    Bool enablePlayback;        /* enable/disable playback */
    Bool enableStereoPb;        /* enable/disable stereo playback */
    Bool sampleBySamplePb;      /* enable/disable DMA for playback */
    Int16 *pingPongI2sTxLeftBuf;    /* Left ch. Tx ping/pong buffer (DMA ping/pong hardware enabled) */
    Int16 *pingPongI2sTxRightBuf;   /* Right ch. Tx ping/pong buffer (DMA ping/pong hardware enabled) */
    PSP_I2SInstanceId i2sPb;    /* I2S for playback */
    Bool enableRecord;          /* enable/disable record */
    Bool enableStereoRec;       /* enable/disable stereo record */
    Int16 *pingPongI2sRxLeftBuf;    /* Left ch. Rx ping/pong buffer (DMA ping/pong hardware enabled) */
    Int16 *pingPongI2sRxRightBuf;   /* Right ch. Rx ping/pong buffer (DMA ping/pong hardware enabled) */
    PSP_I2SInstanceId i2sRec;   /* I2S for record */
} I2sInitPrms;

/* I2S handles */
extern PSP_Handle i2sHandleTx;
extern PSP_Handle i2sHandleRx;

/**< DMA handle for left transmit channel   */
extern DMA_ChanHandle   hDmaTxLeft;
/**< DMA handle for right transmit channel  */
extern DMA_ChanHandle   hDmaTxRight;

/**< DMA handle for left receive channel   */
extern DMA_ChanHandle   hDmaRxLeft;
/**< DMA handle for right receive channel  */
extern DMA_ChanHandle   hDmaRxRight;

#ifdef USE_TWO_CODEC
/* I2S handles */
extern PSP_Handle i2sHandleTx2;
extern PSP_Handle i2sHandleRx2;

/**< DMA handle for left transmit channel   */
extern DMA_ChanHandle   hDmaTxLeft2;
/**< DMA handle for right transmit channel  */
extern DMA_ChanHandle   hDmaTxRight2;

/**< DMA handle for left receive channel   */
extern DMA_ChanHandle   hDmaRxLeft2;
/**< DMA handle for right receive channel  */
extern DMA_ChanHandle   hDmaRxRight2;
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
/* I2S handles */
extern PSP_Handle i2sHandleTx2;
extern PSP_Handle i2sHandleRx2;

/**< DMA handle for left transmit channel   */
extern DMA_ChanHandle   hDmaTxLeft2;
/**< DMA handle for right transmit channel  */
extern DMA_ChanHandle   hDmaTxRight2;

/**< DMA handle for left receive channel   */
extern DMA_ChanHandle   hDmaRxLeft2;
/**< DMA handle for right receive channel  */
extern DMA_ChanHandle   hDmaRxRight2;

/* I2S handles */
extern PSP_Handle i2sHandleTx3;
extern PSP_Handle i2sHandleRx3;

/**< DMA handle for left transmit channel   */
extern DMA_ChanHandle   hDmaTxLeft3;
/**< DMA handle for right transmit channel  */
extern DMA_ChanHandle   hDmaTxRight3;

/**< DMA handle for left receive channel   */
extern DMA_ChanHandle   hDmaRxLeft3;
/**< DMA handle for right receive channel  */
extern DMA_ChanHandle   hDmaRxRight3;
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
/* I2S handles */
extern PSP_Handle i2sHandleTx2;
extern PSP_Handle i2sHandleRx2;

/**< DMA handle for left transmit channel   */
extern DMA_ChanHandle   hDmaTxLeft2;
/**< DMA handle for right transmit channel  */
extern DMA_ChanHandle   hDmaTxRight2;

/**< DMA handle for left receive channel   */
extern DMA_ChanHandle   hDmaRxLeft2;
/**< DMA handle for right receive channel  */
extern DMA_ChanHandle   hDmaRxRight2;

/* I2S handles */
extern PSP_Handle i2sHandleTx3;
extern PSP_Handle i2sHandleRx3;

/**< DMA handle for left transmit channel   */
extern DMA_ChanHandle   hDmaTxLeft3;
/**< DMA handle for right transmit channel  */
extern DMA_ChanHandle   hDmaTxRight3;

/**< DMA handle for left receive channel   */
extern DMA_ChanHandle   hDmaRxLeft3;
/**< DMA handle for right receive channel  */
extern DMA_ChanHandle   hDmaRxRight3;

/* I2S handles */
extern PSP_Handle i2sHandleTx4;
extern PSP_Handle i2sHandleRx4;

/**< DMA handle for left transmit channel   */
extern DMA_ChanHandle   hDmaTxLeft4;
/**< DMA handle for right transmit channel  */
extern DMA_ChanHandle   hDmaTxRight4;

/**< DMA handle for left receive channel   */
extern DMA_ChanHandle   hDmaRxLeft4;
/**< DMA handle for right receive channel  */
extern DMA_ChanHandle   hDmaRxRight4;
#endif //USE_FOUR_CODEC

/* Codec input ping/pong buffer (Left ch.) */
extern Uint16 ping_pong_i2sRxLeftBuf[]; /* 2x for ping/pong */
#ifdef USE_TWO_CODEC
extern Uint16 ping_pong_i2sRxLeftBuf2[]; /* 2x for 2nd ping/pong */
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
extern Uint16 ping_pong_i2sRxLeftBuf2[]; /* 2x for 2nd ping/pong */
extern Uint16 ping_pong_i2sRxLeftBuf3[]; /* 2x for 3rd ping/pong */
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
extern Uint16 ping_pong_i2sRxLeftBuf2[]; /* 2x for 2nd ping/pong */
extern Uint16 ping_pong_i2sRxLeftBuf3[]; /* 2x for 3rd ping/pong */
extern Uint16 ping_pong_i2sRxLeftBuf4[]; /* 2x for 4th ping/pong */
#endif //USE_FOUR_CODEC

extern Int16 left_rx_buf_sel;
/* Codec input ping/pong buffer (Right ch.) */
extern Uint16 ping_pong_i2sRxRightBuf[]; /* 2x for ping/pong */
#ifdef USE_TWO_CODEC
extern Uint16 ping_pong_i2sRxRightBuf2[]; /* 2x for 2nd ping/pong */
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
extern Uint16 ping_pong_i2sRxRightBuf2[]; /* 2x for 2nd ping/pong */
extern Uint16 ping_pong_i2sRxRightBuf3[]; /* 2x for 3rd ping/pong */
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
extern Uint16 ping_pong_i2sRxRightBuf2[]; /* 2x for 2nd ping/pong */
extern Uint16 ping_pong_i2sRxRightBuf3[]; /* 2x for 3rd ping/pong */
extern Uint16 ping_pong_i2sRxRightBuf4[]; /* 2x for 4th ping/pong */
#endif //USE_FOUR_CODEC
extern Int16 right_rx_buf_sel;

/* Codec input circular buffer */
extern Uint16 codec_input_buffer[];
extern Uint16 codec_input_buffer_input_index;
extern Uint16 codec_input_buffer_output_index;
extern Uint32 codec_input_buffer_underflow;
extern Uint32 codec_input_buffer_overflow;
extern Uint32 codec_input_buffer_sample;

/* Codec output ping/pong buffer (Left ch.) */
/* NOTE: Left & Right interleaved channels for sample-by-sample playback */
extern Int16 ping_pong_i2sTxLeftBuf[]; /* 2x for ping/pong */
#ifdef USE_TWO_CODEC
extern Int16 ping_pong_i2sTxLeftBuf2[]; /* 2x for 2nd ping/pong */
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
extern Int16 ping_pong_i2sTxLeftBuf2[]; /* 2x for 2nd ping/pong */
extern Int16 ping_pong_i2sTxLeftBuf3[]; /* 2x for 3rd ping/pong */
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
extern Int16 ping_pong_i2sTxLeftBuf2[]; /* 2x for 2nd ping/pong */
extern Int16 ping_pong_i2sTxLeftBuf3[]; /* 2x for 3rd ping/pong */
extern Int16 ping_pong_i2sTxLeftBuf4[]; /* 2x for 4th ping/pong */
#endif //USE_FOUR_CODEC
extern Int16 left_tx_buf_sel;
/* Codec output ping/pong buffer (Right ch.) */
extern Int16 ping_pong_i2sTxRightBuf[]; /* 2x for ping/pong */
#ifdef USE_TWO_CODEC
extern Int16 ping_pong_i2sTxRightBuf2[]; /* 2x for 2nd ping/pong */
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
extern Int16 ping_pong_i2sTxRightBuf2[]; /* 2x for 2nd ping/pong */
extern Int16 ping_pong_i2sTxRightBuf3[]; /* 2x for 3rd ping/pong */
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
extern Int16 ping_pong_i2sTxRightBuf2[]; /* 2x for 2nd ping/pong */
extern Int16 ping_pong_i2sTxRightBuf3[]; /* 2x for 3rd ping/pong */
extern Int16 ping_pong_i2sTxRightBuf4[]; /* 2x for 4th ping/pong */
#endif //USE_FOUR_CODEC
extern Int16 right_tx_buf_sel;

/* Codec output circular buffer */
extern Uint16 codec_output_buffer[];
extern Uint16 codec_output_buffer_input_index;
extern Uint16 codec_output_buffer_output_index;
extern Uint32 codec_output_buffer_underflow;
extern Uint32 codec_output_buffer_overflow;
extern Uint32 codec_output_buffer_sample;

/* Run-time size of Tx ping/pong buffer */
extern Uint16 i2sTxBuffSz;

void DDC_I2S_transEnable(DDC_I2SHandle    hI2s, Uint16 enableBit);

void i2sTxStart(void);

/* I2S Tx ISR */
void i2s_txIsr(void); /* SampleBySample */

/* Initializes I2S and associated DMA channels for Playback and Record */
Int16 i2sInit(
    I2sInitPrms *pI2sInitPrms
);

#ifdef USE_TWO_CODEC
/* Initializes 2nd I2S and associated DMA channels for Playback and Record */
Int16 i2sInit2(
    I2sInitPrms *pI2sInitPrms
);
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
/* Initializes 2nd I2S and associated DMA channels for Playback and Record */
Int16 i2sInit2(
    I2sInitPrms *pI2sInitPrms
);
/* Initializes 3rd I2S and associated DMA channels for Playback and Record */
Int16 i2sInit3(
    I2sInitPrms *pI2sInitPrms
);
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
/* Initializes 2nd I2S and associated DMA channels for Playback and Record */
Int16 i2sInit2(
    I2sInitPrms *pI2sInitPrms
);
/* Initializes 3rd I2S and associated DMA channels for Playback and Record */
Int16 i2sInit3(
    I2sInitPrms *pI2sInitPrms
);
/* Initializes 4th I2S and associated DMA channels for Playback and Record */
Int16 i2sInit4(
    I2sInitPrms *pI2sInitPrms
);
#endif //USE_FOUR_CODEC

/* Resets codec output buffer */
void reset_codec_output_buffer(void);

#endif /* _I2S_SAMPLE_H_ */
