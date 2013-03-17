/*
 * $$$MODULE_NAME csl_usb_iso_fullspeed_example.c
 *
 * $$$MODULE_DESC csl_usb_iso_fullspeed_example.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** @file csl_usb_iso_fullspeed_example.c
 *
 *  @brief USB Audio Class functional layer full speed mode example source file
 *
 *  This example tests the operation of VC5505 usb in full speed mode.
 *  NOTE: For Testing Audio class module a macro CSL_AC_TEST needs to be defined
 *  This includes some code in csl_usbAux.h file which is essential for Audio class
 *  operation and not required for MUSB stand alone testing.
 *  define this macro in pre defined symbols in project biuld options
 *  (Defined in the current usb audio class example pjt).
 *  Semaphores and mail boxes are used in the Audio class example code as the USB operation
 *  is not possible with out OS calls. DSP BIOS version 5.32.03 is used for this purpose.
 *  Definig Start transfer and complete transfer call back functions is must
 *  and Audio class module does not work if they are not implemeted properly. A call back
 *  is sent to this functions from MUSB module.
 *
 *  NOTE: Message boxes and semaphores are reused from MSC module.
 *  Name MSC is not replaced with Auidio class at some places for quick reusability
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5505 AND
 * C5515. MAKE SURE THAT PROPER CHIP VERSION MACRO CHIP_5505/CHIP_5515 IS
 * DEFINED IN THE FILE c55xx_csl\inc\csl_general.h.
 *
 *  Path: \(CSLPATH)\example\usb\example5
 */

/* ============================================================================
 * Revision History
 * ================
 * 20-Dec-2008 Created
 * ============================================================================
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


#include "csl_types.h"
#include "csl_error.h"
#include "csl_intc.h"
#include "csl_gpio.h"
#include "csl_usb.h"
#include "csl_audioClass.h"
#include "soc.h"
#include "dda_dma.h"
#include "i2s_sample.h"
#include "gpio_control.h"
#include "pll_control.h"
#include "app_globals.h"
#include "app_usb.h"
#include "app_usbac.h"
#include "app_usbac_descs.h"
#include "codec_aic3254.h"
#include "user_interface.h"
#include "psp_i2s.h"
#include "app_asrc.h"
#include "sample_rate.h"

#ifdef C5535_EZDSP_DEMO
#include "lcd_osd.h"
#include "ezdsp5535_lcd.h"
#include "dsplib.h"
#include "soc.h"
#include "cslr.h"
#include "cslr_sysctrl.h"
#include "ezdsp5535.h"
#include "ezdsp5535_led.h"
#include "sem.h"
SEM_Obj SEM_BufferInReady;

CSL_Status  CSL_i2cPowerTest(void);
void calculate_FFT(int *input, int size);
// buffer for data collection from USB Audio playback
int bufferIn[256];
// index for bufferIn
int bufferInIdx = 0;
// buffer for perform FFT
#pragma DATA_ALIGN(bufferFFT, 4)
DATA bufferFFT[512];
// scarch buffer for FFT
DATA bufferScrach[512];
// display buffer for spectrum display
int display_buffer[128];
// Demo switch flag: 0 - power display, 1 - spectrum analyzer
Uint16 DemoSwitchFlag = 1;
#endif

#include "VC5505_CSL_BIOS_cfg.h"

#define DISABLE_BIOS_TRACE
//#define ENABLE_I2S_INT
#define ENABLE_DMA_INT

 /* Debug: enable run-time storage of data to SDRAM */
//#define STORE_PARAMETERS_TO_SDRAM

// Clock gating for unused peripherals
void ClockGating(void);

/* Initializes application */
void CSL_acTest(void);

/* Resets C5515 */
void C5515_reset(void);

void SYS_GlobalIntEnable(void);
void SYS_GlobalIntDisable(void);


/**
 *  \brief  CSL Audio Class main function
 *
 *  \param  None
 *
 *  \return None
 */
void main(void)
{
    CSL_Status status;
    Uint32 gpioIoDir;
    int i;

#ifdef DISABLE_BIOS_TRACE
    // Disable trace to reduce MHz load
    TRC_disable(TRC_GBLTARG);
    // disable the message log
    LOG_disable(&trace);
#endif

    /* Clock gate all peripherals */
    CSL_SYSCTRL_REGS->PCGCR1 = 0x7FFF;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x007F;

    /* Configure EBSR */
#if defined(USE_I2S0_PB) || defined(USE_I2S0_REC)
    /* SP0 Mode 1 (I2S0 and GP[5:4]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE1);
    /* SP0 Mode 2 (GP[5:0]) -- GPIO02/GPIO04 for debug  */
    //CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE2);
#else
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE1);
#endif
#if defined(USE_I2S1_PB) || defined(USE_I2S1_REC)
    /* SP1 Mode 1 (I2S1 and GP[11:10]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE1);
#else
#ifdef USE_FOUR_CODEC
    /* SP1 Mode 1 (I2S1 and GP[11:10]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE1);
#else
#ifdef USE_THREE_CODEC
    /* SP1 Mode 1 (I2S1 and GP[11:10]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE1);
#else
#ifdef USE_TWO_CODEC
    /* SP1 Mode 1 (I2S1 and GP[11:10]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE1);
#else
    /* SP1 Mode 1 (GP[11:10]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE1);
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#endif //defined(USE_I2S1_PB) || defined(USE_I2S1_REC)
    /* PP Mode 1 (SPI, GPIO[17:12], UART, and I2S2) -- note this allows UART */
    //CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);
    /* PP Mode 6 (SPI, I2S2, I2S3 and GPIO) -- no UART */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE6);

    /* Reset C5515 -- ungates all peripherals */
    C5515_reset();

    /* Initialize DSP PLL */
    status = pll_sample();
    if (status != CSL_SOK)
    {
        LOG_printf(&trace, "ERROR: Unable to initialize PLL");
    }

    /* Clear pending timer interrupts */
    CSL_SYSCTRL_REGS->TIAFR = 0x7;

    /* Initialize GPIO module */
