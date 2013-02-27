/*
 * $$$MODULE_NAME codec_aic3254.c
 *
 * $$$MODULE_DESC codec_aic3254.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/**
 *  \file   codec_aic3254.c
 *
 *  \brief  codec configuration function
 *
 *   This file contains the APIs for codec(AIC3254) read and write using I2C
 *
 *  (C) Copyright 2005, Texas Instruments, Inc
 *
 *  \author     PR Mistral
 *
 *  \version    1.0
 *
 */

#include "psp_i2s.h"
#include "psp_i2c.h"
#include "dda_i2c.h"
#include "psp_common.h"
#include "codec_aic3254.h"
#include "app_globals.h"
#include "VC5505_CSL_BIOS_cfg.h"
#include "sample_rate.h"

#define I2C_OWN_ADDR            (0x2F)
#define I2C_BUS_FREQ            (10000u)
#define I2C_CODEC_ADDR          (0x18)

PSP_Handle    hi2c = NULL;

/*
 * Mute control for codec output
 * TRUE = Mute codec output
 * FALSE = UnMute codec output
 ***********************************************************************/
Bool Set_Mute_State(Bool flag)
{
    PSP_Result    result = PSP_SOK;
    Bool retval;

    retval = TRUE;

    // write 0 to page register to select page 0
    result = AIC3254_Write(0, 0, hi2c);
    if (result != PSP_SOK) 
    {
        retval = FALSE;
    }
    else
    {
        if (flag == TRUE)
        {
            //mute output
            result = AIC3254_Write(64,0xd,hi2c);
            if (result != PSP_SOK) 
            {
                retval = FALSE;
            }
        }
        else
        {
            //unmute output
            result = AIC3254_Write(64,0x1,hi2c);
            if (result != PSP_SOK) 
            {
                retval = FALSE;
            }
        }
    }
#if 1
    // write 1 to page register to select page 1 - prepare for next headset volume change
    result = AIC3254_Write(0, 1, hi2c);
    if (result != PSP_SOK) 
    {
        retval = FALSE;
    }
#endif
    return retval;
}

#define HEADPHONE_DRIVER_GAIN_MUTE_ENABLE  0x40    // bit6 =1 mute headphone driver
#define VOLUME_STEP_SIZE                   256
#define VOLUME_TABLE_MAX_GAIN_INDEX        29      // headphone gain setting = 29 -> 29 dB gain
#define VOLUME_TABLE_MAX_ATTNEUATION_INDEX 35      // headphone gain setting = 0x3A -> -6dB gain
#define USB_MAX_ATTENUATION_VALUE          -32768
#define VOLUME_TABLE_MUTE_HEADPHONE_INDEX  36      // headphone gain setting = 0x7B set gain to -5dB with headphone driver muted

// table has both gain and attenuation settings for headphone output of the codec.
// 0 : no gain/no attenuation, gain : 1 - 29, attenuation : 0x3F - 0x3A, muted: 0x7B
const Uint16 volume_table[] =  {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
    0x3F,0x3E,0x3D,0x3C,0x3B,0x3A,(0x3B | HEADPHONE_DRIVER_GAIN_MUTE_ENABLE)
};

/*
 * Change gain setting of headphone output of codec
 * volume = gain setting received from USB
 * channel = 0:left channel, =1 right channel
 ***********************************************************************/
