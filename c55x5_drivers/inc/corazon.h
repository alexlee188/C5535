/*
 * $$$MODULE_NAME corazon.h
 *
 * $$$MODULE_DESC corazon.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/
#ifndef _CORAZON_H
#define _CORAZON_H

/*****************************************************************************/
 /** \file Corazon.h
 * 
 * \brief This file contains the Chip Description for CORAZON
 * 
 *****************************************************************************/

#include <cslr.h>
#include <tistdtypes.h>
#include <csl_ioport.h>

/*****************************************************************************\
* Include files for all the modules in the device
\*****************************************************************************/

#include "cslr_i2c_001.h"
#include "cslr_i2s_001.h"
//#include "cslr_emif_001.h"
//#include "cslr_uart_001.h"
//#include "cslr_spi_001.h"
//#include "cslr_mmcsd_001.h"
//#include "cslr_lcdc_001.h"
//#include "cslr_rtc_001.h"
#include "cslr_dma_001.h"
#include "cslr_dmaevtint_001.h"
//#include "cslr_sar_001.h"
//#include "cslr_usb_001.h"
#include "cslr_pll_001.h"


#define PLL_CNFIG(VP,VS) \
    CSL_FINS(CSL_PLL_REGS->PLLCNTL2,CSL_PLL_PLLCNTL2_PLL_DIS,1);\
    CSL_FINS(CSL_PLL_REGS->PLLCNTL1,CSL_PLL_PLLCNTL1_VP,VP);\
    CSL_FINS(CSL_PLL_REGS->PLLINCNTL,CSL_PLL_PLLINCNTL_VS,VS);\
    CSL_FINS(CSL_PLL_REGS->PLLOUTCNTL,CSL_PLL_PLLOUTCNTL_OUT_DIV_EN,0);\
    CSL_FINS(CSL_PLL_REGS->PLLINCNTL,CSL_PLL_PLLINCNTL_RP_BYPASS,0);\
    CSL_FINS(CSL_PLL_REGS->PLLINCNTL,CSL_PLL_PLLINCNTL_RD,120);\
    CSL_FINS(CSL_PLL_REGS->PLLCNTL1,CSL_PLL_PLLCNTL1_PLL_STANDBY,0);\
    CSL_FINS(CSL_PLL_REGS->PLLCNTL2,CSL_PLL_PLLCNTL2_PLL_DIS,0);\
    asm("\trpt #1000");\
    asm("\tnop");

/*
RP_bypass is 0 since Input clock is 12MHz and has to be scaled down betweek 30kHz-170kHz for PD input
*/

/*****************************************************************************\
* Peripheral Instance counts
\*****************************************************************************/
#define CSL_SAR_PER_CNT     1
#define CSL_USB_PER_CNT     1
#define CSL_I2C_PER_CNT     1                                              
#define CSL_UART_PER_CNT    1                                       
#define CSL_SPI_PER_CNT     1                                       
#define CSL_MMCSD_PER_CNT   2                                       
#define CSL_LCDC_PER_CNT        1    
#define CSL_RTC_PER_CNT     1      
#define CSL_DMA_PER_CNT     4                                

#define CORAZON_SILICON                                         
#define SDRAMTIMING0    0x5810
#define SDRAMTIMING1    0x4221
//#define   PLL_60MHz
//#define   PLL_100MHz

/*****************************************************************************\
* Peripheral Overlay Structures
\*****************************************************************************/
//typedef volatile ioport CSL_UsbRegs          * CSL_UsbRegsOvly;    
typedef volatile ioport CSL_I2cDrvRegs       * CSL_I2cDrvRegsOvly;    
typedef volatile ioport CSL_I2sDrvRegs       * CSL_I2sDrvRegsOvly;         
//typedef volatile ioport CSL_EmifRegs         * CSL_EmifRegsOvly;                          
//typedef volatile ioport CSL_UartRegs         * CSL_UartRegsOvly;                  
//typedef volatile ioport CSL_SpiRegs          * CSL_SpiRegsOvly;                   
//typedef volatile ioport CSL_MmcsdDrvRegs        * CSL_MmcsdDrvRegsOvly;                 
//typedef volatile ioport CSL_LcdcRegs         * CSL_LcdcRegsOvly;   
//typedef volatile ioport CSL_RtcRegs          * CSL_RtcRegsOvly;      
typedef volatile ioport CSL_DmaDrvRegs          * CSL_DmaDrvRegsOvly;
//typedef volatile ioport CSL_SarRegs          * CSL_SarRegsOvly;    
typedef volatile ioport CSL_PllRegs          * CSL_PllRegsOvly;  
typedef volatile ioport CSL_DmaEvtRegs       * CSL_DmaEvtIntRegsOvly;



