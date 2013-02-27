/*
 * $$$MODULE_NAME bios_osserv.h
 *
 * $$$MODULE_DESC bios_osserv.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/** \file   bios_osserv.h
    \brief  BIOS OS Service Header File

    This file declares ECM mappings for each of the peripheral supported by PSP.

    (C) Copyright 2007, Texas Instruments, Inc

    \author     - PSG, TI
    \version    1.0
*/
/**
 * \defgroup BIOS_OsServ BIOS OS Service Interface
 *
 * BIOS OS Service Interface
 * @{
 */

#ifndef _BIOS_OSSERV_H_
#define _BIOS_OSSERV_H_

#include <psp_common.h>


/** \brief BIOS ECM Mappings, for each peripheral
 *
 */
#define BIOS_HWI_MASK_EDMA      (0x0080u)
#define BIOS_HWI_MASK_UART_0    (0x0100u)
#define BIOS_HWI_MASK_UART_1    (0x0100u)
#define BIOS_HWI_MASK_UART_2    (0x0100u)
#define BIOS_HWI_MASK_UART_3    (0x0100u)
#define BIOS_HWI_MASK_I2C_0     (0x0100u)
#define BIOS_HWI_MASK_I2C_1     (0x0100u)
#define BIOS_HWI_MASK_I2C_2     (0x0100u)
#define BIOS_HWI_MASK_I2C_3     (0x0100u)
#define BIOS_HWI_MASK_MIBSPI_0  (0x0100u)
#define BIOS_HWI_MASK_SPI_1     (0x0100u)
#define BIOS_HWI_MASK_SPI_2     (0x0100u)
#define BIOS_HWI_MASK_SPI_3     (0x0100u)
#define BIOS_HWI_MASK_McASP_0   (0x0080u)
#define BIOS_HWI_MASK_McASP_1   (0x0080u)
#define BIOS_HWI_MASK_McASP_2   (0x0080u)
#define BIOS_HWI_MASK_McASP_3   (0x0100u)
#define BIOS_HWI_MASK_SPDIF_0   (0x0200u)
#define BIOS_HWI_MASK_VENC      (0x0040u)
#define BIOS_HWI_MASK_CCDC      (0x0040u)
#define BIOS_HWI_MASK_DARTE     (0x0080u)
#define BIOS_HWI_MASK_MMCSD     (0x0100u)

//#define BIOS_HWI_MASK_MMCSD     0x0 //(0x0080u) // Re-check

#endif /* __BIOS_OS_SERV_H__ */