Bool Adjust_Volume(Int16 volume, Uint16 channel)
{
    PSP_Result    result = PSP_SOK;
    Uint16        gain;

    // goto max attentuation
    if (volume == USB_MAX_ATTENUATION_VALUE)
    {
        // the max attenuation for the headpphone  is -6dB so we will mute the headphone driver
        // and set the codec gain to the lowest value(-5dB) that allows the headphone driver
        // to be muted. any volume change other than the max attenuation will turn off the
        // headphone driver mute
        gain = VOLUME_TABLE_MUTE_HEADPHONE_INDEX;
    }
    else if (volume >= 0)
    {
        // determine gain index
        gain = volume/VOLUME_STEP_SIZE;

        // check range
        if(gain > VOLUME_TABLE_MAX_GAIN_INDEX)
        {
            // set to max gain
            gain = VOLUME_TABLE_MAX_GAIN_INDEX;
        }
    }
    else
    {
        // determine attenuation index
        gain = (-volume)/VOLUME_STEP_SIZE;
        if (gain !=0)
        {
            //index from start of attentuation values in table
            gain += VOLUME_TABLE_MAX_GAIN_INDEX;
            if (gain > VOLUME_TABLE_MAX_ATTNEUATION_INDEX)
            {
                // set to max attenuation
                gain = VOLUME_TABLE_MAX_ATTNEUATION_INDEX;
            }
        }

    }

    if (channel == 0)
    {
        //adjust volume setting of left headphone
        result = AIC3254_Write(0x10,volume_table[gain],hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
    }
    else
    {
        //adjust volume setting of right headphone
        result = AIC3254_Write(0x11,volume_table[gain],hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
    }
    return TRUE;
}

PSP_Result AIC3254_init(long sampRatePb, long sampRateRec)
{
    PSP_Result result = PSP_SOK;
    volatile Uint16 looper;

	// Playback sample rate has to be the same as the record sample rate
	if (sampRatePb!=sampRateRec)
	{
		return PSP_E_NOT_SUPPORTED;
	}

    /* Reset AIC3254 */
    /* NOTE: Assumes EBSR and GPIO are set correctly before function is called */
    CSL_FINS((*GPIO_DOUT0_ADDR), GPIO_DOUT0, 0x0000); /* reset active low */
    for(looper=0; looper<10; looper++ )
        asm("    nop");
    CSL_FINS((*GPIO_DOUT0_ADDR), GPIO_DOUT0, 0x0400);

    hi2c = I2C_Init(0x2f, 300000); /* 38 kHz, assuming 100 MHZ cpu clock */
    if (hi2c)
    {
        result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(1, 1, hi2c); // reset codec
        if (result != PSP_SOK) 
        {
            return result;
        }

//////////////////////////////////////////////////////////
//					 Filter coefficient load 			//
//////////////////////////////////////////////////////////
		if (sampRatePb==SAMP_RATE_48KHZ)
		{
// Text "* Sample Rate = 48000"
// Text "* Filter 1 High Pass 1st Order Butterworth, 20 Hz Fc 0.0 dB "
// Text "* Filter 2 High Pass 2nd Order Butterworth, 20 Hz Fc 0.0 dB "
// 
// Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (8388608 - 2*D1/z - D2/(z2))"
// Text "* Filter Coefficients in format N0, N1, N2, D1, D2"
// 
// Text "* 1O: H(z) = (N0 + N1/z) / (8388608 - D1/z)"
// Text "* Filter Coefficients in format N0, N1, D1"
//
// Text "* Filter 1 1O "
// 0x7FD528
// 0x802AD8
// 0x7FAA52
//        
//		  ADC Coeff Left Channel		ADC Coeff Right Channel
// N0 --> C4 (Pg 8, Reg 24,25,26) 		C36 (Pg 9, Reg 32,33,34)
// N1 --> C5 (Pg 8, Reg 28,29,30)		C37 (Pg 9, Reg 36,37,38)
// D1 --> C6 (Pg 8, Reg 32,33,34)		C38 (Pg 9, Reg 40,31,41)
//
//
// Text "* Filter 2 BQ "
// 0x7FC362
// 0x803C9E
// 0x7FC362
// 0x7FC353
// 0x80791F
//
// BiQUAD A
//        ADC Coeff Left Channel		ADC Coeff Right Channel 
// N0 --> C7 (Pg 8, Reg 36,37,38)		C39 (Pg 9, Reg 44,45,46)
// N1 --> C8 (Pg 8, Reg 40,41,42)		C40 (Pg 9, Reg 48,49,50)
// N2 --> C9 (Pg 8, Reg 44,45,46)		C41 (Pg 9, Reg 52,53,54)
// D1 --> C10 (Pg 8, Reg 48,49,50)		C39 (Pg 9, Reg 56,57,58)
// D2 --> C11 (Pg 8, Reg 52,53,54)		C39 (Pg 9, Reg 60,61,62)
//
//////////////////////////////////////////////////////////
//
////////Left
//      Filter 1
    	result = AIC3254_Write(0, 8, hi2c); // write 8 to page register to select page 8
    	if (result != PSP_SOK) 
    	{
        	return result;
    	}
    	
        result = AIC3254_Write(24, 0x7F, hi2c); // Filter 1 1O N0--> 0x7FD528
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(25, 0xD5, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(26, 0x28, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }     
        result = AIC3254_Write(28, 0x80, hi2c); // Filter 1 1O N1--> 0x802AD8
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(29, 0x2A, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(30, 0xD8, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }      
        result = AIC3254_Write(32, 0x7E, hi2c); // Filter 1 1O D1--> 0x7FAA52
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(33, 0xAA, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(34, 0x52, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }   
//      Filter 2
        result = AIC3254_Write(36, 0x7F, hi2c); // Filter 2 BQ N0--> 0x7FC362
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(37, 0xC3, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(38, 0x62, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }     
        result = AIC3254_Write(40, 0x80, hi2c); // Filter 2 BQ  N1--> 0x803C9E
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(41, 0x3C, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(42, 0x9E, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }      
        result = AIC3254_Write(44, 0x7F, hi2c); // Filter 2 BQ  N2--> 0x7FC362
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(45, 0xC3, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(46, 0x62, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
        result = AIC3254_Write(48, 0x7F, hi2c); // Filter 2 BQ  D1--> 0x7FC353
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(49, 0xC3, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(50, 0x53, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
        result = AIC3254_Write(52, 0x80, hi2c); // Filter 2 BQ  D2--> 0x80791F
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(53, 0x79, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(54, 0x1F, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
// Right channel 
// Filter 1       
    	result = AIC3254_Write(0, 9, hi2c); // write 8 to page register to select page 8
    	if (result != PSP_SOK) 
    	{
        	return result;
    	}
    	
        result = AIC3254_Write(32, 0x7F, hi2c); // Filter 1 1O N0--> 0x7FD528
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(33, 0xD5, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(34, 0x28, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }     
        result = AIC3254_Write(36, 0x80, hi2c); // Filter 1 1O N1--> 0x802AD8
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(37, 0x2A, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(38, 0xD8, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }      
        result = AIC3254_Write(40, 0x7F, hi2c); // Filter 1 1O D1--> 0x7FAA52
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(41, 0xAA, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(42, 0x52, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
//Filter 2
        result = AIC3254_Write(44, 0x7F, hi2c); // Filter 2 BQ N0--> 0x7FC362
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(45, 0xC3, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(46, 0x62, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }     
        result = AIC3254_Write(48, 0x80, hi2c); // Filter 2 BQ  N1--> 0x803C9E
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(49, 0x3C, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(50, 0x9E, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }      
        result = AIC3254_Write(52, 0x7F, hi2c); // Filter 2 BQ  N2--> 0x7FC362
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(53, 0xC3, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(54, 0x62, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
        result = AIC3254_Write(56, 0x7F, hi2c); // Filter 2 BQ  D1--> 0x7FC353
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(57, 0xC3, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(58, 0x53, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
        result = AIC3254_Write(60, 0x80, hi2c); // Filter 2 BQ  D2--> 0x80791F
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(61, 0x79, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(62, 0x1F, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }                                  
		} // if (sampRatePb==SAMP_RATE_48KHZ) 

		if (sampRatePb==SAMP_RATE_96KHZ)
		{
//
// Text "* Sample Rate = 96000"
// Text "* Filter 1 High Pass 1st Order Butterworth, 20 Hz Fc 0.0 dB "
// Text "* Filter 2 High Pass 2nd Order Butterworth, 20 Hz Fc 0.0 dB "
//
// Text "* BQ: H(z) = (N0 + 2*N1/z + N2/(z2)) / (8388608 - 2*D1/z - D2/(z2))"
// Text "* Filter Coefficients in format N0, N1, N2, D1, D2"
//
// Text "* 1O: H(z) = (N0 + N1/z) / (8388608 - D1/z)"
// Text "* Filter Coefficients in format N0, N1, D1"
//
// Text "* Filter 1 1O "
// 0x7FEA90
// 0x801570
// 0x7FD521
//        
//		  ADC Coeff Left Channel		ADC Coeff Right Channel
// N0 --> C4 (Pg 8, Reg 24,25,26) 		C36 (Pg 9, Reg 32,33,34)
// N1 --> C5 (Pg 8, Reg 28,29,30)		C37 (Pg 9, Reg 36,37,38)
// D1 --> C6 (Pg 8, Reg 32,33,34)		C38 (Pg 9, Reg 40,31,41)
//
//
// Text "* Filter 2 BQ "
// 0x7FE1AC
// 0x801E54
// 0x7FE1AC
// 0x7FE1A9
// 0x803C9E
//
// BiQUAD A
//        ADC Coeff Left Channel		ADC Coeff Right Channel 
// N0 --> C7 (Pg 8, Reg 36,37,38)		C39 (Pg 9, Reg 44,45,46)
// N1 --> C8 (Pg 8, Reg 40,41,42)		C40 (Pg 9, Reg 48,49,50)
// N2 --> C9 (Pg 8, Reg 44,45,46)		C41 (Pg 9, Reg 52,53,54)
// D1 --> C10 (Pg 8, Reg 48,49,50)		C39 (Pg 9, Reg 56,57,58)
// D2 --> C11 (Pg 8, Reg 52,53,54)		C39 (Pg 9, Reg 60,61,62)
//
//////////////////////////////////////////////////////////
//
////////Left
//      Filter 1
    	result = AIC3254_Write(0, 8, hi2c); // write 8 to page register to select page 8
    	if (result != PSP_SOK) 
    	{
        	return result;
    	}
    	
        result = AIC3254_Write(24, 0x7F, hi2c); // Filter 1 1O N0--> 0x7FEA90
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(25, 0xEA, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(26, 0x90, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }     
        result = AIC3254_Write(28, 0x80, hi2c); // Filter 1 1O N1--> 0x801570
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(29, 0x15, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(30, 0x70, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }      
        result = AIC3254_Write(32, 0x7F, hi2c); // Filter 1 1O D1--> 0x7FD521
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(33, 0xD5, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(34, 0x21, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }   
//      Filter 2
        result = AIC3254_Write(36, 0x7F, hi2c); // Filter 2 BQ N0--> 0x7FE1AC
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(37, 0xE1, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(38, 0xAC, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }     
        result = AIC3254_Write(40, 0x80, hi2c); // Filter 2 BQ  N1--> 0x801E54
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(41, 0x1E, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(42, 0x54, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }      
        result = AIC3254_Write(44, 0x7F, hi2c); // Filter 2 BQ  N2--> 0x7FE1AC
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(45, 0xE1, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(46, 0xAC, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
        result = AIC3254_Write(48, 0x7F, hi2c); // Filter 2 BQ  D1--> 0x7FE1A9
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(49, 0xE1, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(50, 0xA9, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
        result = AIC3254_Write(52, 0x80, hi2c); // Filter 2 BQ  D2--> 0x803C9E
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(53, 0x3C, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(54, 0x9E, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
// Right channel 
// Filter 1       
    	result = AIC3254_Write(0, 9, hi2c); // write 8 to page register to select page 8
    	if (result != PSP_SOK) 
    	{
        	return result;
    	}
    	
        result = AIC3254_Write(32, 0x7F, hi2c); // Filter 1 1O N0--> 0x7FEA90
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(33, 0xEA, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(34, 0x90, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }     
        result = AIC3254_Write(36, 0x80, hi2c); // Filter 1 1O N1--> 0x801570
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(37, 0x15, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(38, 0x70, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }      
        result = AIC3254_Write(40, 0x7F, hi2c); // Filter 1 1O D1--> 0x7FD521
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(41, 0xD5, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(42, 0x21, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
//Filter 2
        result = AIC3254_Write(44, 0x7F, hi2c); // Filter 2 BQ N0--> 0x7FE1AC
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(45, 0xE1, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(46, 0xAC, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }     
        result = AIC3254_Write(48, 0x80, hi2c); // Filter 2 BQ  N1--> 0x801E54
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(49, 0x1E, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(50, 0x54, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }      
        result = AIC3254_Write(52, 0x7F, hi2c); // Filter 2 BQ  N2--> 0x7FE1AC
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(53, 0xE1, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(54, 0xAC, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
        result = AIC3254_Write(56, 0x7F, hi2c); // Filter 2 BQ  D1--> 0x7FE1A9
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(57, 0xE1, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(58, 0xA9, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        } 
        result = AIC3254_Write(60, 0x80, hi2c); // Filter 2 BQ  D2--> 0x803C9E
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(61, 0x3C, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        result = AIC3254_Write(62, 0x9E, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }                
		} // if (sampRatePb==SAMP_RATE_96KHZ)
//////////////////// End of coefficient load //////////////

//////////////////////////////////////////////////////////
// 		Signal Processing Settings
//		Page 0
///////////////////////////////////////////////////////////
        result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(61, 2, hi2c); // PRB_R2: Stereo CH, A Decimation Filter 1st Order IIR and 5 BiQuads
        //result = AIC3254_Write(61, 8, hi2c); // PRB_R8: Stereo CH, B Decimation Filter 1st Order IIR and 5 BiQuads
        if (result != PSP_SOK) 
        {
            return result;
        }

        /* Select the PLL input and CODEC_CLKIN */
        /* PLL input is assumed as 12MHz */
        result = AIC3254_Write(4, 0x03, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        /*
          PLL_CLK = (PLL_CLKIN * R * J.D)/P
          DAC_FS = PLL_CLK/(NDAC * MDAC * DOSR)
          ADC_FS = PLL_CLK/(NADC * MADC * AOSR)
          DAC_CLK = PLL_CLK/NDAC
          BCLK = DAC_CLK/BCLK N

          DAC_FS, BCLK:
              96 kHz: P=1, R=1, J=8, D=1920 (0x780)
                      NDAC=4, MDAC=4, DOSR=64
                      PLL_CLK = (12e6 * 1 * 8.192)/1 = 98304000
                      BCLK = PLL_CLK/(NDAC*BCLK N) = 98304000/(4*4) = 6144000
                      DAC_FS = PLL_CLK/(NDAC*MDAC*DOSR) = 98304000/(4*4*64) = 96000
              48 kHz: P=1, R=1, J=8, D=1920 (0x780)
                      NDAC=4, MDAC=4, DOSR=128
                      PLL_CLK = (12e6 * 1 * 8.192)/1 = 98304000
                      BCLK = PLL_CLK/(NDAC*MDAC) = 98304000/(4*4) = 6144000
                      DAC_FS = PLL_CLK/(NDAC*MDAC*DOSR) = 98304000/(4*4*128) = 48000
              88.2 kHz: P=1, R=1, J=7, D=5264 (0x1490)
                      NDAC=4, MDAC=4, DOSR=64
                      PLL_CLK = (12e6 * 1 * 7.5264)/1 = 90316800
                      BCLK = PLL_CLK/(NDAC*MDAC) = 90316800/(4*4) = 5644800
                      DAC_FS = PLL_CLK/(NDAC*MDAC*DOSR) = 90316800/(4*4*64) = 88200
              44.1 kHz: P=1, R=1, J=7, D=5264 (0x1490)
                      NDAC=4, MDAC=4, DOSR=128
                      PLL_CLK = (12e6 * 1 * 7.5264)/1 = 90316800
                      BCLK = PLL_CLK/(NDAC*MDAC) = 90316800/(4*4) = 5644800
                      DAC_FS = PLL_CLK/(NDAC*MDAC*DOSR) = 90316800/(4*4*128) = 44100
          ADC_FS:
              96 kHz: P=1, R=1, J=8, D=1920 (0x780)
                      NADC=4, MADC=4, AOSR=64
                      BCLK = PLL_CLK/(NADC*MADC) = 98304000/(4*4) = 6144000
                      ADC_FS = PLL_CLK/(NADC*MADC*AOSR) = 98304000/(4*4*64) = 96000
              48 kHz: P=1, R=1, J=8, D=1920 (0x780)
                      NADC=4, MADC=4, AOSR=128
                      BCLK = PLL_CLK/(NADC*MADC) = 98304000/(4*4) = 6144000
                      ADC_FS = PLL_CLK/(NADC*MADC*AOSR) = 98304000/(4*4*128) = 48000
              88.2 kHz: P=1, R=1, J=7, D=5264 (0x1490)
                      NADC=4, MADC=4, AOSR=64
                      PLL_CLK = (12e6 * 1 * 7.5264)/1 = 90316800
                      BCLK = PLL_CLK/(NADC*MADC) = 90316800/(4*4) = 5644800
                      DAC_FS = PLL_CLK/(NADC*MADC*AOSR) = 90316800/(4*4*64) = 88200
              44.1 kHz: P=1, R=1, J=7, D=5264 (0x1490)
                      NADC=4, MADC=4, AOSR=128
                      PLL_CLK = (12e6 * 1 * 7.5264)/1 = 90316800
                      BCLK = PLL_CLK/(NADC*MADC) = 90316800/(4*4) = 5644800
                      DAC_FS = PLL_CLK/(NADC*MADC*AOSR) = 90316800/(4*4*128) = 44100
        */

        //
        // CODEC_CLKIN = 12MHz *(R * J.D)/P
        //
        // BCLK = (12MHz *(R * J.D)/P)/(NDAC * BCLK N)
        //
		if (sampRatePb==SAMP_RATE_96KHZ)
		{
			// Power up the PLL and set P = 1 & R = 1
			result = AIC3254_Write(5, 0x91, hi2c); // 96khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
			// Set J value to 8
			result = AIC3254_Write(6, 0x08, hi2c); // 96khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
			// Set D value(MSB) = 0x7
			result = AIC3254_Write(7, 0x7, hi2c); // 96khz
			  if (result != PSP_SOK) 
			{
			    return result;
			}
			// Set D value(LSB) = 0x80
			result = AIC3254_Write(8, 0x80, hi2c); // 96khz ; 0x780 => .1920 => D = 8.192 => DAC_FS = 96000
			if (result != PSP_SOK) 
			{
				return result;
			}

			// Set NDAC to 4 - this along with BCLK N configures BCLK
			result = AIC3254_Write(11,0x84, hi2c); // 96khz
			if (result != PSP_SOK) 
			{
				return result;
			}
			// Set MDAC to 4
			result = AIC3254_Write(12,0x84, hi2c); // 96khz
			if (result != PSP_SOK) 
			{
				return result;
			}
			/* Set DAC OSR MSB value to 0 */
			result = AIC3254_Write(13, 0x0, hi2c );
			if (result != PSP_SOK) 
			{
				return result;
			}
	        // Set DAC OSR LSB value to 64
	        // This generates the DAC_FS = 96KHz
	        result = AIC3254_Write(14, 64, hi2c ); // 96khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
			// Set BCLK N value to 4
			// This along with NDAC generates the  BCLK = 6.144 MHz
			result = AIC3254_Write(30,0x84, hi2c); // 96khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
        } // if (sampRatePb==SAMP_RATE_96KHZ)
		else if (sampRatePb==SAMP_RATE_48KHZ)
        {
	        // Power up the PLL and set P = 1 & R = 1
	        result = AIC3254_Write(5, 0x91, hi2c); // 48khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
				// Set J value to 8
				result = AIC3254_Write(6, 0x08, hi2c); // 48khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
			// Set D value(MSB) = 0x7
			result = AIC3254_Write(7, 0x7, hi2c); // 48khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
	        // Set D value(LSB) = 0x80
	        result = AIC3254_Write(8, 0x80, hi2c); // 48khz ; 0x780 => .1920 => D = 8.192 => DAC_FS = 48000
			if (result != PSP_SOK) 
			{
			    return result;
			}

			// Set NDAC to 4 - this along with BCLK N configures BCLK
			result = AIC3254_Write(11,0x84, hi2c); // 48khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
			// Set MDAC to 4
			result = AIC3254_Write(12,0x84, hi2c); // 48khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
			/* Set DAC OSR MSB value to 0 */
			result = AIC3254_Write(13, 0x0, hi2c );
			if (result != PSP_SOK) 
			{
			    return result;
			}
			// Set DAC OSR LSB value to 128
			// This generates the DAC_FS = 48KHz
			result = AIC3254_Write(14, 128, hi2c ); // 48khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
			// Set BCLK N value to 8
			// This along with NDAC generates the  BCLK = 6.144 MHz
			result = AIC3254_Write(30,0x84, hi2c); // 48khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
        } // if (sampRatePb==SAMP_RATE_48KHZ)
		else if (sampRatePb==SAMP_RATE_88_2KHZ)
		{
			/* Power up the PLL and set P = 1 & R = 1 */
			result = AIC3254_Write(5, 0x91, hi2c); // 88.2khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
			/* Set J value to 7 */
			result = AIC3254_Write(6, 0x07, hi2c); // 88.2khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
			/* Set D value(MSB) = 0x14 */
			result = AIC3254_Write(7, 0x14, hi2c); // 88.2khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
			/* Set D value(LSB) = 0x90 */
			result = AIC3254_Write(8, 0x90, hi2c); // 88.2khz ; 0x1490 => .5264 => D = 7.5264 => DAC_FS = 88200
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }

			/* Set NDAC to 4 */
			result = AIC3254_Write(11,0x84, hi2c);
			if (result != PSP_SOK) 
			{
			    return result;
			}
			/* Set MDAC to 4 */
			result = AIC3254_Write(12,0x84, hi2c);
			if (result != PSP_SOK) 
			{
			    return result;
			}
			/* Set DAC OSR MSB value to 0 */
			result = AIC3254_Write(13, 0x0, hi2c );
			if (result != PSP_SOK) 
			{
			    return result;
			}
			/* Set DAC OSR LSB value to 64 */
			/* This generates the DAC_FS = 88.2KHz */
			result = AIC3254_Write(14, 64, hi2c );
			if (result != PSP_SOK) 
			{
			    return result;
			}
			/* Set BCLK N value to4 */
			/* This generates the  BCLK = 5.6448MHz */
			result = AIC3254_Write(30,0x84, hi2c); // 88.2khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
		} // if (sampRatePb==SAMP_RATE_88_2KHZ)
		else if (sampRatePb==SAMP_RATE_44_1KHZ)
		{
			/* Power up the PLL and set P = 1 & R = 1 */
			result = AIC3254_Write(5, 0x91, hi2c); // 44.1khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
			/* Set J value to 7 */
			result = AIC3254_Write(6, 0x07, hi2c); //44.1khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
			/* Set D value(MSB) = 0x14 */
			result = AIC3254_Write(7, 0x14, hi2c);
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
			/* Set D value(LSB) = 0x90 */
			result = AIC3254_Write(8, 0x90, hi2c); // 44.1khz ; 0x1490 => .5264 => D = 7.5264 => DAC_FS = 44100
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }

			/* Set NDAC to 4 */
			result = AIC3254_Write(11,0x84, hi2c);
			if (result != PSP_SOK) 
			{
			    return result;
			}
			/* Set MDAC to 4 */
			result = AIC3254_Write(12,0x84, hi2c);
			if (result != PSP_SOK) 
			{
			    return result;
			}
			/* Set DAC OSR MSB value to 0 */
			result = AIC3254_Write(13, 0x0, hi2c );
			if (result != PSP_SOK) 
			{
			    return result;
			}
			/* Set DAC OSR LSB value to 128 */
			/* This generates the DAC_FS = 44.1KHz */
			result = AIC3254_Write(14, 128, hi2c );
			if (result != PSP_SOK) 
			{
			    return result;
			}
			/* Set BCLK N value to 8 */
			/* This generates the  BCLK = 5.6448MHz */
			result = AIC3254_Write(30,0x84, hi2c); // 44.1khz
			if (result != PSP_SOK) 
			{
			    return result;
			}
		} // if (sampRatePb==SAMP_RATE_44_1KHZ)
		else
		{
			LOG_printf(&trace, "PLAYBACK FORMAT IS NOT SUPPORTED\n");
		}
#ifdef ENABLE_RECORD
		if (sampRateRec==SAMP_RATE_96KHZ)
		{
	        //
	        // Set ADC_FS to 96 kHZ
	        //

	        // Set NADC to 4
	        result = AIC3254_Write(18,0x84, hi2c); // 96khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }

	        // Set MADC to 4
	        result = AIC3254_Write(19,0x84, hi2c); // 96khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }

	        // Set ADC OSR LSB value to 64
	        // This generates the ADC_FS = 96KHz
	        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
	        result = AIC3254_Write(20, 64, hi2c ); // 96khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
		} // if (sampRateRec==SAMP_RATE_96KHZ) 
		else if (sampRateRec==SAMP_RATE_48KHZ)
		{
	        //
	        // Set ADC_FS to 48 kHZ
	        //

	        // Set NADC to 4
	        result = AIC3254_Write(18,0x84, hi2c); // 48khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }

	        // Set MADC to 4
	        result = AIC3254_Write(19,0x84, hi2c); // 48khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }

	        // Set ADC OSR LSB value to 128
	        // This generates the ADC_FS = 48KHz
	        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
	        result = AIC3254_Write(20, 128, hi2c ); // 48khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
		} // if (sampRateRec==SAMP_RATE_48KHZ) 
		else if (sampRateRec==SAMP_RATE_88_2KHZ)
		{
	        //
	        // Set ADC_FS to 88.2 kHZ
	        //

	        // Set NADC to 4
	        result = AIC3254_Write(18,0x84, hi2c); // 88.2khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }

	        // Set MADC to 4
	        result = AIC3254_Write(19,0x84, hi2c); // 88.2khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }

	        // Set ADC OSR LSB value to 64
	        // This generates the ADC_FS = 88.2khz
	        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
	        result = AIC3254_Write(20, 64, hi2c ); // 88.2khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
        } // if (sampRateRec==SAMP_RATE_88_2KHZ) 
		else if (sampRateRec==SAMP_RATE_44_1KHZ)
		{
	        //
	        // Set ADC_FS to 44.1 kHZ
	        //

	        // Set NADC to 4
	        result = AIC3254_Write(18,0x84, hi2c); // 44.1khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }

	        // Set MADC to 4
	        result = AIC3254_Write(19,0x84, hi2c); // 44.1khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }

	        // Set ADC OSR LSB value to 128
	        // This generates the ADC_FS = 44.1KHz
	        // ADC_FS = (12MHz *(R * J.D)/P)/(NADC * MADC * AOSR)
	        result = AIC3254_Write(20, 128, hi2c ); // 44.1khz
	        if (result != PSP_SOK) 
	        {
	            return result;
	        }
        } // if (sampRateRec==SAMP_RATE_44_1KHZ) 
        else
		{
        	LOG_printf(&trace, "RECORD FORMAT IS NOT SUPPORTED\n");
		}
#endif // ENABLE_RECORD

#ifdef SAMP_24BIT
        result = AIC3254_Write(27, 0x2d, hi2c); // BCLK and WCLK is set as op to AIC3254(Master)
#else
        result = AIC3254_Write(27, 0xd, hi2c); // BCLK and WCLK is set as op to AIC3254(Master)
#endif
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0,1,hi2c);// select page 1
        if (result != PSP_SOK) 
        {
            return result;
        }

#ifdef C5535_EZDSP        
		// power up Mic Bias using LDO-IN
        result = AIC3254_Write(51,0x48,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
#endif

        result = AIC3254_Write(0x1,0x8,hi2c);// Disable crude AVDD generation from DVDD
        if (result != PSP_SOK) 
        {
            return result;
        }

#ifdef C5535_EZDSP        
		result = AIC3254_Write(0x2,1,hi2c);// Enable Analog Blocks and internal LDO
#else
        result = AIC3254_Write(0x2,0,hi2c);// Enable Analog Blocks
#endif

        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x14,32,hi2c);// Depop reg R=6K,t=8RC(2.256ms),ramp time=0ms
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0xc,0x8,hi2c);// LDAC AFIR routed to HPL
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0xd,0x8,hi2c);// RDAC AFIR routed to HPR
        if (result != PSP_SOK) 
        {
            return result;
        }

#ifdef ENABLE_RECORD
        //Route IN2L to LEFT_P with 40K input impedance
        result = AIC3254_Write(52,0x30,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Route Common Mode to LEFT_M with impedance of 40K
        result = AIC3254_Write(54,0xc0,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Route IN2R to RIGHT_P with 40K input impedance
        result = AIC3254_Write(55,0x30,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Route Common Mode to RIGHT_M with impedance of 40K
        result = AIC3254_Write(57,0xc0,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Unmute Left MICPGA
        result = AIC3254_Write(59,0x0f,hi2c); // Gain = 7.5 dB
        //result = AIC3254_Write(59,0x1e,hi2c); // Gain = 15 dB
        //result = AIC3254_Write(59,0x3c,hi2c); // Gain = 30 dB
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Unmute Right MICPGA
        result = AIC3254_Write(60,0x0f,hi2c); // Gain = 7.5 dB
        //result = AIC3254_Write(60,0x1e,hi2c); // Gain = 15 dB
        //result = AIC3254_Write(60,0x3c,hi2c); // Gain = 30 dB
        if (result != PSP_SOK) 
        {
            return result;
        }
        #endif // ENABLE_RECORD

        result = AIC3254_Write(0, 0, hi2c); // write 0 to page register to select page 0
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(64,0x2,hi2c); // left vol=right vol
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(63,0xd4, hi2c); // power up left,right data paths and set channel
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(64,0xc,hi2c); // left vol=right vol; muted
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0,1,hi2c);// select page 1
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x10,0,hi2c);// unmute HPL , 0dB gain
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x11,0,hi2c);// unmute HPR , 0dB gain
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x9,0x30,hi2c);// power up HPL,HPR
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x0,0x0,hi2c);// select page 0
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x40,0x2,hi2c);// unmute DAC with right vol=left vol
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(65,/*20 48*/ 0,hi2c);// set DAC gain to 0dB
        if (result != PSP_SOK) 
        {
            return result;
        }

        #ifdef ENABLE_RECORD
        //Powerup left and right ADC
        result = AIC3254_Write(81,0xc0,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }

        //Unmute left and right ADC
        result = AIC3254_Write(82,0x00,hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
        #endif // ENABLE_RECORD

        result = AIC3254_Write(0x0,0x1,hi2c);// select page 1
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x10,0,hi2c);// unmute HPL , 0dB gain
        if (result != PSP_SOK) 
        {
            return result;
        }

        result = AIC3254_Write(0x11,0,hi2c);// unmute HPR , 0dB gain
        if (result != PSP_SOK) 
        {
            return result;
        }

#if 0 // debug
       // route ADC_FS to WCLK (I2S FS)
        result = AIC3254_Write(33, 0x10, hi2c);
        if (result != PSP_SOK) 
        {
            return result;
        }
#endif

        // write 1 to page register to select page 1 - prepare for next headset volume change
        result = AIC3254_Write(0, 1, hi2c);
        if (result != PSP_SOK) 
        {
            return FALSE;
        }
        return result;
  }
  else
  {
        //printf("\n I2C init error \n");
        LOG_printf(&trace, "\n I2C init error \n");
        return PSP_E_DRIVER_INIT;
  }

}

/**
 *  \brief Codec write function
 *
 *  Function to write a byte of data to a codec register.
 *
 *  \param regAddr  [IN]  Address of the register to write the data
 *  \param regData  [IN]  Data to write into the register
 *
 *  \return PSP_SOK - if successful, else suitable error code
 */
PSP_Result AIC3254_Write(Uint16 regAddr, Uint16 regData, PSP_Handle hi2c)
{
    PSP_Result    status;
    Uint16        writeCount;
    Uint16        writeBuff[2];

    status = PSP_E_INVAL_PARAM;

    //if(hi2c != NULL)
    {
        writeCount  =  2;
        /* Initialize the buffer          */
        /* First byte is Register Address */
        /* Second byte is register data   */
        writeBuff[0] = (regAddr & 0x00FF);
        writeBuff[1] = (regData & 0x00FF);

        /* Write the data */
        status = I2C_Write(hi2c, I2C_CODEC_ADDR, writeCount, writeBuff);
    }

    return status;
}

/**
 *  \brief Codec read function
 *
 *  Function to read a byte of data from a codec register.
 *
 *  \param regAddr  [IN]  Address of the register to read the data
 *  \param data     [IN]  Pointer to the data read from codec register
 *
 *  \return PSP_SOK - if successful, else suitable error code
 */
PSP_Result AIC3254_Read(Uint16 regAddr, Uint16 *data, PSP_Handle  hi2c)
{
    PSP_Result status  = PSP_E_INVAL_PARAM;
    Uint16 readCount = 1;
    Uint16 readBuff[1];

    regAddr = (regAddr & 0x00FF);

   if(hi2c)
     status = I2C_Read(hi2c,
                    I2C_CODEC_ADDR,
                    readCount,
                    regAddr,
                    readBuff);

    if(status == PSP_SOK)
     *data = readBuff[1];

    return status;
}


Uint16   codec_Ioctl(Uint16 regAddr, Uint16 regData, PSP_Handle hi2c)
{
    return PSP_SOK;
}
