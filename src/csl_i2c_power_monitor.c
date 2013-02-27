//////////////////////////////////////////////////////////////////////////////
// * File name: csl_i2c_power_monitor.c
// *                                                                          
// * Description: This file includes main() and system initialization funcitons.
// *                                                                          
// * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
// *                                                                          
// *                                                                          
// *  Redistribution and use in source and binary forms, with or without      
// *  modification, are permitted provided that the following conditions      
// *  are met:                                                                
// *                                                                          
// *    Redistributions of source code must retain the above copyright        
// *    notice, this list of conditions and the following disclaimer.         
// *                                                                          
// *    Redistributions in binary form must reproduce the above copyright     
// *    notice, this list of conditions and the following disclaimer in the   
// *    documentation and/or other materials provided with the                
// *    distribution.                                                         
// *                                                                          
// *    Neither the name of Texas Instruments Incorporated nor the names of   
// *    its contributors may be used to endorse or promote products derived   
// *    from this software without specific prior written permission.         
// *                                                                          
// *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     
// *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       
// *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   
// *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT    
// *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   
// *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        
// *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
// *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   
// *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     
// *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   
// *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
// *                                                                          
//////////////////////////////////////////////////////////////////////////////

#include "csl_pll.h"
#include "csl_pllAux.h"
#include <csl_i2c.h>
#include <stdio.h>
#include <csl_general.h>
#include <csl_rtc.h>
#include <csl_intc.h>

#include "app_globals.h"
#include "psp_i2s.h"
#include "psp_i2c.h"
#include "dda_i2c.h"
#include "psp_common.h"
#include "lcd_osd.h"
#include "string.h"
#include "dsplib.h"

#include "VC5505_CSL_BIOS_cfg.h"

#ifdef C5535_EZDSP_DEMO

#define CSL_I2C_TEST_PASSED      (0)
#define CSL_I2C_TEST_FAILED      (1)

#define CSL_I2C_DATA_SIZE        (64)
#define CSL_EEPROM_ADDR_SIZE     (2)
#define CSL_I2C_OWN_ADDR         (0x2F)

#define CSL_I2C_SYS_CLK          	(100)
#define CSL_I2C_BUS_FREQ         	(10)
#define CSL_I2C_EEPROM_ADDR		 	(0x50)

// Power monitoring by AN219
#define CSL_I2C_CODEC_ADDR		 	(0x18)
#define CSL_I2C_POWER_CORE_ADDR  	(0x40)
#define CSL_I2C_POWER_IO2_ADDR   	(0x42)
#define CSL_I2C_POWER_USB1p3_ADDR  	(0x43)
#define CSL_I2C_POWER_IO1_ADDR  	(0x44)
#define CSL_I2C_POWER_EVM3p3_ADDR  	(0x45)
#define CSL_I2C_POWER_EVM1p8_ADDR  	(0x46)
#define CSL_I2C_POWER_EVMVIN_ADDR  	(0x47)

#define RTC_CLK						(32768)  // HZ

//#define EXT_CLK						(12000)	// KHz

void mysprintf(char *buf, char *str1, Uint32 num, char *str2);

PLL_Config pllCfgPM;
extern PLL_Handle hPll;

Uint16  gI2cWrBuf[CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE];
Uint16  gI2cRdBuf[CSL_I2C_DATA_SIZE];

Uint16  gPowerReg[12];

CSL_I2cSetup     i2cSetup;
CSL_I2cConfig    i2cConfig;

char Freq[7], Core[7], IO1[7], POWER[7];

extern DATA bufferFFT[512];
extern DATA bufferScrach[512];
extern int display_buffer[128];

extern Uint16 banner_sel, draw_banner, freq_sel, chan_freq;

/**
 *  \brief  Tests I2C Power monitor operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_i2cPowerTest(void);

/**
 *  \brief  Tests I2C Power test
 *
 *  \param  none
 *
 *  \return Test result
 */
char	tempString1[40] = "1234567890";
char	tempString2[40] = "ABCDEFG";
CSL_Status  CSL_i2cPowerTest(void)
{
	CSL_Status         status;
	CSL_Status         result;
	Uint16             i;
	volatile Uint16    looper;
	Uint32				core_i, core_power, core_v, freq;
	Uint32				PLL_FB, PLL_OD;
	
	result = CSL_I2C_TEST_FAILED;
	
	i=1;
	gI2cWrBuf[0]= i;
	status = I2C_Write(hi2c, CSL_I2C_POWER_CORE_ADDR, 1, gI2cWrBuf);
	if(status != CSL_SOK)
	{
		printf("I2C Write Failed!!\n");
		return(result);
	}
	
	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){asm("	nop");}
					
	status = PLL_getConfig(hPll, &pllCfgPM);
	if(status != CSL_SOK)
	{
		printf("TEST FAILED: PLL get config... Failed.\n");
		printf ("Reason: PLL_getConfig failed. [status = 0x%x].\n", status);
		return(result);
	}
	
	PLL_FB = pllCfgPM.PLLCNTL1 & 0x0FFF;
	///MWPLL_RD = pllCfgPM.PLLINCNTL & 0x0FFF;
	
	freq = PLL_FB * RTC_CLK;
	if (pllCfgPM.PLLOUTCNTL & 0x200) 
	{
		PLL_OD = pllCfgPM.PLLOUTCNTL & 0xFF;
		freq = freq / (PLL_OD+1);
	}
	freq /=1000; //KHZ

	/* Reading current and voltage */
	for (i=1; i<3; i++) 
	{
		/* Read data */
		status = I2C_Read(hi2c, CSL_I2C_POWER_CORE_ADDR, 2, i, &gPowerReg[i*2]);
		if(status != CSL_SOK)
		{
			printf("I2C Read Failed!!\n");
			return(result);
		}
		
		/* Give some delay */
		for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){asm("	nop");}
	}
	
	core_i= ((gPowerReg[2]  << 8) + gPowerReg[3])*10;
	
	core_v= (((gPowerReg[4]  << 8) + gPowerReg[5])>>3)*4;
	core_power = (core_v * core_i)/freq ; // power/MHz;
	//printf("Power: %2.2fmW/Mhz\n\n", core_power);
	mysprintf(tempString1, "Freq: ", freq, " Mhz");
	mysprintf(tempString2, "Pwr: ", core_power, " mW/Mhz");
	print_two(tempString1, tempString2);

	return 0;
}

