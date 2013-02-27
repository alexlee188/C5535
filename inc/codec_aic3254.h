/*
 * $$$MODULE_NAME codec_aic3254.h
 *
 * $$$MODULE_DESC codec_aic3254.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _CODEC_AIC3204_H_
#define _CODEC_AIC3204_H_

#include "psp_common.h"

PSP_Result AIC3254_init(long sampRatePb, long sampRaterec);
Bool Set_Mute_State(Bool flag);
Bool Adjust_Volume(Int16 volume, Uint16 channel);
PSP_Result AIC3254_Write(Uint16 regAddr, Uint16 regData, PSP_Handle hi2c);
PSP_Result AIC3254_Read(Uint16 regAddr, Uint16 *Data,PSP_Handle  hi2c);

#endif /* _CODEC_AIC3204_H_ */