/*****************************************************************************\
* Peripheral Base Address
\*****************************************************************************/
//#define CSL_USB_REGS                    ((CSL_UsbRegsOvly)  0x8000) 
//#define CSL_SAR_REGS                    ((CSL_SarRegsOvly)  0x7000) 
//#define CSL_EMIF_REGS                   ((CSL_EmifRegsOvly) 0x1000) 
#define CSL_I2C_REGS                  ((CSL_I2cDrvRegsOvly)  0x1A00) 
#define CSL_I2S_REGS0                   ((CSL_I2sDrvRegsOvly)   0x2800)       
#define CSL_I2S_REGS1                   ((CSL_I2sDrvRegsOvly)   0x2900)       
#define CSL_I2S_REGS2                   ((CSL_I2sDrvRegsOvly)   0x2A00)       
#define CSL_I2S_REGS3                   ((CSL_I2sDrvRegsOvly)   0x2B00)   
//#define CSL_UART_REGS                   ((CSL_UartRegsOvly) 0x1B00)
//#define CSL_SPI_REGS                      ((CSL_SpiRegsOvly)  0x3000)          
//#define CSL_MMCSD0_REGS                   ((CSL_MmcsdDrvRegsOvly)0x3A00)        
//#define CSL_MMCSD1_REGS                   ((CSL_MmcsdRegsOvly)0x3B00)        
//#define CSL_LCDC_REGS                     ((CSL_LcdcRegsOvly) 0x2E00)  
//#define CSL_RTC_REGS                      ((CSL_RtcRegsOvly)  0x1900) 
#define CSL_PLL_REGS                    ((CSL_PllRegsOvly)  0x1C20)  
#define CSL_DMA_REGS0                   ((CSL_DmaDrvRegsOvly)  0x0C00)       
#define CSL_DMA_REGS1                   ((CSL_DmaDrvRegsOvly)  0x0D00)       
#define CSL_DMA_REGS2                   ((CSL_DmaDrvRegsOvly)  0x0E00)       
#define CSL_DMA_REGS3                   ((CSL_DmaDrvRegsOvly)  0x0F00)  
#define CSL_DMAEVTINT_REGS              ((CSL_DmaEvtIntRegsOvly)0x1C1A)            

/** \brief I2C Module Instances
* 
*/
typedef enum {
  CSL_I2C_ANY    = -1, /**< <b>: Any instance of I2C module</b> */
  CSL_I2C_0      =  0  /**< <b>: I2C Instance 0</b> */
} CSL_I2cNum;


/** \brief GPIO Module Instances
* 
*/
typedef enum {
  CSL_GPIO_ANY    = -1, /**< <b>: Any instance of GPIO module</b> */
  CSL_GPIO        =  0,  /**< <b>: GPIO </b> */
  CSL_MGPIO       =  1  /**< <b>: Multiplexed GPIO </b> */
} CSL_GpioNum;


/** \brief  UART Module Instances
* 
*/
typedef enum {
  CSL_UART_ANY    = -1, /**< <b>: Any instance of UART module</b> */
  CSL_UART_0          =  0,  /**< <b>: UART Instance 0</b> */
  CSL_UART_1          =  1  /**< <b>: UART Instance 1</b> */
} CSL_UartNum;

/** \brief  SPI Module Instances
* 
*/
typedef enum {
  CSL_SPI_ANY    = -1, /**< <b>: Any instance of SPI module</b> */
  CSL_SPI_0      =  0  /**< <b>: SPI Instance 0</b> */
} CSL_SpiNum;

/** \brief  MMC/SD Module Instances
* 
*/
typedef enum {
  CSL_MMCSD_ANY    = -1, /**< <b>: Any instance of MMCSD module</b> */
  CSL_MMCSD_0      =  0  /**< <b>: MMCSD Instance 0</b> */
} CSL_MmcsdNum;


/** \brief  DMA Module Instances
* 
*/
typedef enum {
  CSL_DMA_ANY    = -1, /**< <b>: Any instance of DMA module</b> */
  CSL_DMA_0      =  0  /**< <b>: DMA Instance 0</b> */
} CSL_DmaNum;

/** @brief Enumerations for DMA channels */
typedef enum {
    CSL_DMA_CHA_ANY        =             -1,  /**< Any Channel */
    CSL_DMA_CHA_0          =              0,  /**< Channel 0 */
    CSL_DMA_CHA_1          =              1,  /**< Channel 1 */
    CSL_DMA_CHA_2          =              2   /**< Channel 2 */
} CSL_DmaChaNum;

/** \brief  ATA Module Instances
* 
*/
typedef enum {
    CSL_ATA_PRIMARY = 0,
    CSL_ATA_0 = 0,
    CSL_ATA_SECONDARY = 1,
    CSL_ATA_1 = 0,    
    CSL_ATA_ANY = -1
} CSL_AtaNum;

/** \brief LCDC Module Instances
*
*/

typedef enum {
  CSL_LCDC_ANY    = -1, /**< <b>: Used to refer any instance of LCDC
                  module</b> */
  CSL_LCDC_0      =  0  /**< <b>: LCDC Instance 0</b> */
} CSL_LcdcNum;

/** \brief  NTSC/PAL Module Instances
* 
*/
typedef enum {
  CSL_NTSC_ANY    = -1, /**< <b>: Any instance of NTSC module</b> */
  CSL_NTSC_0      =  0  /**< <b>: NTSC Instance 0</b> */
} CSL_NtscNum;

/** \brief EMIF Module Instances
* 
*/
typedef enum {
  CSL_EMIF_ANY    = -1, /**< <b>: Any instance of EMIF module</b> */
  CSL_EMIF_0      =  0  /**< <b>: EMIF Instance 0</b> */
} CSL_EmifNum;

extern void * _CSL_mmcsdlookup[];

/*
extern void * _CSL_i2clookup[];
extern void * _CSL_uartlookup[];
extern void * _CSL_spilookup[];
extern void * _CSL_lcdclookup[];
*/
#endif
