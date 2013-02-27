/*
 * $$$MODULE_NAME user_interface.c
 *
 * $$$MODULE_DESC user_interface.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include <string.h>
#include "VC5505_CSL_BIOS_cfg.h"
#include "csl_usb.h"
#include "csl_gpt.h"
#include "csl_sar.h"
#include "csl_gpio.h"
#include "data_types.h"
#include "app_usb.h"
#include "user_interface.h"

//#define PBN_SIM /* define for push-button network simulation */

#define SAR_MAX_CLK_FREQ        ( 2000000.0 ) /* SAR max. clock freq. = 2 MHz */

#ifdef PBN_SIM
#define NUM_TEST_PBN_STATES     ( 5 ) /* number of test push-button network states */

/* Test PBN state */
typedef struct
{
    UI_PBNState pbnState;   /* test push-button network state */
    Uint16 numPollInts;     /* number of polling intervals test state is active */
} TestPbnState;

/* Push-button network simulation parameters */
typedef struct {
    Bool pbnSimLoop; /* indicates whether to loop test PBN states */
} PbnSimPrms;

/* Push-button network simulation state */
typedef struct
{
    Bool pbnSimDone; /* indicates whether PBN simulation complete */
    Bool pbnSimLoop; /* indicates whether to continuously loop test PBN states */
    Int16 curTestPbnState;
    Uint16 pollIntCnt;
} PbnSimState;

/* Test push-button network states */
static TestPbnState testPbnState[NUM_TEST_PBN_STATES] = 
{
    {UI_PUSH_BUTTON_NONE, 2}, 
    {UI_PB_VOL_DN, 2}, 
    {UI_PUSH_BUTTON_NONE, 2}, 
    {UI_PB_VOL_DN, 2},
    {UI_PUSH_BUTTON_NONE, 2}, 
};

/* Push-button network simulation state */
static PbnSimState gPbnSimState;
#endif

/* Allowed push-button network states */
/* Note multiple button presses not allowed */
UI_PBNState gAllowedPbnStates[NUM_ALLOWED_PBN_STATES] = 
{
    UI_PUSH_BUTTON_NONE, 
    UI_MIC_MUTE, 
    UI_PB_VOL_DN, 
    UI_PB_VOL_UP,
    UI_PB_MUTE
};

Bool gHidReportReady; /* indicates whether HID report ready for Tx */
Uint8 gHidReport[HID_REPORT_SIZE_WORDS]; /* HID report */

/* GPT object/handle for SAR sampling for HID */
static CSL_GptObj gsGptObj;
static CSL_Handle gsGptHandle;
/* SAR object/handle for SAR sampling for HID */
static CSL_SarHandleObj gsSarObj;
static SAR_Handle gsSarHandle;

static UI_PBNState gPrevPbnState;  /* previous push-button network state */

extern CSL_UsbContext     gUsbContext;

#ifdef PBN_SIM
/* Initializes push-button network simulation */
static Int16 PbnSimInit(
    PbnSimPrms *pPbnSimPrms, 
    PbnSimState *pPbnSimState
);

/* Initializes push-button network simulation */
static Int16 PbnSimInit(
    PbnSimPrms *pPbnSimPrms, 
    PbnSimState *pPbnSimState
)
{
    if ((pPbnSimPrms == NULL) || (pPbnSimPrms == NULL))
    {
        return -1;
    }

    pPbnSimState->pbnSimDone = FALSE;
    pPbnSimState->pbnSimLoop = pPbnSimPrms->pbnSimLoop;
    pPbnSimState->curTestPbnState = -1;
    pPbnSimState->pollIntCnt = 0;

    return 0;
}
#endif