#if !defined(USE_I2S0_PB) && !defined(USE_I2S0_REC)
    /* GPIO02 and GPIO04 for debug */
    /* GPIO10 for AIC3204 reset */
    gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN2) | 
        (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN4) |
        (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN10);
#else
    /* GPIO10 for AIC3204 reset */
    gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN10)
#endif
    status = gpioInit(gpioIoDir, 0x00000000, 0x00000000);
    if (status != GPIOCTRL_SOK)
    {
        LOG_printf(&trace, "ERROR: Unable to initialize GPIO");
    }

    /* Enable the USB LDO */
    *(volatile ioport unsigned int *)(0x7004) |= 0x0001;

    EZDSP5535_init();
    EZDSP5535_LED_init( );
    for (i = 0; i < 4; i++) EZDSP5535_LED_off(i);
};

/**
 *  \brief  Audio Class intialization function
 *
 *  \param  None
 *
 *  \return None
 */
void CSL_acTest(void)
{
    I2sInitPrms i2sInitPrms;
    CSL_UsbConfig usbConfig;
    PSP_Result result;
    Int16 status;

    LOG_printf(&trace, "USB ISO HIGH SPEED MODE\n");

    /* Initialize audio module */
    result = AIC3254_init(SAMP_RATE_96KHZ, SAMP_RATE_96KHZ);
    if(result != 0)
    {
        LOG_printf(&trace, "ERROR: Unable to configure audio codec");
    }
    else
    {
		// enable the MPORT and disable HWA
		*(volatile ioport Uint16 *)0x0001 = 0x020E;
		asm("   idle");

        DMA_HwInit();
        DMA_DrvInit();

        /* Initialize I2S and associated DMA channels for Playback and Record */
        i2sInitPrms.enablePlayback = TRUE;
        i2sInitPrms.enableStereoPb = TRUE;
        i2sInitPrms.sampleBySamplePb = FALSE;
        i2sInitPrms.pingPongI2sTxLeftBuf = (Int16 *)ping_pong_i2sTxLeftBuf;
        i2sInitPrms.pingPongI2sTxRightBuf = (Int16 *)ping_pong_i2sTxRightBuf;
        i2sInitPrms.i2sPb = PSP_I2S_TX_INST_ID;
        i2sInitPrms.enableRecord = TRUE;
        i2sInitPrms.enableStereoRec = TRUE;
        i2sInitPrms.pingPongI2sRxLeftBuf = (Int16 *)ping_pong_i2sRxLeftBuf;
        i2sInitPrms.pingPongI2sRxRightBuf = (Int16 *)ping_pong_i2sRxRightBuf;
        i2sInitPrms.i2sRec = PSP_I2S_RX_INST_ID;
        status = i2sInit(&i2sInitPrms);
        if (status != I2SSAMPLE_SOK)
        {
            LOG_printf(&trace, "ERROR: Unable to initialize I2S");
        }

#ifdef USE_TWO_CODEC
        /* Initialize 2nd I2S and associated DMA channels for Playback and Record */
        i2sInitPrms.enablePlayback = TRUE;
        i2sInitPrms.enableStereoPb = TRUE;
        i2sInitPrms.sampleBySamplePb = FALSE;
        i2sInitPrms.pingPongI2sTxLeftBuf = (Int16 *)ping_pong_i2sTxLeftBuf2;
        i2sInitPrms.pingPongI2sTxRightBuf = (Int16 *)ping_pong_i2sTxRightBuf2;
        i2sInitPrms.i2sPb = PSP_I2S_TX_INST_ID_2;
        i2sInitPrms.enableRecord = TRUE;
        i2sInitPrms.enableStereoRec = TRUE;
        i2sInitPrms.pingPongI2sRxLeftBuf = (Int16 *)ping_pong_i2sRxLeftBuf2;
        i2sInitPrms.pingPongI2sRxRightBuf = (Int16 *)ping_pong_i2sRxRightBuf2;
        i2sInitPrms.i2sRec = PSP_I2S_RX_INST_ID_2;
        status = i2sInit2(&i2sInitPrms);
        if (status != I2SSAMPLE_SOK)
        {
            LOG_printf(&trace, "ERROR: Unable to initialize 2nd I2S");
        }
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
        /* Initialize 2nd I2S and associated DMA channels for Playback and Record */
        i2sInitPrms.enablePlayback = TRUE;
        i2sInitPrms.enableStereoPb = TRUE;
        i2sInitPrms.sampleBySamplePb = FALSE;
        i2sInitPrms.pingPongI2sTxLeftBuf = (Int16 *)ping_pong_i2sTxLeftBuf2;
        i2sInitPrms.pingPongI2sTxRightBuf = (Int16 *)ping_pong_i2sTxRightBuf2;
        i2sInitPrms.i2sPb = PSP_I2S_TX_INST_ID_2;
        i2sInitPrms.enableRecord = TRUE;
        i2sInitPrms.enableStereoRec = TRUE;
        i2sInitPrms.pingPongI2sRxLeftBuf = (Int16 *)ping_pong_i2sRxLeftBuf2;
        i2sInitPrms.pingPongI2sRxRightBuf = (Int16 *)ping_pong_i2sRxRightBuf2;
        i2sInitPrms.i2sRec = PSP_I2S_RX_INST_ID_2;
        status = i2sInit2(&i2sInitPrms);
        if (status != I2SSAMPLE_SOK)
        {
            LOG_printf(&trace, "ERROR: Unable to initialize 2nd I2S");
        }

        /* Initialize 3rd I2S and associated DMA channels for Playback and Record */
        i2sInitPrms.enablePlayback = TRUE;
        i2sInitPrms.enableStereoPb = TRUE;
        i2sInitPrms.sampleBySamplePb = FALSE;
        i2sInitPrms.pingPongI2sTxLeftBuf = (Int16 *)ping_pong_i2sTxLeftBuf3;
        i2sInitPrms.pingPongI2sTxRightBuf = (Int16 *)ping_pong_i2sTxRightBuf3;
        i2sInitPrms.i2sPb = PSP_I2S_TX_INST_ID_3;
        i2sInitPrms.enableRecord = TRUE;
        i2sInitPrms.enableStereoRec = TRUE;
        i2sInitPrms.pingPongI2sRxLeftBuf = (Int16 *)ping_pong_i2sRxLeftBuf3;
        i2sInitPrms.pingPongI2sRxRightBuf = (Int16 *)ping_pong_i2sRxRightBuf3;
        i2sInitPrms.i2sRec = PSP_I2S_RX_INST_ID_3;
        status = i2sInit3(&i2sInitPrms);
        if (status != I2SSAMPLE_SOK)
        {
            LOG_printf(&trace, "ERROR: Unable to initialize 3rd I2S");
        }
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
        /* Initialize 2nd I2S and associated DMA channels for Playback and Record */
        i2sInitPrms.enablePlayback = TRUE;
        i2sInitPrms.enableStereoPb = TRUE;
        i2sInitPrms.sampleBySamplePb = FALSE;
        i2sInitPrms.pingPongI2sTxLeftBuf = (Int16 *)ping_pong_i2sTxLeftBuf2;
        i2sInitPrms.pingPongI2sTxRightBuf = (Int16 *)ping_pong_i2sTxRightBuf2;
        i2sInitPrms.i2sPb = PSP_I2S_TX_INST_ID_2;
        i2sInitPrms.enableRecord = TRUE;
        i2sInitPrms.enableStereoRec = TRUE;
        i2sInitPrms.pingPongI2sRxLeftBuf = (Int16 *)ping_pong_i2sRxLeftBuf2;
        i2sInitPrms.pingPongI2sRxRightBuf = (Int16 *)ping_pong_i2sRxRightBuf2;
        i2sInitPrms.i2sRec = PSP_I2S_RX_INST_ID_2;
        status = i2sInit2(&i2sInitPrms);
        if (status != I2SSAMPLE_SOK)
        {
            LOG_printf(&trace, "ERROR: Unable to initialize 2nd I2S");
        }

        /* Initialize 3rd I2S and associated DMA channels for Playback and Record */
        i2sInitPrms.enablePlayback = TRUE;
        i2sInitPrms.enableStereoPb = TRUE;
        i2sInitPrms.sampleBySamplePb = FALSE;
        i2sInitPrms.pingPongI2sTxLeftBuf = (Int16 *)ping_pong_i2sTxLeftBuf3;
        i2sInitPrms.pingPongI2sTxRightBuf = (Int16 *)ping_pong_i2sTxRightBuf3;
        i2sInitPrms.i2sPb = PSP_I2S_TX_INST_ID_3;
        i2sInitPrms.enableRecord = TRUE;
        i2sInitPrms.enableStereoRec = TRUE;
        i2sInitPrms.pingPongI2sRxLeftBuf = (Int16 *)ping_pong_i2sRxLeftBuf3;
        i2sInitPrms.pingPongI2sRxRightBuf = (Int16 *)ping_pong_i2sRxRightBuf3;
        i2sInitPrms.i2sRec = PSP_I2S_RX_INST_ID_3;
        status = i2sInit3(&i2sInitPrms);
        if (status != I2SSAMPLE_SOK)
        {
            LOG_printf(&trace, "ERROR: Unable to initialize 3rd I2S");
        }

        /* Initialize 4th I2S and associated DMA channels for Playback and Record */
        i2sInitPrms.enablePlayback = TRUE;
        i2sInitPrms.enableStereoPb = TRUE;
        i2sInitPrms.sampleBySamplePb = FALSE;
        i2sInitPrms.pingPongI2sTxLeftBuf = (Int16 *)ping_pong_i2sTxLeftBuf4;
        i2sInitPrms.pingPongI2sTxRightBuf = (Int16 *)ping_pong_i2sTxRightBuf4;
        i2sInitPrms.i2sPb = PSP_I2S_TX_INST_ID_4;
        i2sInitPrms.enableRecord = TRUE;
        i2sInitPrms.enableStereoRec = TRUE;
        i2sInitPrms.pingPongI2sRxLeftBuf = (Int16 *)ping_pong_i2sRxLeftBuf4;
        i2sInitPrms.pingPongI2sRxRightBuf = (Int16 *)ping_pong_i2sRxRightBuf4;
        i2sInitPrms.i2sRec = PSP_I2S_RX_INST_ID_4;
        status = i2sInit4(&i2sInitPrms);
        if (status != I2SSAMPLE_SOK)
        {
            LOG_printf(&trace, "ERROR: Unable to initialize 4th I2S");
        }
#endif //USE_FOUR_CODEC

#ifdef C5535_EZDSP_DEMO
        // initialize the OLED display        
        //oled_init();
    	Int16 i;
    	Uint16 cmd[10];    // For multibyte commands
        EZDSP5535_OSD9616_init( );
        EZDSP5535_OSD9616_send(0x00,0x2e);  // Deactivate Scrolling


        /* Fill page 0 */
        EZDSP5535_OSD9616_send(0x00,0x00);   // Set low column address
        EZDSP5535_OSD9616_send(0x00,0x10);   // Set high column address
        EZDSP5535_OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5
        for(i=0;i<128;i++)
        {
        	EZDSP5535_OSD9616_send(0x40,0xff);
        }
        /* Write to page 0 */
        EZDSP5535_OSD9616_send(0x00,0x00);   // Set low column address
        EZDSP5535_OSD9616_send(0x00,0x10);   // Set high column address
        EZDSP5535_OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5
        for(i=0;i<22;i++)
        {
        	EZDSP5535_OSD9616_send(0x40,0x00);  // Spaces
        }
        EZDSP5535_OSD9616_printLetter(0x32,0x49,0x49,0x26);  // S
        EZDSP5535_OSD9616_printLetter(0x01,0x7F,0x01,0x01);  // T
        EZDSP5535_OSD9616_printLetter(0x7F,0x30,0x0E,0x7F);  // N
        EZDSP5535_OSD9616_printLetter(0x41,0x49,0x49,0x7F);  // E
        EZDSP5535_OSD9616_printLetter(0x7F,0x06,0x06,0x7F);  // M
        EZDSP5535_OSD9616_printLetter(0x3F,0x40,0x40,0x3F);  // U
        EZDSP5535_OSD9616_printLetter(0x46,0x29,0x19,0x7F);  // R
        EZDSP5535_OSD9616_printLetter(0x01,0x7F,0x01,0x01);  // T
        EZDSP5535_OSD9616_printLetter(0x32,0x49,0x49,0x26);  // S
        EZDSP5535_OSD9616_printLetter(0x7F,0x30,0x0E,0x7F);  // N
        EZDSP5535_OSD9616_printLetter(0x00,0x7F,0x00,0x00);  // I
        for(i=0;i<5;i++)
        {
        	EZDSP5535_OSD9616_send(0x40,0x00);  // Spaces
        }
        EZDSP5535_OSD9616_printLetter(0x32,0x49,0x49,0x26);  // S
        EZDSP5535_OSD9616_printLetter(0x7C,0x09,0x0A,0x7C);  // A
        EZDSP5535_OSD9616_printLetter(0x63,0x1C,0x1C,0x63);  // X
        EZDSP5535_OSD9616_printLetter(0x41,0x49,0x49,0x7F);  // E
        EZDSP5535_OSD9616_printLetter(0x01,0x7F,0x01,0x01);  // T

        for(i=0;i<23;i++)
        {
        	EZDSP5535_OSD9616_send(0x40,0x00);  // Spaces
        }
        /* Fill page 1*/
        EZDSP5535_OSD9616_send(0x00,0x00);   // Set low column address
        EZDSP5535_OSD9616_send(0x00,0x10);   // Set high column address
        EZDSP5535_OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5
        for(i=0;i<128;i++)
        {
        	EZDSP5535_OSD9616_send(0x40,0xff);
        }

        /* Write to page 1*/
        EZDSP5535_OSD9616_send(0x00,0x00);   // Set low column address
        EZDSP5535_OSD9616_send(0x00,0x10);   // Set high column address
        EZDSP5535_OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5
        for(i=0;i<46;i++)
        {
        	EZDSP5535_OSD9616_send(0x40,0x00);
        }

       	EZDSP5535_OSD9616_printLetter(0x06,0x09,0x09,0x7F);  // P
        EZDSP5535_OSD9616_printLetter(0x32,0x49,0x49,0x26);  // S
        EZDSP5535_OSD9616_printLetter(0x3E,0x41,0x41,0x7F);  // D
        EZDSP5535_OSD9616_printLetter(0x43,0x4D,0x51,0x61);  // Z
        EZDSP5535_OSD9616_printLetter(0x10,0x58,0x58,0x30);  // e
        for(i=0;i<5;i++)
        {
        	EZDSP5535_OSD9616_send(0x40,0x00);  // Spaces
        }
        EZDSP5535_OSD9616_printLetter(0x31,0x49,0x49,0x2F);  // 5
       	EZDSP5535_OSD9616_printLetter(0x36,0x49,0x49,0x22);  // 3
        EZDSP5535_OSD9616_printLetter(0x31,0x49,0x49,0x2F);  // 5
       	EZDSP5535_OSD9616_printLetter(0x31,0x49,0x49,0x2F);  // 5
        EZDSP5535_OSD9616_printLetter(0x22,0x41,0x41,0x3E);  // C

        for(i=0;i<47;i++)
        {
        	EZDSP5535_OSD9616_send(0x40,0x00);       // Spaces
        }

        EZDSP5535_OSD9616_send(0x00,0x2e);  // Deactivate Scrolling

        /* Set vertical and horizontal scrolling */
        cmd[0] = 0x00;
        cmd[1] = 0x29;  // Vertical and Right Horizontal Scroll
        cmd[2] = 0x00;  // Dummy byte
        cmd[3] = 0x00;  // Define start page address
        cmd[4] = 0x03;  // Set time interval between each scroll step
        cmd[5] = 0x01;  // Define end page address
        cmd[6] = 0x01;  // Vertical scrolling offset
        EZDSP5535_OSD9616_multiSend( cmd, 7 );
        EZDSP5535_OSD9616_send(0x00,0x2f);

        /* Keep first 8 rows from vertical scrolling  */
        cmd[0] = 0x00;
        cmd[1] = 0xa3;  // Set Vertical Scroll Area
        cmd[2] = 0x08;  // Set No. of rows in top fixed area
        cmd[3] = 0x08;  // Set No. of rows in scroll area
        EZDSP5535_OSD9616_multiSend( cmd, 4 );

#endif
        
        /* Initialising the Pointer to the Audio Class Handle to the Buffer Allocated */
        AC_AppHandle.pAcObj = &ACAppBuffer[0];

        usbConfig.devNum                = CSL_USB0;
        usbConfig.opMode                = CSL_USB_OPMODE_POLLED;
#ifdef APP_USB_SELF_POWERED
        usbConfig.selfPowered           = TRUE;
#else
        usbConfig.selfPowered           = FALSE;
#endif
        usbConfig.maxCurrent            = APP_USB_MAX_CURRENT;
        usbConfig.appSuspendCallBack    = (CSL_USB_APP_CALLBACK)CSL_suspendCallBack;
        usbConfig.appWakeupCallBack     = (CSL_USB_APP_CALLBACK)CSL_selfWakeupCallBack;
        usbConfig.startTransferCallback  = StartTransfer;
        usbConfig.completeTransferCallback = CompleteTransfer;

        USB_init(&usbConfig);

        USB_setHighSpeedMode(0x40); /* parameter is EP0 data size in bytes */

        USB_resetDev(CSL_USB0);

        /* Calling init routine */
        /* Giving all the table hanldes and the buffers to the Audio Class module */
        AC_AppHandle.strDescrApp = (char **)string_descriptor;
        AC_AppHandle.lbaBufferPbApp = &lbaBufferPbApp[0];
        AC_AppHandle.lbaBufferRecApp = &lbaBufferRecApp[0];
        AC_AppHandle.lbaBufferFbckApp = &lbaBufferFbckApp[0];
        AC_AppHandle.lbaBufferHidReportApp = &lbaBufferHidReportApp[0];
        AC_AppHandle.acReqTableApp = USB_ReqTable;
        AC_AppHandle.pId = pId;
        AC_AppHandle.vId = vId;

        AC_AppHandle.rxEpNum = EP_NUM_PLAY; /* playback endpoint number */
        AC_AppHandle.txEpNum = EP_NUM_REC; /* record endpoint number */
        AC_AppHandle.hidTxEpNum = EP_NUM_HID; /* HID interrupt-IN endpoint number */
#ifdef FEEDBACKEP
        AC_AppHandle.fbEpNum = EP_NUM_FBCK; /* feedback endpoint number */
#endif //FEEDBACKEP

///#ifdef SAMPLE_RATE_TX_96kHz
///		LOG_printf(&trace, "PLAYBACK: 96KHZ ");
///		LOG_printf(&trace, "STEREO\n");
        AC_AppHandle.rxPktSize = EP_PB_MAXP; // max packet size for 96K stereo
///#endif // SAMPLE_RATE_TX_96kHz

        AC_AppHandle.txPktSize = EP_REC_MAXP;
        AC_AppHandle.hidTxPktSize = EP_HID_MAXP; // max packet size for HID output report
#ifdef FEEDBACKEP
        AC_AppHandle.fbckTxPktSize = EP_FBCK_MAXP; // 4
#endif //FEEDBACKEP

        /* All Function Handlers need to be Initialised */
        AC_AppHandle.playAudioApp = appPlayAudio;
        AC_AppHandle.recordAudioApp = appRecordAudio;
        AC_AppHandle.initPlayAudioApp = appInitPlayAudio;
        AC_AppHandle.initRecordAudioApp = appInitRecordAudio;
        AC_AppHandle.stopPlayAudioApp = appStopPlayAudio;
        AC_AppHandle.stopRecordAudioApp = appStopRecordAudio;
        AC_AppHandle.mediaGetPresentStateApp = AppGetMediaStatus;
        AC_AppHandle.mediaInitApp = AppMediaInit;
        AC_AppHandle.mediaEjectApp = AppMediaEject;
        AC_AppHandle.mediaLockUnitApp = AppLockMedia;
        AC_AppHandle.getMediaSizeApp = AppGetMediaSize;
        AC_AppHandle.getHidReportApp = appGetHidReport;
        AC_AppHandle.ctrlHandler  = appCtrlFxn;
        AC_AppHandle.isoHandler   = appIsoFxn;
        AC_AppHandle.hidHandler = appHidFxn;

        AC_AppHandle.numLun = 2;

        /* Initialize End point descriptors */
        AC_initDescriptors(AC_AppHandle.pAcObj, (Uint16 *)deviceDescriptorB,
                            CSL_AC_DEVICE_DESCR, sizeof(deviceDescriptorB));

        AC_initDescriptors(AC_AppHandle.pAcObj, (Uint16 *)deviceQualifierDescr,
                            CSL_AC_DEVICE_QUAL_DESCR, 10);

        AC_initDescriptors(AC_AppHandle.pAcObj, (Uint16 *)configDescriptor,
                            CSL_AC_CONFIG_DESCR, sizeof(configDescriptor));

        AC_initDescriptors(AC_AppHandle.pAcObj, (Uint16 *)acHidReportDescriptor,
                            CSL_AC_HID_REPORT_DESC, sizeof(acHidReportDescriptor));

        /* Initialize HID */
        AC_AppHandle.acHidIfNum = IF_NUM_HID; // HID interface number
        AC_AppHandle.acHidReportId = HID_REPORT_ID; // HID report ID
        AC_AppHandle.acHidReportLen = HID_REPORT_SIZE_BYTES; // HID report length (bytes)
        genHidReport(UI_PUSH_BUTTON_NONE, gHidReport); // init. HID report for Get Report 

        /* Call Init API */
        AC_Open(&AC_AppHandle);

        /* Enable CPU USB interrupts */
        CSL_FINST(CSL_CPU_REGS->IER1, CPU_IER1_USB, ENABLE);

        /* Initialize active playback sample rate */
///#if defined(SAMPLE_RATE_TX_96kHz)
        initSampleRate(RATE_96_KHZ, &active_sample_rate, 
            &i2sTxBuffSz);
///#endif
        /* Reset codec output buffer */
        reset_codec_output_buffer();

        /* Start the Tx DMAs */
        DMA_StartTransfer(hDmaTxLeft);
        DMA_StartTransfer(hDmaTxRight);

#ifdef USE_TWO_CODEC
        /* Start the 2nd Tx DMAs */
        DMA_StartTransfer(hDmaTxLeft2);
        DMA_StartTransfer(hDmaTxRight2);
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
        /* Start the 2nd Tx DMAs */
        DMA_StartTransfer(hDmaTxLeft2);
        DMA_StartTransfer(hDmaTxRight2);
        /* Start the 3rd Tx DMAs */
        DMA_StartTransfer(hDmaTxLeft3);
        DMA_StartTransfer(hDmaTxRight3);
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
        /* Start the 2nd Tx DMAs */
        DMA_StartTransfer(hDmaTxLeft2);
        DMA_StartTransfer(hDmaTxRight2);
        /* Start the 3rd Tx DMAs */
        DMA_StartTransfer(hDmaTxLeft3);
        DMA_StartTransfer(hDmaTxRight3);
        /* Start the 4th Tx DMAs */
        DMA_StartTransfer(hDmaTxLeft4);
        DMA_StartTransfer(hDmaTxRight4);
#endif //USE_FOUR_CODEC

        #ifdef ENABLE_RECORD
///#if defined(SAMPLE_RATE_RX_96kHz)
///        LOG_printf(&trace, "RECORD: 96KHZ ");
///#else
///        LOG_printf(&trace, "UNSUPPORTED FORMAT");
///#endif // SAMPLE_RATE_RX_96kHz

        /* Start left Rx DMA */
        DMA_StartTransfer(hDmaRxLeft);

        LOG_printf(&trace, "STEREO\n");
        /* Start right Rx DMA */
        DMA_StartTransfer(hDmaRxRight);

#ifdef USE_TWO_CODEC
        /* Start 2nd left Rx DMA */
        DMA_StartTransfer(hDmaRxLeft2);
        /* Start 2nd right Rx DMA */
        DMA_StartTransfer(hDmaRxRight2);
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
        /* Start 2nd left Rx DMA */
        DMA_StartTransfer(hDmaRxLeft2);
        /* Start 2nd right Rx DMA */
        DMA_StartTransfer(hDmaRxRight2);

        /* Start 3rd left Rx DMA */
        DMA_StartTransfer(hDmaRxLeft3);
        /* Start 3rd right Rx DMA */
        DMA_StartTransfer(hDmaRxRight3);
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
        /* Start 2nd left Rx DMA */
        DMA_StartTransfer(hDmaRxLeft2);
        /* Start 2nd right Rx DMA */
        DMA_StartTransfer(hDmaRxRight2);

        /* Start 3rd left Rx DMA */
        DMA_StartTransfer(hDmaRxLeft3);
        /* Start 3rd right Rx DMA */
        DMA_StartTransfer(hDmaRxRight3);

        /* Start 4th left Rx DMA */
        DMA_StartTransfer(hDmaRxLeft4);
        /* Start 3rd right Rx DMA */
        DMA_StartTransfer(hDmaRxRight4);
#endif //USE_FOUR_CODEC
        #endif // ENABLE_RECORD

#ifdef STORE_PARAMETERS_TO_SDRAM
        initSdram(TRUE, 0x0000);
#endif // STORE_PARAMETERS_TO_SDRAM

        DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx, TRUE);    /* SampleBySample, enable I2S transmit and receive */
#ifdef USE_TWO_CODEC
        DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx2, TRUE);    /* SampleBySample, enable I2S transmit and receive */
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
        DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx2, TRUE);    /* SampleBySample, enable I2S transmit and receive */
        DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx3, TRUE);    /* SampleBySample, enable I2S transmit and receive */
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
        DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx2, TRUE);    /* SampleBySample, enable I2S transmit and receive */
        DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx3, TRUE);    /* SampleBySample, enable I2S transmit and receive */
        DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx4, TRUE);    /* SampleBySample, enable I2S transmit and receive */
