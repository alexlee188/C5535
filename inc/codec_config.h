/*
 * $$$MODULE_NAME codec_config.h
 *
 * $$$MODULE_DESC codec_config.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _CODEC_CFG_H_
#define _CODEC_CFG_H_

typedef enum {
	/** Adjust left volume message                                           */
    CODEC_CFG_MSG_ADJ_VOL_L = 0,
	/** Adjust right volume message                                          */
    CODEC_CFG_MSG_ADJ_VOL_R,
	/** Adjust mute message                                                  */
    CODEC_CFG_MSG_ADJ_MUTE,
    CODEC_CFG_MSG_MUTE,
    CODEC_CFG_MSG_UNMUTE,
    CODEC_CFG_MSG_FORCE_MUTE,
    CODEC_CFG_MSG_FORCE_UNMUTE,
	/** Adjust sample rate                                                   */
    CODEC_CFG_MSG_ADJ_RATE
} CodecConfigMsg;

typedef struct CodecCfgMsgObj {
	/** \brief  Message posted                                               */
    Int16    wMsg;
    /** \brief  Message data                                                 */
    void    *wData;
} CodecCfgMsgObj;

extern Bool gPbSampRateChange;
extern Uint16 gSetPbSampRateFlag;
extern long gSetPbSampRate;
extern long gSetPbSampRateTemp;
extern long gSetPbSampRatePrev;
// the number of sample per uFrame
extern Uint16 gSetRecSampNum;
// the revise amount to the number of sample per uFrame
extern Uint16 gSetRecSampNumDelta;

#endif /* _CODEC_CFG_H_ */