/* Initializes user interface */
Int16 userInterfaceInit(
     Float32 cpuFreq,
     Float32 pbnSampFreq,
     CSL_Status *pCslStatus
)
{
    CSL_SarChSetup SarParam;
    CSL_SarChanSel sarChanNo;
    CSL_Config hwConfig;
    Uint32 tmrPrd;
    CSL_Status status;
#ifdef PBN_SIM
    PbnSimPrms pbnSimPrms;
#endif

#ifdef PBN_SIM
    /* Initialize push-button network simulation */
    pbnSimPrms.pbnSimLoop = TRUE;
    PbnSimInit(&pbnSimPrms, &gPbnSimState);
#endif

    /* Initialize SAR CSL */
    SAR_init();

    /* Populate SAR object information */
    status = SAR_chanOpen(&gsSarObj, CSL_SAR_CHAN_3);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }
    gsSarHandle = &gsSarObj;

    /* Initialize SAR registers */
    SarParam.SysClkDiv = (Uint16)((Float32)cpuFreq/SAR_MAX_CLK_FREQ + 0.5)-1;
    SarParam.OpMode = CSL_SAR_POLLING;
    SarParam.MultiCh = CSL_SAR_NO_DISCHARGE;
    SarParam.RefVoltage = CSL_SAR_REF_VIN;
    status = SAR_chanSetup(gsSarHandle, &SarParam);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Initialize A/D conversion type */
    status = SAR_A2DMeasParamSet(gsSarHandle, CSL_KEYPAD_MEAS, &sarChanNo);
    if ((status != CSL_SOK) || (sarChanNo != CSL_SAR_CHAN_3))
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Initialize A/D conversion type */
    status = SAR_chanCycSet(gsSarHandle, CSL_SAR_SINGLE_CONVERSION);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Initialize UI control variables */
    gPrevPbnState = UI_PUSH_BUTTON_NONE;
    genHidReport(UI_PUSH_BUTTON_NONE, gHidReport);
    gHidReportReady = FALSE;

    /* Open GPT1 -- used as timer for SAR sampling for HID */
    gsGptHandle = GPT_open(GPT_1, &gsGptObj, &status);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Reset GPT1 */
    status = GPT_reset(gsGptHandle);
    if(status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Configure GPT1 */
    tmrPrd = (Uint32)((Float32)cpuFreq/2.0/pbnSampFreq + 0.5)-1;
    hwConfig.autoLoad = GPT_AUTO_ENABLE;
    hwConfig.ctrlTim = GPT_TIMER_ENABLE;
    hwConfig.preScaleDiv = GPT_PRE_SC_DIV_0;
    hwConfig.prdLow = (Uint16)tmrPrd;
    hwConfig.prdHigh = (Uint16)(tmrPrd>>16);
    status = GPT_config(gsGptHandle, &hwConfig);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Clear pending interrupts */
    CSL_SYSCTRL_REGS->TIAFR = 0x7;

    return UI_SOK;
}

/* Starts user interface */
Int16 userInterfaceStart(
     CSL_Status *pCslStatus
)
{
    CSL_Status status;

    /* Start 1st conversion -- subsequent conversions are initiated in response to Timer ISR */
    status = SAR_startConversion(gsSarHandle);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    /* Start GPT1 -- initiate SAR sampling for HID */
    status = GPT_start(gsGptHandle);
    if (status != CSL_SOK)
    {
        *pCslStatus = status;
        return UI_CSL_FAIL;
    }

    return UI_SOK;
}

/* Gets current state of push-button network */
/* Currently push-button network simulation */
Int16 getCurrentPBNState(
    Uint16 sarReadData, 
    UI_PBNState *pPbnState
)
{   
    UI_PBNState pbnState;

    if (pPbnState == NULL)
    {
        return UI_INV_PRM;
    }

#ifndef PBN_SIM
    switch (sarReadData >> SAR_SAMP_SHIFT) /* discard 2 LSBs in sampled voltage to account for variability in resistors and SAR supply voltage */
    {
        case SAR_SAMP_SW_NONE: 
            pbnState = UI_PUSH_BUTTON_NONE;
            break;
        case SAR_SAMP_SW06:
            pbnState = UI_PUSH_BUTTON_SW06;
            break;
        case SAR_SAMP_SW07:
            pbnState = UI_PUSH_BUTTON_SW07;
            break;
        case SAR_SAMP_SW08:
            pbnState = UI_PUSH_BUTTON_SW08;
            break;
        case SAR_SAMP_SW09:
            pbnState = UI_PUSH_BUTTON_SW09;
            break;
        case SAR_SAMP_SW10:
            pbnState = UI_PUSH_BUTTON_SW10;
            break;
        case SAR_SAMP_SW11:
            pbnState = UI_PUSH_BUTTON_SW11;
            break;
        case SAR_SAMP_SW12:
            pbnState = UI_PUSH_BUTTON_SW12;
            break;
        case SAR_SAMP_SW13:
            pbnState = UI_PUSH_BUTTON_SW13;
            break;
        case SAR_SAMP_SW14:
            pbnState = UI_PUSH_BUTTON_SW14;
            break;
        case SAR_SAMP_SW15:
            pbnState = UI_PUSH_BUTTON_SW15;
            break;
        default:
            pbnState = UI_PUSH_BUTTON_UNKNOWN;
            break;
    }

#else
    if (gPbnSimState.pbnSimDone == FALSE)
    {
        if (gPbnSimState.pollIntCnt == 0)
        {
            if (gPbnSimState.curTestPbnState < NUM_TEST_PBN_STATES-1)
            {
                gPbnSimState.curTestPbnState++;
                gPbnSimState.pollIntCnt = testPbnState[gPbnSimState.curTestPbnState].numPollInts;
            }
            else
            {
                if (gPbnSimState.pbnSimLoop == TRUE)
                {
                    gPbnSimState.curTestPbnState = 0;
                    gPbnSimState.pollIntCnt = testPbnState[gPbnSimState.curTestPbnState].numPollInts;
                }
                else
                {
                    gPbnSimState.pbnSimDone = TRUE;
                }
            }
        }

        if (gPbnSimState.pbnSimDone == FALSE)
        {
            pbnState = testPbnState[gPbnSimState.curTestPbnState].pbnState;
            gPbnSimState.pollIntCnt--;
        }
        else
        {
            pbnState = UI_PUSH_BUTTON_NONE;
        }
    }
    else
    {
        pbnState = UI_PUSH_BUTTON_NONE;
    }
#endif

    *pPbnState = pbnState;

    return UI_SOK;
}

