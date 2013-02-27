/*
 * $$$MODULE_NAME sample_rate.h
 *
 * $$$MODULE_DESC sample_rate.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _SAMPLE_RATE_H_
#define _SAMPLE_RATE_H_

#define RATE_1KHZ               ( 1000 )

/* Audio sampling rates */
#define SAMP_RATE_8KHZ          ( 0x00001F40 )
#define SAMP_RATE_16KHZ         ( 0x00003E80 )
#define SAMP_RATE_22_05KHZ      ( 0x00005622 )
#define SAMP_RATE_24KHZ         ( 0x00005DC0 )
#define SAMP_RATE_32KHZ         ( 0x00007D00 )
#define SAMP_RATE_44_1KHZ       ( 0x0000AC44 )
#define SAMP_RATE_48KHZ         ( 0x0000BB80 )
#define SAMP_RATE_88_2KHZ       ( 0x00015888 )
#define SAMP_RATE_96KHZ         ( 0x00017700 )

/* Audio sampling rates -- samples/msec. */
#define SAMPS_PER_MSEC_8KHZ     ( SAMP_RATE_8KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_16KHZ    ( SAMP_RATE_16KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_22_05KHZ ( SAMP_RATE_22_05KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_24KHZ    ( SAMP_RATE_24KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_32KHZ    ( SAMP_RATE_32KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_44_1KHZ  ( SAMP_RATE_44_1KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_48KHZ    ( SAMP_RATE_48KHZ / RATE_1KHZ )
#define SAMPS_PER_MSEC_88_2KHZ  ( 88 )
#define SAMPS_PER_MSEC_96KHZ    ( 96 )

#endif /* _SAMPLE_RATE_H_ */