#endif //USE_FOUR_CODEC

        /* Initialize pitch calculation */
        initPitchCalc(hDmaTxLeft);

        /* Set HWAI and IPORT in ICR */
        *(volatile unsigned int *)0x0001 = (1<<9) | (1<<8) | 0x0E;
        asm("   idle");     

        /* Clock gate usused peripherals */
		//ClockGating();
    }
}

/* Resets C5515 */
void C5515_reset(void)
{
    volatile int i;

    // disable all interrupts (IER0 and IER1)
    *(volatile ioport unsigned int *)(0x0000) = 0x0000;
    *(volatile ioport unsigned int *)(0x0045) = 0x0000;

    // clear all interrupts (IFR0 and IFR1)
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;

    // enable all peripherials
    *(volatile ioport unsigned int *)(0x1c02) = 0;
    *(volatile ioport unsigned int *)(0x1c03) = 0;

    // reset peripherals
    *(volatile ioport unsigned int *)(0x1c04) = 0x0020;
    *(volatile ioport unsigned int *)(0x1c05) = 0x00BF;
    // some delay
    for (i=0; i<0xFFF; i++);

    // clear all interrupts (IFR0 and IFR1)
    *(volatile ioport unsigned int *)(0x0001) = 0xFFFF;
    *(volatile ioport unsigned int *)(0x0046) = 0xFFFF;
}

