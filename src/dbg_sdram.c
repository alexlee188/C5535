/*
 * $$$MODULE_NAME dbg_sdram.c
 *
 * $$$MODULE_DESC dbg_sdram.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include "soc.h"
#include "dbg_sdram.h"

Int16 *sdramBasePtr0;
Int16 *sdramPtr0;
Int16 *sdramBasePtr1;
Int16 *sdramPtr1;

Uint32 sdram_cntr;
Uint32 sdram_flag;

/* Congures EMIF/SDRAM */
void initSdram(
    Bool fillMem, 
    Int16 fillMemVal
)
{
    Uint32 i;
    Uint16 reg;
    volatile ioport Uint16 *pReg;
    Int16 *sdram_ptr;

    /* Congure EMIF/SDRAM */
    CSL_FINST(CSL_SYSCTRL_REGS->SDRAMCCR, SYS_SDRAMCCR_SDCLK_EN, SDCLKOFF);
    for(i=0;i<20000;i++);
    CSL_FINST(CSL_SYSCTRL_REGS->ESCR, SYS_ESCR_BYTEMODE, FULL);
    reg = CSL_FMK(EMIF_SDTIMR1_TRAS, 0x3) |
        CSL_FMK(EMIF_SDTIMR1_TRC, 0x5) |
        CSL_FMK(EMIF_SDTIMR1_TRRD, 0x1);
    CSL_EMIF_REGS->SDTIMR1 = reg;
    pReg = (volatile ioport Uint16 *)EMIF_SDTIMR2;
    *pReg = (EMIF_SDTIMR2_TRFC_VAL << 11) |
        (EMIF_SDTIMR2_TRP_VAL << 8) |
        (EMIF_SDTIMR2_TRCD_VAL << 4) |
        (EMIF_SDTIMR2_TWR_VAL << 0); /* 0x4225 */
    reg = CSL_FMK(EMIF_SDCR1_NM, 0x1) |
        CSL_FMKT(EMIF_SDCR1_CL, THREE) |
        CSL_FMKT(EMIF_SDCR1_BIT_11_9_LOCK, WRITES) |
        CSL_FMKT(EMIF_SDCR1_IBANK, FOUR) |
        CSL_FMKT(EMIF_SDCR1_EBANK, ONE) |
        CSL_FMKT(EMIF_SDCR1_PAGESIZE, EIGHT);
    CSL_EMIF_REGS->SDCR1 = reg;
    reg = CSL_FMKT(EMIF_SDCR2_SR, EXIT) |
        CSL_FMKT(EMIF_SDCR2_PD, EXIT) |
        CSL_FMKT(EMIF_SDCR2_PDWR, NOREFR) |
        CSL_FMKT(EMIF_SDCR2_PASR, FOUR) |
        CSL_FMKT(EMIF_SDCR2_ROWSIZE, TWELVE) |
        CSL_FMK(EMIF_SDCR2_IBANK_POS, 0x0) |
        CSL_FMKT(EMIF_SDCR2_SDRAM_DRIVE, FULL) |
        CSL_FMKT(EMIF_SDCR2_BIT_9_1_LOCK, WRITES);
    CSL_EMIF_REGS->SDCR2 = reg;
    for (i=0; i<2048; i++);

    /* Point to initial location to store parameters in SDRAM */
    sdramBasePtr0 = (Int16 *)SDRAM_BASE_ADDR;
    sdramPtr0 = sdramBasePtr0;
    sdramBasePtr1 = sdramBasePtr0 + SDRAM_LEN/2;
    sdramPtr1 = sdramBasePtr1;

    sdram_cntr = 0;
    sdram_flag = 0;

    /* Fill memory */
    if (fillMem == TRUE)
    {
        sdram_ptr = (Int16 *)SDRAM_BASE_ADDR;
        for (i=0; i < SDRAM_LEN; i++)
        {
            *sdram_ptr = fillMemVal;
            sdram_ptr++;
        }
    }
}
