/*
 * $$$MODULE_NAME dbg_sdram.h
 *
 * $$$MODULE_DESC dbg_sdram.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _DBG_SDRAM_H_
#define _DBG_SDRAM_H_

// SDRAM base address- should match value in bios tcf file
#define SDRAM_BASE_ADDR         ( 0x28000 )  /* 16-bit word base address */
#define SDRAM_LEN               ( 0x3D8000 ) /* length in 16-bit words */

/* SDTIMR2 EMIF register (missing from CSL 2.01 */
#define EMIF_SDTIMR2            ( 0x1021 )
#define EMIF_SDTIMR2_TRFC_VAL   ( 0x8 )
#define EMIF_SDTIMR2_TRP_VAL    ( 0x2 )
#define EMIF_SDTIMR2_TRCD_VAL   ( 0x2 )
#define EMIF_SDTIMR2_TWR_VAL    ( 0x5 )

extern Int16 *sdramBasePtr0;
extern Int16 *sdramPtr0;
extern Int16 *sdramBasePtr1;
extern Int16 *sdramPtr1;

extern Uint32 sdram_cntr;
extern Uint32 sdram_flag;

/* Congures EMIF/SDRAM */
void initSdram(
    Bool fillMem, 
    Int16 fillMemVal
);

#endif /* _DBG_SDRAM_H_ */