#ifdef C5535_EZDSP_DEMO
// Power Display task code
void PowerDisplayTask(void)
{
	while (1)
	{
		// sleep for one second
		TSK_sleep(1000);
		// read and display the power usage
		if (DemoSwitchFlag==0)
		{
			//CSL_i2cPowerTest();
		}
	}
}

// Spectrum Display Task code
void SpectrumDisplayTask(void)
{
	// display the play audio message
	//print_playaudio();

	while (1)
	{
		// wait on bufferIn ready semaphore
		SEM_pend(&SEM_BufferInReady, SYS_FOREVER);
		// compute and display the bargraph
		if (DemoSwitchFlag)
		{
			//calculate_FFT(bufferIn, 256);
			// clear the bufferInIdx to 0
			bufferInIdx = 0;
		}		
	}
}

// PRD function. Runs every 10 second to switch the demo mode between
// power display mode and spectrum analyzer mode
void DemoSwitch(void)
{
	DemoSwitchFlag++;
	if (DemoSwitchFlag==1)
	{
		// if we were in power display mode, swtch to spectrum analyzer mode
		// clear the bufferInIdx
		bufferInIdx = 0; 
	}
	else if (DemoSwitchFlag==3)
	{
		// if we were in spectrum analyzer mode, switch to power display mode
		DemoSwitchFlag = 1;
		// stop data collection for spectrum analyzer
		bufferInIdx = 0;
	}
}
#endif