/* Checks if current push-button network state is in allowed set of states */
Int16 chkAllowedPBNState(
    UI_PBNState pbnState, 
    UI_PBNState *allowedPbnStates, 
    Uint16 numAllowedPbnStates, 
    Int16 *pPbnStateAllowed
)
{
    Uint16 curAllowedPbnState;
    Int16 allowed;

    if ((allowedPbnStates == NULL) || (pPbnStateAllowed == NULL))
    {
        return UI_INV_PRM;
    }

    curAllowedPbnState = 0;
    allowed = 0;
    while (!allowed && (curAllowedPbnState < numAllowedPbnStates))
    {
        if (pbnState == allowedPbnStates[curAllowedPbnState])
        {
            allowed = 1;
        }
        else
        {
            curAllowedPbnState++;
        }
    }

    *pPbnStateAllowed = allowed;

    return UI_SOK;
}

/* Generates HID report */
void genHidReport(
    UI_PBNState pbnState, 
    Uint8 *pHidReport
)
{
    /* Construct default report */
    memset(pHidReport, 0, HID_REPORT_SIZE_WORDS);
    pHidReport[0] = HID_REPORT_ID;

    /* Add non-default report data */
    switch (pbnState)
    {
        case UI_MIC_MUTE:
            pHidReport[1] |= 0x1 << 2;
            break;
        case UI_PB_VOL_UP:
            pHidReport[0] |= 0x1 << (8+0);
            break;
        case UI_PB_VOL_DN:
            pHidReport[0] |= 0x1 << (8+1);
            break;
        case UI_PB_MUTE:
            pHidReport[0] |= 0x1 << (8+2);
            break;
        default :
            break;
    }
}

//extern Int16 gpio02State; // debug
void gpt1Isr(void)
{
    /* Clear TIAFR flag */
    CSL_SYSCTRL_REGS->TIAFR = 0x2;

    //gpio02State ^= 0x1; // debug
    //GpioWrite02(gpio02State);

    /* Perform USB frame rate processing */
    SWI_post(&SWI_UserInterface);
}

/* User Interface SWI */
void UserInterfaceSwi(void)
{
    Bool sarDataReady;
    Int16 status;
    Uint16 sarReadData;
    UI_PBNState pbnState;
    Int16 pbnStateAllowed;
    pUsbContext pContext;

    pContext = &gUsbContext;

    /* Check SAR status */
    sarDataReady = SAR_getStatus(gsSarHandle, &status); /* SAR data should always be ready since Timer frequency is very slow compared with SAR sampling frequency */
    if ((sarDataReady != CSL_SAR_DATA_AVAILABLE) || (status != CSL_SOK))
    {
        LOG_printf(&trace, "ERROR: SAR_getStatus()");
    }

    /* Read SAR data */
    status = SAR_readData(gsSarHandle, &sarReadData);
    if (status != CSL_SOK)
    {
        LOG_printf(&trace, "ERROR: SAR_readData()");
    }

    /* Start next conversion */
    status = SAR_startConversion(gsSarHandle);
    if (status != CSL_SOK)
    {
        LOG_printf(&trace, "ERROR: SAR_startConversion()");
    }

    /* Get current push-button network state */
    status = getCurrentPBNState(sarReadData, &pbnState);
    if (status != UI_SOK)
    {
        LOG_printf(&trace, "ERROR: getCurrentPBNState()");
    }
    //if (pbnState == UI_PUSH_BUTTON_UNKNOWN) LOG_printf(&trace, "pbnState = 0x%04X", pbnState);

    /* Check push-button network state is allowed state */
    status = chkAllowedPBNState(pbnState, gAllowedPbnStates, NUM_ALLOWED_PBN_STATES, &pbnStateAllowed);
    if (status != UI_SOK)
    {
        LOG_printf(&trace, "ERROR: chkAllowedPBNState()");
    }

    /* Filter disallowed push-button network states,  */
    /* Check change in push-button network state */
    if ((pbnStateAllowed) && (pbnState != gPrevPbnState))
    {
        /* Generate HID report */
        genHidReport(pbnState, gHidReport);

        gPrevPbnState = pbnState;
        gHidReportReady = TRUE;

        if (hidIntInEpReady == TRUE)
        {
            DeviceNotification(pContext, CSL_USB_EVENT_HID_REPORT_TX);
            gHidReportReady = FALSE;
            hidIntInEpReady = FALSE;
        }
    }
}