void mysprintf(char *buf, char *str1, Uint32 num, char *str2)
{
	Uint32 temp;
	Uint16 len, digit, start;
		
	temp = num;
	len = strlen(str1);
	start = 0;
	// copy the first string into buffer
	strcpy(buf, str1);
	// the number input is 1000 times of the original number
	// do we have hundred's digit
	digit = temp/100000;
	if (digit)
	{
		buf[len] = digit+'0';
		len++;
		start = 1;
		temp = temp%100000;
	} else
	{
		if (start)
		{
			buf[len] = '0';
			len++;
		}
		temp = temp%100000;
	}
	// do we have ten's digit
	digit = temp/10000;
	if (digit)
	{
		buf[len] = digit+'0';
		len++;
		start = 1;
		temp = temp%10000;
	} else
	{
		if (start)
		{
			buf[len] = '0';
			len++;
		}
		temp = temp%10000;
	}
	// do we have one's digit
	digit = temp/1000;
	if (digit)
	{
		buf[len] = digit+'0';
		len++;
		start = 1;
		temp = temp%1000;
	} else
	{
		buf[len] = '0';
		len++;
		temp = temp%1000;
	}
	
	// put decimal point
	buf[len] = '.';
	len++;
	
	// do we have tenth digit
	digit = temp/100;
	if (digit)
	{
		buf[len] = digit+'0';
		len++;
		temp = temp%100;
	} else
	{
		buf[len] = '0';
		len++;
		temp = temp%100;
	}

	// do we have hundredth digit
	digit = temp/10;
	if (temp)
	{
		buf[len] = digit+'0';
		len++;
		buf[len] = 0;
	} else
	{
		buf[len] = '0';
		len++;
		buf[len] = 0;
	}

	// copy the second string into buffer
	strcat(buf, str2);
	
	return;
}

/*****************************************************************************/
/* calculate_power()                                                         */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Parameter 1: Real term a.                                                 */
/* Parameter 2: Immaginary term jb.                                          */
/*                                                                           */
/* RETURNS: a*a + b*b. Result will always be positive.                       */
/*                                                                           */
/*****************************************************************************/

int calculate_power (int a, int b)
{

 return ( (int) ( ( (long)a * a + (long) b * b) >> 11) );
}

/*****************************************************************************/
/* calculate_FFT()                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Parameter 1: Latest audio input (real value).                             */
/* Parameter 2: size of FFT e.g. 128, 512 and 1024 elements.                 */
/*                                                                           */
/* RETURNS: None.                                                            */
/*                                                                           */
/*****************************************************************************/

void calculate_FFT(int *input, int size)
{
	int i, j;
	Uint16 out_sel;

	for (i=0; i<size; i++)
	{
 		bufferFFT[i*2] = input[i]; /* Store as a real value */
		bufferFFT[i*2+1] = 0;     /* Store with an imaginary value of 0 */
	}
	 
	/* Perform complex FFT using N real and N imaginary values */     
#if 1
	cfft (&bufferFFT[0], size, SCALE);           
	cbrev(&bufferFFT[0], &bufferFFT[0], size);  
	out_sel = 1;   
#else
    hwafft_br((Int32 *)bufferFFT, (Int32 *)bufferScrach, 256);  /* bit-reverse input data */
    out_sel = hwafft_256pts((Int32 *)bufferScrach, (Int32 *)bufferFFT, 0, 1); /* perform FFT */
#endif

	// construct the bargraph
    for ( j = 0 ; j < 128 ; j ++)
    {
    	// display first 96 bins out of 128 bins (256/2)
    	if (out_sel)
    		display_buffer[j] = calculate_power((int) bufferFFT[2*j], (int)bufferFFT[2*j+1])+8;
    	else
    		display_buffer[j] = calculate_power((int) bufferScrach[2*j], (int)bufferScrach[2*j+1])+8;
	}
    						
    // display the bargraph
    oled_display_bargraph( &display_buffer[0]);     
}
#endif

