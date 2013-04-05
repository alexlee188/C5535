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

/* Modifications of the software from Texas Instruments are under the following license:
 *
 * Copyright (C) 2013 Alex Lee
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _CODEC_AIC3204_H_
#define _CODEC_AIC3204_H_

#include "psp_common.h"

PSP_Result AIC3254_init(long sampRatePb, long sampRaterec);
PSP_Result AIC3254_set_sample_rate(long sampRatePb);
Bool Set_Mute_State(Bool flag);
Bool Adjust_Volume(Int16 volume, Uint16 channel);
PSP_Result AIC3254_Write(Uint16 regAddr, Uint16 regData, PSP_Handle hi2c);
PSP_Result AIC3254_Read(Uint16 regAddr, Uint16 *Data,PSP_Handle  hi2c);

#endif /* _CODEC_AIC3204_H_ */
