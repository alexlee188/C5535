/*
 * $$$MODULE_NAME user_interface.h
 *
 * $$$MODULE_DESC user_interface.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _USER_INTERFACE_H_
#define _USER_INTERFACE_H_

#include <std.h>
#include "csl_gpt.h"
#include "data_types.h"

#define UI_SOK      ( 0 )   /* indicates successful return status */
#define UI_INV_PRM  ( 1 )   /* indicates invalid parameter to function */
#define UI_CSL_FAIL ( 2 )   /* indicates CSL function failure */

#define UI_DEF_CPU_FREQ         ( 100000000.0 ) /* default CPU frequency in Hz */
#define UI_DEF_SAR_SAMP_FREQ    ( 15.625 )      /* default SAR sampling frequency in Hz */

/* SAR sample voltages correponding to push-button network presses */
/* LSBs discarded to account for variability in resistors and SAR supply voltage */
#ifdef C5535_EZDSP
#define SAR_SAMP_SHIFT          ( 4 ) /* no. LSBs to discard from SAR sample */
#define SAR_SAMP_SW_NONE        ( 0x3FF >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW06           ( 0x376 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW07           ( 0x2A8 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW08           ( 0x318 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW09           ( 0x238 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW10           ( 0x218 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW11           ( 0x1D9 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW12           ( 0x168 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW13           ( 0x198 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW14           ( 0x1F8 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW15           ( 0x0D8 >> SAR_SAMP_SHIFT )
#else
#define SAR_SAMP_SHIFT          ( 4 ) /* no. LSBs to discard from SAR sample */
#define SAR_SAMP_SW_NONE        ( 0x3FF >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW06           ( 0x376 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW07           ( 0x259 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW08           ( 0x318 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW09           ( 0x238 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW10           ( 0x218 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW11           ( 0x1D9 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW12           ( 0x168 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW13           ( 0x000 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW14           ( 0x2C9 >> SAR_SAMP_SHIFT )
#define SAR_SAMP_SW15           ( 0x0D8 >> SAR_SAMP_SHIFT )
#endif

#define UI_MIC_MUTE  ( UI_PUSH_BUTTON_SW06 )
#define UI_PB_VOL_DN ( UI_PUSH_BUTTON_SW14 )
#define UI_PB_VOL_UP ( UI_PUSH_BUTTON_SW07 )
#define UI_PB_MUTE   ( UI_PUSH_BUTTON_SW13 )

#define NUM_ALLOWED_PBN_STATES ( 5 ) /* number of allowed push-button network states */

#define HID_REPORT_ID           ( 1 ) /* HID report ID */
#define HID_REPORT_SIZE_BYTES   ( 3 ) /* report size in bytes */
#define HID_REPORT_SIZE_WORDS   ( (HID_REPORT_SIZE_BYTES + 1)/2 )

/* Push-button network states */
typedef enum
{
    UI_PUSH_BUTTON_NONE,
    UI_PUSH_BUTTON_SW06,
    UI_PUSH_BUTTON_SW07,
    UI_PUSH_BUTTON_SW08,
    UI_PUSH_BUTTON_SW09,
    UI_PUSH_BUTTON_SW10,
    UI_PUSH_BUTTON_SW11,
    UI_PUSH_BUTTON_SW12,
    UI_PUSH_BUTTON_SW13,
    UI_PUSH_BUTTON_SW14,
    UI_PUSH_BUTTON_SW15, 
    UI_PUSH_BUTTON_UNKNOWN
} UI_PBNState;

/* Allowed push-button network states */
extern UI_PBNState gAllowedPbnStates[NUM_ALLOWED_PBN_STATES];

extern Bool gHidReportReady; /* indicates whether HID report ready for Tx */
extern Uint8 gHidReport[HID_REPORT_SIZE_WORDS]; /* HID report */

/* Initializes user interface */
Int16 userInterfaceInit(
     Float32 cpuFreq,
     Float32 pbnSampFreq, 
     CSL_Status *pCslStatus
);

/* Starts user interface */
/* Starts user interface */
Int16 userInterfaceStart(
     CSL_Status *pCslStatus
);

/* Gets current state of push-button network */
Int16 getCurrentPBNState(
    Uint16 sarReadData, 
    UI_PBNState *pPbnState
);

/* Checks if current push-button network state is in allowed set of states */
Int16 chkAllowedPBNState(
    UI_PBNState pbnState, 
    UI_PBNState *allowedPbnStates, 
    Uint16 numAllowedPbnStates,
    Int16 *pPbnStateAllowed
);

/* Generates HID report */
void genHidReport(
    UI_PBNState pbnState, 
    Uint8 *pHidReport
);

#endif /* _USER_INTERFACE_H_ */