// Clock gating for unused peripherals
void ClockGating(void)
{
	Uint16 pcgcr_value, clkstop_value;
	
	// set PCGCR1
	pcgcr_value = 0; 
	// clock gating SPI
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_SPICG, DISABLED);
	// clock gating SD/MMC
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD0CG, DISABLED);
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD1CG, DISABLED);
	// clock stop request for UART
	clkstop_value = CSL_FMKT(SYS_CLKSTOP_URTCLKSTPREQ, REQ);
	// write to CLKSTOP
	CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
	// wait for acknowledge
	while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_URTCLKSTPACK)==0);
	// clock gating UART
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_UARTCG, DISABLED);
	// clock stop request for EMIF
	//clkstop_value = CSL_FMKT(SYS_CLKSTOP_EMFCLKSTPREQ, REQ);
	// write to CLKSTOP
	//CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
	// wait for acknowledge
	//while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_EMFCLKSTPACK)==0);
	// clock gating EMIF
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR1_EMIFCG, DISABLED);
	// clock gating unused I2S (I2S 0, 1, 3)
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S0CG, DISABLED);
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S1CG, DISABLED);
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S2CG, DISABLED);
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S3CG, DISABLED);
	// clock gating DMA0
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_DMA0CG, DISABLED);
	// clock gating Timer 1
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR1CG, DISABLED);
	// clock gating Timer 2
	pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR2CG, DISABLED);
	// write to PCGCR1
	CSL_FSET(CSL_SYSCTRL_REGS->PCGCR1, 15, 0, pcgcr_value);
	
	// set PCGCR2
	pcgcr_value = 0; 
	// clock gating LCD
	pcgcr_value |= CSL_FMKT(SYS_PCGCR2_LCDCG, DISABLED);
	// clock gating SAR
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR2_SARCG, DISABLED);
	// clock gating DMA1
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA1CG, DISABLED);
	// clock gating DMA2
	pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA2CG, DISABLED);
	// clock gating DMA3
	pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA3CG, DISABLED);
	// clock analog registers
	//pcgcr_value |= CSL_FMKT(SYS_PCGCR2_ANAREGCG, DISABLED);
    // write to PCGCR2
	CSL_FSET(CSL_SYSCTRL_REGS->PCGCR2, 15, 0, pcgcr_value);
	
	// disable the CLKOUT. It is on reset
	// set bit 2 of ST3_55 to 1
	asm("	bit(ST3, #ST3_CLKOFF) = #1");
	
	// turn off the XF
	// set bit 13 of ST1_55 to 0
	asm("	bit(ST1, #ST1_XF) = #0");

#ifdef C5535_EZDSP_DEMO
    // turn off the DS3-6
	// set the GPIO pin 14 - 15 to output, set SYS_GPIO_DIR0 (0x1C06) bit 14 and 15 to 1 
    *(volatile ioport unsigned int *)(0x1C06) |= 0xC000;
	// set the GPIO pin 16 - 17 to output, set SYS_GPIO_DIR1 (0x1C07) bit 0 and 1 to 1 
    *(volatile ioport unsigned int *)(0x1C07) |= 0x0003;
	
	// set the GPIO 14 - 15 to 0, set SYS_GPIO_DATAOUT0 (0x1C0A) bit 14 and 15 to 0
    //*(volatile ioport unsigned int *)(0x1C0A) &= 0x3FFF;
    *(volatile ioport unsigned int *)(0x1C0A) |= 0xC000;
	// set the GPIO 16 - 17 to 0, set SYS_GPIO_DATAOUT1 (0x1C0B) bit 0 and 1 to 0
    //*(volatile ioport unsigned int *)(0x1C0B) &= 0xFFFC;
    *(volatile ioport unsigned int *)(0x1C0B) |= 0x0003;
#endif

	return;
}

#if 0
void userIdle(void)
{
	// set CPUI bit in ICR
	*(ioport volatile unsigned int *)(0x0001) = 0x000F;
	// execute idle instruction to make CPU idle
	asm("	idle");		
}
#endif

extern Uint32 recAudioTaskCount;
extern Uint32 playAudioTaskCount;
extern Uint32 sofIntCountRec;
extern Uint32 sofIntCountPlay;

Uint32 recAudioTaskCountSave = 0;
Uint32 playAudioTaskCountSave = 0;
Uint32 sofIntCountRecSaved = 0;
Uint32 sofIntCountPlaySave = 0;

Uint32 recAudioTaskCountSec = 0;
Uint32 playAudioTaskCountSec = 0;
Uint32 sofIntCountRecSec = 0;
Uint32 sofIntCountPlaySec = 0;

Uint32 secCount = 0;
void secPrd()
{
	secCount++;
	
	recAudioTaskCountSec = recAudioTaskCount - recAudioTaskCountSave;
	recAudioTaskCountSave = recAudioTaskCount;
	
	playAudioTaskCountSec = playAudioTaskCount - playAudioTaskCountSave;
	playAudioTaskCountSave = playAudioTaskCount;

	sofIntCountRecSec = sofIntCountRec - sofIntCountRecSaved;
	sofIntCountRecSaved = sofIntCountRec;

	sofIntCountPlaySec = sofIntCountPlay = sofIntCountPlaySave;
	sofIntCountPlaySave = sofIntCountPlay;
}



void SYS_GlobalIntEnable(void)
{
    asm(" BIT (ST1, #ST1_INTM) = #0");
}

void SYS_GlobalIntDisable(void)
{
    asm(" BIT (ST1, #ST1_INTM) = #1");
}
