/*
 * $$$MODULE_NAME app_usb.c
 *
 * $$$MODULE_DESC app_usb.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include "csl_usb.h"
#include "csl_usbAux.h"
#include "csl_audioClass.h"
#include "app_usb.h"
#include "app_usbac.h"
#include "i2s_sample.h"
#include "user_interface.h"
#include "codec_config.h"
#include "VC5505_CSL_BIOS_cfg.h"
#include "psp_i2s.h"
#include "app_asrc.h"

extern CSL_UsbContext  gUsbContext;
extern CSL_UsbRegsOvly usbRegisters;

/* USB connect/disconnect control variable for self-powered device */
static Bool usbDevDisconnect = FALSE;

/* Indicates whether EP2 Rx interrupt received since last SOF interrupt */
static Uint16 playIntrRcvd = 0;
/* Indicates whether EP2 Rx interrupt received since last SOF interrupt */
static Uint16 recIntrRcvd = 0;

Bool usb_rec_mode = FALSE;
Bool usb_rec_start = FALSE;

Bool usb_play_mode = FALSE;
Bool usb_play_start = FALSE;

Bool  no_main_task = FALSE;

/* HID endpoint control variable */
Bool hidIntInEpReady = FALSE;

/* Suspend callback function call count */
Uint32 sus = 0;
/* Self wakeup callback function call count */
Uint32 wake = 0;

Uint16 firstRecordFlag = TRUE;

#if 0 // debug
Uint16 gFaddr_PowerDbg;
Uint16 gRxFifoSz_TxFifoSzDbg;
Uint16 gTxFifoAddrDbg;
Uint16 gTxMaxpDbg;
Uint16 gTxCsrDbg;
Uint16 gRxFifoAddrDbg;
Uint16 gRxMaxpDbg;
Uint16 gRxCsrDbg;
Uint16 gCsr0Dbg;
Uint16 gSaveIndex;
#endif

static void USB_MUSB_Isr(void);
static void MainTask(void);

static void Delay(
    DWORD dwMicroSeconds
);


/**
 *  \brief  Function to start the USB device
 *
 *  \param  pContext - USB context structure pointer
 *
 *  \return TRUE  - Operation success
 *          FALSE - Invalid input parameter
 */
Bool StartDevice(pUsbContext pContext)
{
    pUsbEpStatus peps;
    DWORD dwEndpoint;

    if(pContext == NULL)
        return FALSE;

    if(!USB_checkSpeed(pContext, &pContext->busSpeed))
        return FALSE;

    for (dwEndpoint = 0; dwEndpoint < CSL_USB_ENDPOINT_COUNT; ++dwEndpoint)
    {
        if(peps && peps->fSelected)
        {
            USB_configEndpointDataSize(pContext, dwEndpoint);
        }
    }

    if(pContext->cableState != CSL_USB_DEVICE_ATTACH)
    {
        pContext->cableState = CSL_USB_DEVICE_ATTACH;
        DeviceNotification(pContext, CSL_USB_EVENT_RESUME);
    }

    /* Send Reset */
    DeviceNotification(pContext, CSL_USB_EVENT_RESET);

    return TRUE;
}

/**
 *  \brief  Function to stop the USB device
 *
 *  \param  pContext - USB context structure pointer
 *
 *  \return TRUE  - Operation success
 *          FALSE - Invalid input parameter
 */
Bool StopDevice(pUsbContext pContext)
{
    pUsbEpStatus peps;
    DWORD dwEndpoint;

    if(pContext == NULL)
        return FALSE;

    for (dwEndpoint = 0; dwEndpoint < CSL_USB_ENDPOINT_COUNT; ++dwEndpoint)
    {
        peps = &pContext->pEpStatus[dwEndpoint];
        if(peps && peps->fInitialized)
        {
            if(peps && peps->pTransfer)
            {
                peps->pTransfer->fComplete = TRUE;
                peps->pTransfer = NULL;
            }
            peps->fStalled = FALSE;
            peps->fInitialized = FALSE;
        }
    }

       pContext->cableState = CSL_USB_DEVICE_DETACH;

    DeviceNotification(pContext, CSL_USB_EVENT_SUSPEND);

    return TRUE;
}

/**
 *  \brief  USB interrupt handler
 *
 *  \param  None
 *
 *  \return None
 */
Uint32 isoInIntCount = 0;
Uint32 isoOutIntCount = 0;
Uint32 isoSofIntCount = 0;
Uint32 usbIntTotalCount = 0;
Bool HandleUSBInterrupt(
    pUsbContext pContext
)
{
	usbIntTotalCount++;
    /* If MUSB is not ready, do nothing */
    if(!pContext->fMUSBIsReady)
        return FALSE;

    if(pContext->dwIntSourceH & CSL_USB_GBL_INT_RESET)
    {
        /* These USB controller registers are reset by USB bus reset condition */

        /* Set Tx MaxP for record endpoint */
        USB_setTxMaxp(EP_NUM_REC, EP_REC_MAXP);
        /* Set ISO mode for record endpoint */
        USB_setTxCsr(EP_NUM_REC, 0x4000);

        /* Set Rx MaxP for playback endpoint */
        USB_setRxMaxp(EP_NUM_PLAY, EP_PB_MAXP);
        /* Set ISO mode for playback endpoint */
        USB_setRxCsr(EP_NUM_PLAY, 0x4000);

        /* Set Tx MaxP for HID interupt-IN endpoint */
        USB_setTxMaxp(EP_NUM_HID, EP_HID_MAXP);

#if 0
        /* Initialize HID interrupt endpoint ready indication */
        hidIntInEpReady = TRUE;

        /* Initialize user interface */
        status = userInterfaceInit(UI_DEF_CPU_FREQ, UI_DEF_SAR_SAMP_FREQ, &cslStatus);
        if (status != UI_SOK)
        {
            LOG_printf(&trace, "ERROR: userInterfaceInit()");
        }

        /* Start user interface */
        status = userInterfaceStart(&cslStatus);
        if (status != UI_SOK)
        {
            LOG_printf(&trace, "ERROR: userInterfaceStart()");
        }
#endif
    }

    if(pContext->dwIntSourceH & CSL_USB_GBL_INT_RESUME)
    {
        /* Resume interrupt */
        MUSB_Handle_Resume_Intr();
    }

    if(pContext->dwIntSourceL & CSL_USB_TX_RX_INT_EP0)
    {
        MUSB_Handle_EP0_Intr(pContext);

        /* post a message to the MSC task only when
          there is a setup packet available */
        if(pContext->fSetupPktCmd == TRUE)
        {
            pContext->fSetupPktCmd = FALSE;
            if (pContext->cableState == CSL_USB_DEVICE_DETACH)
            {
                if(!StartDevice(pContext))
                {
                  /* fExitMainTaskOnUSBError = TRUE; */
                }
            }
            DeviceNotification(pContext, CSL_USB_EVENT_SETUP);
        }
        else
        {
            //LOG_printf(&trace, "noset\n");
        }
    }

#if 0
    /* Interrupt OUT, HID Tx endpoint */
    if(pContext->dwIntSourceL & USB_TX_INT_EP_HID)
    {
        if (gHidReportReady == FALSE)
        {
            hidIntInEpReady = TRUE;
        }
        else
        {
            DeviceNotification(pContext, CSL_USB_EVENT_HID_REPORT_TX);
            gHidReportReady = FALSE;
            hidIntInEpReady = FALSE;
        }
    }
#endif

    if(pContext->dwIntSourceH & CSL_USB_GBL_INT_DEVDISCONN)
    {
        /* disconnect interrupt */
        USB_disconnectDev(CSL_USB0);
        usbDevDisconnect = TRUE;
    }

    if(pContext->dwIntSourceH & CSL_USB_GBL_INT_SUSPEND)
    {
        /* suspend interrupt */
        USB_suspendDevice(CSL_USB0);
    }

    /* all interrupts are handled, signal 'End Of Interrupt' */
    usbRegisters->EOIR = CSL_USB_EOIR_RESETVAL;

    return TRUE;
}

/**
 *  \brief  Function to handle Ep0 interrupts
 *
 *  \param  pContext - USB context structure pointer
 *
 *  \return None
 */
void  MUSB_Handle_EP0_Intr(pUsbContext pContext)
{

    pUsbEpStatus           peps;
    CSL_UsbSetupStruct  *usbSetup = &pContext->usbSetup;

    peps = &pContext->pEpStatus[CSL_USB_EP0];

    /* set index register = 0 to select EP0 */
    usbRegisters->INDEX_TESTMODE &= ~CSL_USB_INDEX_TESTMODE_EPSEL_MASK;
       usbRegisters->INDEX_TESTMODE = CSL_USB_EP0;

    /* check if STALLed -- error condition */
    if ((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_SENTSTALL_MASK) == 
        CSL_USB_PERI_CSR0_INDX_SENTSTALL_MASK)
    {
        usbRegisters->PERI_CSR0_INDX &= ~CSL_USB_PERI_CSR0_INDX_SENTSTALL_MASK;
        pContext->ep0State = CSL_USB_EP0_IDLE;
    }

    /* check if SetupEnd -- error condition */
    if ((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_SETUPEND_MASK) ==
        CSL_USB_PERI_CSR0_INDX_SETUPEND_MASK)
    {
        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_SETUPEND_MASK;
        pContext->ep0State = CSL_USB_EP0_IDLE;
    }

    if(pContext->ep0State == CSL_USB_EP0_STATUS_IN)
    {
       if(pContext->usbSetup.bRequest == CSL_USB_SET_ADDRESS)
        {
            /* ask the main task to process it */
            pContext->fSetupPktCmd = TRUE;
            peps->wUSBEvents |= CSL_USB_EVENT_SETUP;
        }
       pContext->ep0State = CSL_USB_EP0_IDLE;

       return;
    }

    if(CSL_USB_EP0_IDLE == pContext->ep0State)
    {
        if((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK)
            == CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK)
        {

            /* setup packet received */
            USB_getSetupPacket(CSL_USB0, usbSetup, TRUE);

            if (((pContext->usbSetup.bmRequestType & (CSL_USB_REQTYPE_CLASS | CSL_USB_REQTYPE_RCVR_INTERFACE)) == 
                (CSL_USB_REQTYPE_CLASS | CSL_USB_REQTYPE_RCVR_INTERFACE)) && 
                (pContext->usbSetup.wIndex == IF_NUM_HID)) /* HID Class-Specific Request */
            {
                switch (pContext->usbSetup.bRequest)
                {
                    case CSL_USB_GET_REPORT :
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
                        pContext->ep0State = CSL_USB_EP0_TX;

                        break;

                    case CSL_USB_GET_IDLE :
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
                        pContext->ep0State = CSL_USB_EP0_TX;

                        break;

                    case CSL_USB_GET_PROTOCOL :
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
                        pContext->ep0State = CSL_USB_EP0_TX;

                        break;

                    case CSL_USB_SET_REPORT :
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
                        pContext->ep0State = CSL_USB_EP0_RX;

                        break;

                    case CSL_USB_SET_IDLE :
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        /* DataEnd + ServicedRxPktRdy */
                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_DATAEND_MASK;
                        pContext->ep0State = CSL_USB_EP0_STATUS_IN;

                        break;

                    case CSL_USB_SET_PROTOCOL :
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        /* DataEnd + ServicedRxPktRdy */
                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_DATAEND_MASK;
                        pContext->ep0State = CSL_USB_EP0_STATUS_IN;

                        break;

                    default:
                        //LOG_printf(&trace, "ERROR: MUSB_Handle_EP0_Intr() request = 0x%04X", pContext->usbSetup.bRequest);
                        break;
                }
            }
            //else if (((pContext->usbSetup.bmRequestType & (CSL_USB_REQTYPE_CLASS | CSL_USB_REQTYPE_RCVR_INTERFACE)) ==
            //               (CSL_USB_REQTYPE_CLASS | CSL_USB_REQTYPE_RCVR_INTERFACE))) //AC Class-Specific Request
            else if ((pContext->usbSetup.bmRequestType & CSL_USB_REQTYPE_CLASS) ==
            				CSL_USB_REQTYPE_CLASS) //AC Class-Specific Request
            {
                switch(pContext->usbSetup.bRequest)
                {
                    case CSL_USB_CUR :
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
                        // get or set
                        if (pContext->usbSetup.bmRequestType & CSL_USB_REQTYPE_XFER_DTOH)
                          pContext->ep0State = CSL_USB_EP0_TX;
						else
							pContext->ep0State = CSL_USB_EP0_RX;
                        break;

                    case CSL_USB_RANGE :
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
                        // get or set
                        if (pContext->usbSetup.bmRequestType & CSL_USB_REQTYPE_XFER_DTOH)
                          pContext->ep0State = CSL_USB_EP0_TX;
						else
							pContext->ep0State = CSL_USB_EP0_RX;
                        break;

                    case CSL_USB_MEM :
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
                        // get or set
                        if (pContext->usbSetup.bmRequestType & CSL_USB_REQTYPE_XFER_DTOH)
                          pContext->ep0State = CSL_USB_EP0_TX;
						else
							pContext->ep0State = CSL_USB_EP0_RX;
                        break;

                    default:
                        //LOG_printf(&trace, "ERROR: MUSB_Handle_EP0_Intr - 0():\n");
                        LOG_printf(&trace, "ERROR: MUSB_Handle_EP0_Intr() request = 0x%04X", pContext->usbSetup.bRequest);
                        break;
                }
            }
            else /* Standard Request */
            {
                /* check the request type */
                switch(pContext->usbSetup.bRequest)
                {
                    /* zero data */
                    case CSL_USB_SET_FEATURE:
                    case CSL_USB_CLEAR_FEATURE:
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        /* DataEnd + ServicedRxPktRdy */
                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
                        		CSL_USB_PERI_CSR0_INDX_DATAEND_MASK;
                          pContext->ep0State = CSL_USB_EP0_STATUS_IN;
                        /* no state change */
                        break;

                    case CSL_USB_SET_INTERFACE:
                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        /* DataEnd + ServicedRxPktRdy */
                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
                        		CSL_USB_PERI_CSR0_INDX_DATAEND_MASK;
                          pContext->ep0State = CSL_USB_EP0_STATUS_IN;
                        /* no state change */
                        break;

                    case CSL_USB_SET_CONFIGURATION:

                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        /* DataEnd + ServicedRxPktRdy */
                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
                        		CSL_USB_PERI_CSR0_INDX_DATAEND_MASK;
                          pContext->ep0State = CSL_USB_EP0_STATUS_IN;
                        /* no state change */
                        break;

                    case CSL_USB_SET_ADDRESS:

                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK |
                        	CSL_USB_PERI_CSR0_INDX_DATAEND_MASK;
                           pContext->ep0State = CSL_USB_EP0_STATUS_IN;
                        /* no state change */
                        break;

                    case CSL_USB_SET_DESCRIPTOR:

                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        /* ServicedRxPktRdy */
                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;

                        /* write request; move to rx state */
                        pContext->ep0State = CSL_USB_EP0_RX;
                        break;

                    case CSL_USB_GET_CONFIGURATION:
                    case CSL_USB_GET_INTERFACE:
                    case CSL_USB_GET_DESCRIPTOR:
                    case CSL_USB_GET_STATUS:
                    case CSL_USB_SYNCH_FRAME:
                    case CSL_USB_GET_MAX_LUN:

                        /* ask the main task to process it */
                        pContext->fSetupPktCmd = TRUE;
                        peps->wUSBEvents |= CSL_USB_EVENT_SETUP;

                        /* ServicedRxPktRdy */

                        /* read request; move to tx state */
                        pContext->ep0State = CSL_USB_EP0_TX;
                        usbRegisters->PERI_CSR0_INDX |= CSL_USB_PERI_CSR0_INDX_SERV_RXPKTRDY_MASK;
                    break;

                    default:
                        /* something's wrong */
                        //LOG_printf(&trace, "ERROR: MUSB_Handle_EP0_Intr - 1():\n");
                        break;
                }
            }
        }
        return;
    }

    if(CSL_USB_EP0_TX == pContext->ep0State)
    {
        pContext->fWaitingOnEP0BUFAvail = TRUE;
        pContext->fEP0BUFAvailable = TRUE;
        return;
    }

    if(CSL_USB_EP0_RX == pContext->ep0State)
    {
        if((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK) ==
         CSL_USB_PERI_CSR0_INDX_RXPKTRDY_MASK)
        {
            USB_readEP0Buf(pContext, pContext->wOutEP0Buf);
            pContext->fOutPhaseCmd = TRUE;

            /* check if data end bit is set */
            if((usbRegisters->PERI_CSR0_INDX & CSL_USB_PERI_CSR0_INDX_DATAEND_MASK) ==
            CSL_USB_PERI_CSR0_INDX_DATAEND_MASK)
            {
                pContext->ep0State = CSL_USB_EP0_IDLE;
            }
            /* else remain in rx state */
        }
        else
        {
            //LOG_printf(&trace, "ERROR: MUSB_Handle_EP0_Intr - 2():\n");
        }
        return;
    }

}

/**
 *  \brief  function to handle resume interrupt
 *
 *  \param  None
 *
 *  \return None
 */
void MUSB_Handle_Resume_Intr()
{
    /* set the resume bit */
    usbRegisters->FADDR_POWER |= CSL_USB_FADDR_POWER_RESUME_MASK;

    /* wait for 10ms for musb to generate resume signaling on the bus */
    Delay(10*1);

    /* clear the resume bit */
    usbRegisters->FADDR_POWER &= ~(CSL_USB_FADDR_POWER_RESUME_MASK);
}

/**
 *  \brief  USB suspend call back function
 *
 *  \param  none
 *
 *  \return Test result
 */
void CSL_suspendCallBack(CSL_Status    status)
{
    sus++;
}

/**
 *  \brief  USB self wakeup call back function
 *
 *  \param  none
 *
 *  \return Test result
 */
void CSL_selfWakeupCallBack(CSL_Status    status)
{
    wake++;
}

/**
 *  \brief  Function to start USB data transfer
 *
 *  \param  pContext     - Pointer to the global MUSB contoller context structure
 *  \param  peps         - Endpoint status structure pointer
 *
 *  \return TRUE  - Operation successful
 *          FALSE - Invalid input parameters
 */
CSL_Status StartTransfer(void    *vpContext,
                         void    *vpeps)
{
    pUsbContext      pContext;
    pUsbEpStatus     peps;
    pUsbTransfer     pTransfer;
    CSL_UsbMsgObj     USBMsg;

    pContext  = (pUsbContext)vpContext;
    peps      = (pUsbEpStatus)vpeps;
    pTransfer = peps->pTransfer;

    if((pContext == NULL) || (peps == NULL))
    {
        return(CSL_ESYS_INVPARAMS);
    }

    if(!pContext->fMUSBIsReady)
    {
        return(CSL_ESYS_INVPARAMS);
    }

    /* The endpoint should be initialized */
    if(!peps->fInitialized)
    {
        return(CSL_ESYS_INVPARAMS);
    }

    peps->pTransfer = pTransfer;
    pTransfer->fComplete=FALSE;
    if (pTransfer->dwFlags == CSL_USB_OUT_TRANSFER)
    {
        if(peps->dwEndpoint == EP_NUM_PLAY)
        {
            pContext->fWaitingOnFlagB = TRUE;
        }
        else
        {
            //pContext->fOutPhaseCmd = TRUE;
        }
    }
    else if (pTransfer->dwFlags == CSL_USB_IN_TRANSFER)
    {
        if (peps->dwEndpoint == CSL_USB_EP0)
        {
            pContext->fWaitingOnEP0BUFAvail = TRUE;
            if(peps->pTransfer->cbBuffer)
            {
                pContext->fEP0BUFAvailable = TRUE;
            }
        }
        else if (peps->dwEndpoint == EP_NUM_REC)
        {
            // set flag once SOF is received
            pContext->fWaitingOnFlagA = TRUE;
            pContext->fEP1InBUFAvailable = TRUE;
        }
        else if (peps->dwEndpoint == EP_NUM_HID)
        {
            pContext->fWaitingOnFlagA = TRUE;
            pContext->fEP3InBUFAvailable = TRUE;
        }
    }

    if(no_main_task == FALSE)
    {
        /* Just inform the main task */
          USBMsg.wMsg = CSL_USB_MSG_DATA;
        /* enqueue message */
          MBX_post(&MBX_musb, &USBMsg, SYS_FOREVER);
    }
    return(CSL_SOK);
}

/**
 *  \brief  Function to complete the Audio Class data transfer
 *
 *  \param  pContext  - USB context structure pointer
 *  \param  peps      - End point status structure pointer
 *
 *  \return None
 */
Uint32 completeTransCount = 0;
CSL_Status CompleteTransfer(
    void    *vpContext,
    void    *vpeps
)
{
    pUsbContext     pContext;
    pUsbEpStatus    peps;
    CSL_UsbMscMsg   wMSCMsg;
    pUsbTransfer    pTransfer;

    pContext  = (pUsbContext)vpContext;
    peps      = (pUsbEpStatus)vpeps;
    pTransfer = peps->pTransfer;

    if((pTransfer != NULL) && (pContext != NULL))
    {
         pTransfer->fComplete = TRUE;
         peps->pTransfer = NULL;
    }

    peps->wUSBEvents |= CSL_USB_EVENT_EOT;

	completeTransCount++;

    switch(peps->dwEndpoint)
    {
         case  CSL_USB_EP0:
             wMSCMsg = CSL_USB_MSG_MSC_CTL;
               /* enqueue message */
               MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
               break;

         case  EP_NUM_REC:
              ///MWwMSCMsg = CSL_USB_MSG_ISO_IN;
              /* enqueue message */
              ///MWMBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
              break;

         case  EP_NUM_PLAY:
              peps->wUSBEvents |= CSL_USB_EVENT_WRITE_MEDIA;
              wMSCMsg = CSL_USB_MSG_ISO_OUT;
              /* enqueue message */
              MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
              break;

         case  EP_NUM_HID:
              wMSCMsg = CSL_USB_MSG_HID_INT_IN;
              /* enqueue message */
              MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
              break;

         default:
              break;
    }

    return(CSL_SOK);
}

/**
 *  \brief  Function to send device notification
 *
 *  \param  pContext    - USB context structure pointer
 *  \param  wUSBEvents  - USB events
 *
 *  \return None
 */
Uint32 deviceNotifyCount = 0;
Uint32 isoTxCount = 0;
Uint32 isoRxCount = 0;
Uint16 stopMsgMscCtlFlag = 0;
void DeviceNotification(
    pUsbContext    pContext,
    WORD           wUSBEvents
)
{
    CSL_UsbMscMsg     wMSCMsg;
    pUsbEpStatus      peps;

    peps = &pContext->pEpStatus[CSL_USB_EP0];
    peps->wUSBEvents |= wUSBEvents;

	deviceNotifyCount++;

    if (isoTxCount<8000)
	{
	    wMSCMsg = CSL_USB_MSG_MSC_CTL;
	    /* enqueue message */
	    MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
	}

    if(wUSBEvents & CSL_USB_EVENT_RESET)
    {
         peps = &pContext->pEpStatus[EP_NUM_REC];
         peps->wUSBEvents |= wUSBEvents;

         wMSCMsg = CSL_USB_MSG_ISO_IN;
         /* enqueue message */
         MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);

         peps = &pContext->pEpStatus[EP_NUM_PLAY];
         peps->wUSBEvents |= wUSBEvents;

         wMSCMsg = CSL_USB_MSG_ISO_OUT;
         /* enqueue message */
         MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);

         peps = &pContext->pEpStatus[EP_NUM_HID];
         peps->wUSBEvents |= wUSBEvents;

         wMSCMsg = CSL_USB_MSG_HID_INT_IN;
         /* enqueue message */
         MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
    }
    else if (wUSBEvents & CSL_USB_EVENT_ISO_RX)
    {
         peps = &pContext->pEpStatus[EP_NUM_PLAY];
         peps->wUSBEvents |= wUSBEvents;

		 isoRxCount++;
         wMSCMsg = CSL_USB_MSG_ISO_OUT;
         /* enqueue message */
         MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
    }
    else if (wUSBEvents & CSL_USB_EVENT_ISO_TX)
    {
         peps = &pContext->pEpStatus[EP_NUM_REC];
         peps->wUSBEvents |= CSL_USB_EVENT_READ_MEDIA;
         peps->wUSBEvents |= wUSBEvents;

		 isoTxCount++;
         wMSCMsg = CSL_USB_MSG_ISO_IN;
         /* enqueue message */
         MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
    }
    else if (wUSBEvents & CSL_USB_EVENT_HID_REPORT_TX)
    {
        peps = &pContext->pEpStatus[EP_NUM_HID];
        peps->wUSBEvents |= wUSBEvents;

         wMSCMsg = CSL_USB_MSG_HID_INT_IN;
         /* enqueue message */
         MBX_post(&MBX_msc, &wMSCMsg, SYS_FOREVER);
    }
}

/**
 *  \brief  Function to start USB data transfer
 *
 *  \param  pContext     - Pointer to the global MUSB contoller context structure
 *  \param  dwEndpoint   - Endpoint Number
 *
 *  \return TRUE  - Operation successful
 *          FALSE - Invalid input parameters
 */
void USB_configEndpointDataSize(
    pUsbContext pContext,
    DWORD       dwEndpoint
)
{
    pUsbEpStatus    peps;

    peps = &pContext->pEpStatus[dwEndpoint];

    switch(dwEndpoint)
    {
        case CSL_USB_EP0:
            //peps->dwPacketSizeAssigned = CSL_USB_EP0_PACKET_SIZE;
            break;

        case CSL_USB_EP1:
        switch(peps->xferType)
            {
        case CSL_USB_BULK:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP1_PACKET_SIZE_HS : CSL_USB_EP1_PACKET_SIZE_FS;
            break;
        case CSL_USB_INTR:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP_MAX_PACKET_SIZE_INT_HS : CSL_USB_EP_MAX_PACKET_SIZE_INT_FS;
            break;
        case CSL_USB_ISO:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP_MAX_PACKET_SIZE_ISO_HS : CSL_USB_EP_MAX_PACKET_SIZE_ISO_FS;
            break;
        default:
            break;
        }

        break;

    case CSL_USB_EP2:
        switch(peps->xferType)
        {
        case CSL_USB_BULK:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP2_PACKET_SIZE_HS : CSL_USB_EP2_PACKET_SIZE_FS;
            break;
        case CSL_USB_INTR:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP_MAX_PACKET_SIZE_INT_HS : CSL_USB_EP_MAX_PACKET_SIZE_INT_FS;
            break;
        case CSL_USB_ISO:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP_MAX_PACKET_SIZE_ISO_HS : CSL_USB_EP_MAX_PACKET_SIZE_ISO_FS;
            break;
        default:
            break;
        }

        break;

    case CSL_USB_EP3:
        switch(peps->xferType)
        {
        case CSL_USB_BULK:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP3_PACKET_SIZE_HS : CSL_USB_EP3_PACKET_SIZE_FS;
            break;
        case CSL_USB_INTR:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP_MAX_PACKET_SIZE_INT_HS : CSL_USB_EP_MAX_PACKET_SIZE_INT_FS;
            break;
        case CSL_USB_ISO:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP_MAX_PACKET_SIZE_ISO_HS : CSL_USB_EP_MAX_PACKET_SIZE_ISO_FS;
            break;
        default:
            break;
            }

            break;

        case CSL_USB_EP4:
        switch(peps->xferType)
            {
        case CSL_USB_BULK:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP4_PACKET_SIZE_HS : CSL_USB_EP4_PACKET_SIZE_FS;
            break;
        case CSL_USB_INTR:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP_MAX_PACKET_SIZE_INT_HS : CSL_USB_EP_MAX_PACKET_SIZE_INT_FS;
            break;
        case CSL_USB_ISO:
            peps->dwPacketSizeAssigned = (pContext->busSpeed == CSL_USB_BS_HIGH_SPEED) ? CSL_USB_EP_MAX_PACKET_SIZE_ISO_HS : CSL_USB_EP_MAX_PACKET_SIZE_ISO_FS;
            break;
        default:
            break;
        }

        break;

    default:
        break;
    }
}

/* Reads RXMAXP for selected endpoint */
Uint16 USB_getRxMaxp(Uint16 epNum)
{
    Uint16 saveIndex, rxMaxp;

    /* save the index register value */
    saveIndex = usbRegisters->INDEX_TESTMODE;

    CSL_FINS(usbRegisters->INDEX_TESTMODE,
             USB_INDEX_TESTMODE_EPSEL, epNum);

    rxMaxp = usbRegisters->RXMAXP_INDX;

    /* restore the index register */
    usbRegisters->INDEX_TESTMODE = saveIndex;

    return rxMaxp;
}

/* Writes RXMAXP for selected endpoint */
Uint16 USB_setRxMaxp(Uint16 epNum, Uint16 val)
{
    Uint16 saveIndex;

    /* save the index register value */
    saveIndex = usbRegisters->INDEX_TESTMODE;

    CSL_FINS(usbRegisters->INDEX_TESTMODE,
             USB_INDEX_TESTMODE_EPSEL, epNum);

    usbRegisters->RXMAXP_INDX = val;

    /* restore the index register */
    usbRegisters->INDEX_TESTMODE = saveIndex;

    return 0;
}

/* Reads TXCSR for selected endpoint */
Uint16 USB_getTxCsr(Uint16 epNum)
{
    Uint16 saveIndex, txCsr;

    /* save the index register value */
    saveIndex = usbRegisters->INDEX_TESTMODE;

    CSL_FINS(usbRegisters->INDEX_TESTMODE,
             USB_INDEX_TESTMODE_EPSEL, epNum);

    txCsr = usbRegisters->PERI_CSR0_INDX;

    /* restore the index register */
    usbRegisters->INDEX_TESTMODE = saveIndex;

    return txCsr;
}

/* Writes TXCSR for selected endpoint */
Uint16 USB_setTxCsr(Uint16 epNum, Uint16 val)
{
    Uint16 saveIndex;

    /* save the index register value */
    saveIndex = usbRegisters->INDEX_TESTMODE;

    CSL_FINS(usbRegisters->INDEX_TESTMODE,
             USB_INDEX_TESTMODE_EPSEL, epNum);

    usbRegisters->PERI_CSR0_INDX = val;

    /* restore the index register */
    usbRegisters->INDEX_TESTMODE = saveIndex;

    return 0;
}

/* Reads RXCSR for selected endpoint */
Uint16 USB_getRxCsr(Uint16 epNum)
{
    Uint16 saveIndex, rxCsr;

    /* save the index register value */
    saveIndex = usbRegisters->INDEX_TESTMODE;

    CSL_FINS(usbRegisters->INDEX_TESTMODE,
             USB_INDEX_TESTMODE_EPSEL, epNum);

    rxCsr = usbRegisters->PERI_RXCSR_INDX;

    /* restore the index register */
    usbRegisters->INDEX_TESTMODE = saveIndex;

    return rxCsr;
}

/* Reads RXCSR for selected endpoint */
Uint16 USB_setRxCsr(Uint16 epNum, Uint16 val)
{
    Uint16 saveIndex;

    /* save the index register value */
    saveIndex = usbRegisters->INDEX_TESTMODE;

    CSL_FINS(usbRegisters->INDEX_TESTMODE,
             USB_INDEX_TESTMODE_EPSEL, epNum);

    usbRegisters->PERI_RXCSR_INDX = val;

    /* restore the index register */
    usbRegisters->INDEX_TESTMODE = saveIndex;

    return 0;
}

/***********************************************************************
 * get the USB data from the endpoint buffer  - called by SWI_Store_USB_Input
 ***********************************************************************/
Uint32 swiStoreCount = 0;
Uint16 rxPbSampNum;
Uint16 rxPacketSize;
///Int16 rxPbSampNumLogIdx = 0;
///Int16 rxPbSampNumLog[300];
void store_USB_Input(void)
{
    pUsbContext     pContext;
    pUsbEpStatus     peps;
    pAcClassHandle  pAcClassHdl;
    CSL_AcObject    *pAcHandle;
    CSL_Status      status;
    pUsbTransfer    pTransfer;
    ///DWORD              cbBuffer;
    volatile ioport Uint16    *pFifoAddr;
    Uint16              saveIndex;
    volatile Uint16     looper;
	Uint16			tempWd1;

    pAcClassHdl = AC_AppHandle.pAcObj;
    pAcHandle   = &pAcClassHdl->acHandle;

    no_main_task = TRUE;
#ifdef SAMP_24BIT
#ifdef USE_FOUR_CODEC
    status = USB_postTransaction(&pAcHandle->isoOutEpObj, numPbSamps*16,
                     &pAcHandle->lbaBufferPb[0],
                     CSL_USB_IOFLAG_NOSHORT);
#else
#ifdef USE_THREE_CODEC
    status = USB_postTransaction(&pAcHandle->isoOutEpObj, numPbSamps*12,
                     &pAcHandle->lbaBufferPb[0],
                     CSL_USB_IOFLAG_NOSHORT);
#else
#ifdef USE_TWO_CODEC
    status = USB_postTransaction(&pAcHandle->isoOutEpObj, numPbSamps*8,
                     &pAcHandle->lbaBufferPb[0],
                     CSL_USB_IOFLAG_NOSHORT);
#else
    status = USB_postTransaction(&pAcHandle->isoOutEpObj, numPbSamps*4,
                     &pAcHandle->lbaBufferPb[0],
                     CSL_USB_IOFLAG_NOSHORT);
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#else
#ifdef USE_FOUR_CODEC
    status = USB_postTransaction(&pAcHandle->isoOutEpObj, numPbSamps*8,
                     &pAcHandle->lbaBufferPb[0],
                     CSL_USB_IOFLAG_NOSHORT);
#else
#ifdef USE_THREE_CODEC
    status = USB_postTransaction(&pAcHandle->isoOutEpObj, numPbSamps*6,
                     &pAcHandle->lbaBufferPb[0],
                     CSL_USB_IOFLAG_NOSHORT);
#else
#ifdef USE_TWO_CODEC
    status = USB_postTransaction(&pAcHandle->isoOutEpObj, numPbSamps*4,
                     &pAcHandle->lbaBufferPb[0],
                     CSL_USB_IOFLAG_NOSHORT);
#else
    status = USB_postTransaction(&pAcHandle->isoOutEpObj, numPbSamps*2,
                     &pAcHandle->lbaBufferPb[0],
                     CSL_USB_IOFLAG_NOSHORT);
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#endif //SAMP_24BIT
    no_main_task = FALSE;

    if(status != CSL_SOK)
    {
        LOG_printf(&trace, "ERROR: store_USB_Input() failed: %d\n", status);
        return;
    }

    pContext = &gUsbContext;

    /* Get the Endpoint that has the data received from the host */
    peps = &pContext->pEpStatus[EP_NUM_PLAY];

    pTransfer = peps->pTransfer;
    // number of bytes to copy
    ///cbBuffer = pTransfer->cbBuffer;
    // destination buffer
    ///pwBuffer = (WORD*)pTransfer->pvBuffer;
    // source (endpoint) buffer
    pFifoAddr   = (volatile ioport Uint16*)peps->pFifoAddr;
    #if ((defined(CSL_MSC_TEST)) || (defined(CSL_AC_TEST)))
    /* TI MSC put the DATA length as the first WORD, bypass it */
    ///pwBuffer++;
    #endif

	// get the RX packet byte count from USB Host OUT
	rxPacketSize = usbRegisters->EPCSR[EP_NUM_PLAY-1].RXCOUNT&0x1FFF;
#ifdef SAMP_24BIT
#ifdef USE_FOUR_CODEC
	numPbSamps = rxPbSampNum = rxPacketSize/16;
#endif //USE_FOUR_CODEC
#ifdef USE_THREE_CODEC
	numPbSamps = rxPbSampNum = rxPacketSize/12;
#endif //USE_THREE_CODEC
#ifdef USE_TWO_CODEC
	numPbSamps = rxPbSampNum = rxPacketSize/8;
#endif //USE_TWO_CODEC
#if (!defined(USE_FOUR_CODEC))&&(!defined(USE_THREE_CODEC))&&(!defined(USE_TWO_CODEC))
	numPbSamps = rxPbSampNum = rxPacketSize/4;
#endif // (!defined(USE_FOUR_CODEC))&&(!defined(USE_THREE_CODEC))&&(!defined(USE_TWO_CODEC))
    for(looper = 0; looper < numPbSamps; looper++)
    {
        /* Copy data from the USb FIFO */
		// adjust the word order 
        tempWd1 = *pFifoAddr;
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
        codec_output_buffer[codec_output_buffer_input_index] = tempWd1;
		codec_output_buffer_input_index++;
#ifdef USE_TWO_CODEC
		// adjust the word order 
        tempWd1 = *pFifoAddr;
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
        codec_output_buffer[codec_output_buffer_input_index] = tempWd1;
		codec_output_buffer_input_index++;
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
		// adjust the word order 
        tempWd1 = *pFifoAddr;
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
        codec_output_buffer[codec_output_buffer_input_index] = tempWd1;
		codec_output_buffer_input_index++;

		// adjust the word order 
        tempWd1 = *pFifoAddr;
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
        codec_output_buffer[codec_output_buffer_input_index] = tempWd1;
		codec_output_buffer_input_index++;
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
		// adjust the word order 
        tempWd1 = *pFifoAddr;
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
        codec_output_buffer[codec_output_buffer_input_index] = tempWd1;
		codec_output_buffer_input_index++;

		// adjust the word order 
        tempWd1 = *pFifoAddr;
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
        codec_output_buffer[codec_output_buffer_input_index] = tempWd1;
		codec_output_buffer_input_index++;

		// adjust the word order 
        tempWd1 = *pFifoAddr;
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
        codec_output_buffer[codec_output_buffer_input_index] = tempWd1;
		codec_output_buffer_input_index++;
#endif //USE_FOUR_CODEC
		// check for loop around
	    if (codec_output_buffer_input_index >= CODEC_OUTPUT_BUFFER_SIZE)
	    {
	        codec_output_buffer_input_index = 0;
	    }
    }
#else
#ifdef USE_FOUR_CODEC
	numPbSamps = rxPbSampNum = rxPacketSize/8;
#endif //USE_FOUR_CODEC
#ifdef USE_THREE_CODEC
	numPbSamps = rxPbSampNum = rxPacketSize/6;
#endif //USE_THREE_CODEC
#ifdef USE_TWO_CODEC
	numPbSamps = rxPbSampNum = rxPacketSize/4;
#endif //USE_TWO_CODEC
#if (!defined(USE_FOUR_CODEC))&&(!defined(USE_THREE_CODEC))&&(!defined(USE_TWO_CODEC))
	numPbSamps = rxPbSampNum = rxPacketSize/2;
#endif // (!defined(USE_FOUR_CODEC))&&(!defined(USE_THREE_CODEC))&&(!defined(USE_TWO_CODEC))
    for(looper = 0; looper < numPbSamps; looper++)
    {
        /* Copy data from the USb FIFO */
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
#ifdef USE_TWO_CODEC
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;

        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;

        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;

        codec_output_buffer[codec_output_buffer_input_index] = *pFifoAddr;
		codec_output_buffer_input_index++;
#endif //USE_FOUR_CODEC
		// check for loop around
	    if (codec_output_buffer_input_index >= CODEC_OUTPUT_BUFFER_SIZE)
	    {
	        codec_output_buffer_input_index = 0;
	    }
    }
#endif //SAMP_24BIT
    
	// increase the codec_output_buffer uFrame; check for overflow    
		if ((usb_play_mode)||(usb_play_start))
		{
		// increase sample counter (mono)
		codec_output_buffer_sample += (numPbSamps>>1);
		swiStoreCount++;
		// does codec_output_buffer overflow
		if (codec_output_buffer_sample>(MAX_TXBUFF_SZ_DACSAMPS*CODEC_OUTPUT_SZ_MSEC))
    {
			codec_output_buffer_overflow++;
		}
    }

    pTransfer->fComplete = TRUE;
    pContext->fWaitingOnFlagB = FALSE;
    /* clear the rxpktrdy bit */
    /* save the index register value */
    saveIndex = usbRegisters->INDEX_TESTMODE;

    CSL_FINS(usbRegisters->INDEX_TESTMODE,
                USB_INDEX_TESTMODE_EPSEL,
                CSL_USB_INDEX_TESTMODE_EPSEL_RESETVAL);

    CSL_FINS(usbRegisters->INDEX_TESTMODE,
                USB_INDEX_TESTMODE_EPSEL, EP_NUM_PLAY);

    if(peps->xferType == CSL_USB_ISO)
    {
        /* Clear the data toggle bit */
        usbRegisters->PERI_RXCSR_INDX |= (CSL_USB_PERI_RXCSR_CLRDATATOG_MASK);
    }

    CSL_FINS(usbRegisters->PERI_RXCSR_INDX,
                USB_PERI_RXCSR_RXPKTRDY, FALSE);

    /* restore the index register */
    usbRegisters->INDEX_TESTMODE = saveIndex;

    *(PWORD)pTransfer->pvBuffer = pTransfer->cbTransferred;

    // process the USB data
 	process_usb_input();	// call the process_usb_input directly
}

/***********************************************************************
 * sent the USB data to the endpoint buffer  - called by SWI_Send_USB_Output
 ***********************************************************************/
Uint32 swiSendCount = 0;
Uint32 underRunCount = 0;
Uint32 fifoEmptyCount = 0;
void send_USB_Output(void)
{
    pUsbContext     pContext;
    pUsbEpStatus     peps;
    volatile ioport Uint16    *pFifoAddr;
    Uint16              saveIndex;
    volatile Uint16     looper;
	Uint16			txCsr;
	register Uint16			tempWord1;
	register Uint16			tempWord2;
	volatile Uint16		i, pktCount;

    pContext = &gUsbContext;

    /* Send the recorded data to the host */
    peps = &pContext->pEpStatus[EP_NUM_REC];

    // source (endpoint) buffer
    pFifoAddr   = (volatile ioport Uint16*)peps->pFifoAddr;

	/* save the index register value */
	saveIndex = usbRegisters->INDEX_TESTMODE;

	/* Select the end point for data transfer */
	CSL_FINS(usbRegisters->INDEX_TESTMODE,
			 USB_INDEX_TESTMODE_EPSEL,
			 CSL_USB_INDEX_TESTMODE_EPSEL_RESETVAL);

	CSL_FINS(usbRegisters->INDEX_TESTMODE,
			 USB_INDEX_TESTMODE_EPSEL,
			 EP_NUM_REC);

	/* Flush the FIFO */
	txCsr = usbRegisters->PERI_CSR0_INDX;

	// if it is underrun or the first packet for transmitting then flush FIFO
	if ((txCsr&0x0004)==0x0004)
	{
#if 1
		CSL_FINS(usbRegisters->PERI_CSR0_INDX,
				 USB_PERI_TXCSR_FLUSHFIFO, TRUE);
		// if it is ISO mode flush the FIFO twice, because it is double buffered
		if(peps->xferType == CSL_USB_ISO)
		{
			CSL_FINS(usbRegisters->PERI_CSR0_INDX,
					 USB_PERI_TXCSR_FLUSHFIFO, TRUE);
		}
#endif
		underRunCount++;
		// clear the underrun flag
		usbRegisters->PERI_CSR0_INDX &= ~(0x0004);
	}

	// if the USB TX FIFO is not empty.
	if ((txCsr&0x0002)==0x0002)
	{
		// clear the FIFO not empty bit
		usbRegisters->PERI_CSR0_INDX &= ~(0x0002);
		// we need send one packet
		pktCount = 1;
	} else
	{
		fifoEmptyCount++;
		// we need send two packets for double buffering
		pktCount = 2;
	}

	// packet loop (one or two packet depending on the FIFO emptiness)
	//for (i=0; i<pktCount; i++)
	{
	// do we have data to send to USB host
	if (codec_input_buffer_sample==0)
    {
        codec_input_buffer_underflow++;
        //LOG_printf(&trace, "ERROR: codec_input_buffer UNDERFLOW");
#ifdef SAMP_24BIT
	    for(looper = 0; looper < numRecSamps; looper++)
	    {
	        /* Copy zero to the USB FIFO */
#ifdef USE_FOUR_CODEC
		       *pFifoAddr = 0;
			   *pFifoAddr = 0;
		
		       *pFifoAddr = 0;
			   *pFifoAddr = 0;
		
		       *pFifoAddr = 0;
			   *pFifoAddr = 0;

		       *pFifoAddr = 0;
			   *pFifoAddr = 0;
#else
#ifdef USE_THREE_CODEC
	       *pFifoAddr = 0;
		   *pFifoAddr = 0;
	
	       *pFifoAddr = 0;
		   *pFifoAddr = 0;
	
	       *pFifoAddr = 0;
		   *pFifoAddr = 0;
#else
#ifdef USE_TWO_CODEC
	       *pFifoAddr = 0;
		   *pFifoAddr = 0;
	
	       *pFifoAddr = 0;
		   *pFifoAddr = 0;
#else
	       *pFifoAddr = 0;
		   *pFifoAddr = 0;
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
	    }
#else
	    for(looper = 0; looper < numRecSamps; looper++)
	    {
	        /* Copy zero to the USB FIFO */
#ifdef USE_FOUR_CODEC
		       *pFifoAddr = 0;
		       *pFifoAddr = 0;
		       *pFifoAddr = 0;
		       *pFifoAddr = 0;
#else
#ifdef USE_THREE_CODEC
	       *pFifoAddr = 0;
	       *pFifoAddr = 0;
	       *pFifoAddr = 0;
#else
#ifdef USE_TWO_CODEC
	       *pFifoAddr = 0;
	       *pFifoAddr = 0;
#else
	       *pFifoAddr = 0;
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
	    }
#endif //SAMP_24BIT
    } else
	{
#ifdef SAMP_24BIT
    for(looper = 0; looper < numRecSamps; looper++)
    {
#ifdef USE_FOUR_CODEC
		        /* Copy data to the USB FIFO */
				// adjust the word order 
		       tempWord1 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       tempWord2 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       *pFifoAddr = tempWord2;
			   *pFifoAddr = tempWord1;
		
				// adjust the word order 
		       tempWord1 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       tempWord2 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       *pFifoAddr = tempWord2;
			   *pFifoAddr = tempWord1;
		
				// adjust the word order 
		       tempWord1 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       tempWord2 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       *pFifoAddr = tempWord2;
			   *pFifoAddr = tempWord1;

				// adjust the word order 
		       tempWord1 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       tempWord2 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       *pFifoAddr = tempWord2;
			   *pFifoAddr = tempWord1;
#else
#ifdef USE_THREE_CODEC
        /* Copy data to the USB FIFO */
		// adjust the word order 
		       tempWord1 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       tempWord2 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
       *pFifoAddr = tempWord2;
	   *pFifoAddr = tempWord1;

		// adjust the word order 
		       tempWord1 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       tempWord2 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
       *pFifoAddr = tempWord2;
	   *pFifoAddr = tempWord1;

		// adjust the word order 
		       tempWord1 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       tempWord2 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
       *pFifoAddr = tempWord2;
	   *pFifoAddr = tempWord1;
#else
#ifdef USE_TWO_CODEC
		// adjust the word order 
		       tempWord1 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       tempWord2 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
       *pFifoAddr = tempWord2;
	   *pFifoAddr = tempWord1;

		// adjust the word order 
		       tempWord1 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       tempWord2 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
       *pFifoAddr = tempWord2;
	   *pFifoAddr = tempWord1;
#else
        /* Copy data to the USB FIFO */
		// adjust the word order 
		       tempWord1 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       tempWord2 = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
       *pFifoAddr = tempWord2;
	   *pFifoAddr = tempWord1;
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
				// check for loop around
			    if (codec_input_buffer_output_index >= CODEC_INPUT_BUFFER_SIZE)
			    {
			        codec_input_buffer_output_index = 0;
    }
		    }
#else // SAMP_24BIT
    for(looper = 0; looper < numRecSamps; looper++)
    {
#ifdef USE_FOUR_CODEC
		        /* Copy data from the USB FIFO */
		       *pFifoAddr = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       *pFifoAddr = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       *pFifoAddr = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       *pFifoAddr = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
#else
#ifdef USE_THREE_CODEC
        /* Copy data from the USB FIFO */
		       *pFifoAddr = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       *pFifoAddr = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       *pFifoAddr = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
#else
#ifdef USE_TWO_CODEC
        /* Copy data from the USB FIFO */
		       *pFifoAddr = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
		       *pFifoAddr = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
#else
        /* Copy data from the USB FIFO */
		       *pFifoAddr = codec_input_buffer[codec_input_buffer_output_index];
			   codec_input_buffer_output_index++;
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
				// check for loop around
			    if (codec_input_buffer_output_index >= CODEC_INPUT_BUFFER_SIZE)
			    {
			        codec_input_buffer_output_index = 0;
			    }
    }
#endif //SAMP_24BIT
    
		// reduce the number of samples (mono)
		codec_input_buffer_sample -= (numRecSamps>>1);
			swiSendCount++;
	} // if (codec_input_buffer_sample==0)
	
	if(peps->xferType == CSL_USB_ISO)
	{
		usbRegisters->PERI_CSR0_INDX |=
		                CSL_USB_PERI_TXCSR_CLRDATATOG_MASK;
	}

	/* Commit Tx Packet */
	CSL_FINS(usbRegisters->PERI_CSR0_INDX,
			 USB_PERI_CSR0_INDX_RXPKTRDY, TRUE);
	} // for (i=0; i<pktCount; i++)

	/* restore the index register */
	usbRegisters->INDEX_TESTMODE = saveIndex;
}

/* Reads TXMAXP for selected endpoint */
Uint16 USB_getTxMaxp(Uint16 epNum)
{
    Uint16 saveIndex, txMaxp;

    /* save the index register value */
    saveIndex = usbRegisters->INDEX_TESTMODE;

    CSL_FINS(usbRegisters->INDEX_TESTMODE,
             USB_INDEX_TESTMODE_EPSEL, epNum);

    txMaxp = usbRegisters->TXMAXP_INDX;

    /* restore the index register */
    usbRegisters->INDEX_TESTMODE = saveIndex;

    return txMaxp;
}

/* Writes TXMAXP for selected endpoint */
Uint16 USB_setTxMaxp(Uint16 epNum, Uint16 val)
{
    Uint16 saveIndex;

    /* save the index register value */
    saveIndex = usbRegisters->INDEX_TESTMODE;

    CSL_FINS(usbRegisters->INDEX_TESTMODE,
             USB_INDEX_TESTMODE_EPSEL, epNum);

    usbRegisters->TXMAXP_INDX = val;

    /* restore the index register */
    usbRegisters->INDEX_TESTMODE = saveIndex;

    return 0;
}

/**
 *  \brief  USB ISR
 *
 *  \param  none
 *
 *  \return None
 */
Uint32 sofIntCountPlay = 0;
Uint32 sofIntCountRec = 0;
Uint32 sofIntCountRecSave;
Uint32 usbIntCount = 0;
Uint32 sof_int_count = 0;
Uint16 uFrameCount = 0;
Uint32 sofNoRxCount = 0;
Uint32 sofNoTxCount = 0;
///Uint32 sofMoreRxCount = 0;
///Uint32 sofMoreTxCount = 0;
Int32 dmaSampCntPerSec = 0;
Int32 usbIsoInSampCntPerSec = 0;
///Int16 countLog[300];
///Int16 countLogIdx = 0;
#ifdef FEEDBACKEP
Uint32 isoFbckIntCount = 0;
#endif //FEEDBACKEP
void USBisr()
{
    pUsbContext     pContext;
    Uint16 dmaSampCnt;

    usbIntCount++;
    /* Latch and clear interrupts */
    pContext = &gUsbContext;
    pContext->dwIntSourceL = usbRegisters->INTMASKEDR1;
    pContext->dwIntSourceH = usbRegisters->INTMASKEDR2;
    usbRegisters->INTCLRR1 = pContext->dwIntSourceL;
    usbRegisters->INTCLRR2 = pContext->dwIntSourceH;

    /* Handle SOF processing */
    if (pContext->dwIntSourceH & CSL_USB_GBL_INT_SOF)
    {
		sof_int_count++;
		uFrameCount++;

		// if we are using 44.1khz, we may need to revise the sample count
		gSetRecSampNumDelta  = 0;
		if (gSetRecSampNum==5)
		{
			// add 2 for each 2 uFrames
			if ((sof_int_count&0x00000001)==0)
			{
				gSetRecSampNumDelta += 2;
				// add 2 more for each 80 uFrames
				if (uFrameCount==80)
				{
					gSetRecSampNumDelta += 2;
					uFrameCount = 0;
				}
			}
		}
		// if we are using 88.2khz, we may need to revise the sample count
		if (gSetRecSampNum==11)
		{
			// add 2 more for each 40 uFrames
			if (uFrameCount==40)
			{
				gSetRecSampNumDelta += 2;
				uFrameCount=0;
			}
		}
		// get the number of sample per uFrame. x2 for stereo.
		numPbSamps = numRecSamps = (gSetRecSampNum<<1)+gSetRecSampNumDelta;

		if (usb_play_mode)
		{
			sofIntCountPlay++;		
		}
		if (usb_rec_mode)
		{
			sofIntCountRec++;
			if (firstRecordFlag)
			{
                // start record
		        // Send the data to the endpoint buffer
		        SWI_post(&SWI_Send_USB_Output);
				firstRecordFlag = FALSE;
			}
		}
			
        // accumulate the USB ISO IN nominal sample counter (x2 for stereo) 
		usbIsoInSampCntPerSec += ((gSetRecSampNum<<1)+gSetRecSampNumDelta);

		// for each 2ms (16 microframes), get the DMA transfer count
		if ((sof_int_count&0x000F)==0)
		{
	        latchDmaXferState();
		
        	/* Compute DMA transfer count since last SOF */
        	dmaSampCnt = computeDmaXferCnt();			
	        // accumulate the DMA sample counter 
			dmaSampCntPerSec += (dmaSampCnt<<1);			
		}		

		// for each 1.024 second (8192 microframes)
		if ((sof_int_count&0x1FFF)==0)
		{
			// update the numRecSamps, if necessary.
			numRecSamps -= (Int16)(usbIsoInSampCntPerSec-dmaSampCntPerSec); 
			// reset the accumulators
			usbIsoInSampCntPerSec = 0;
			dmaSampCntPerSec = 0;
		}			
    }

    /* ISO IN, TX endpoint  */
    if(pContext->dwIntSourceL & USB_TX_INT_EP_REC)
    {
        recIntrRcvd++; /* indicate record packet sent to next SOF */
		isoInIntCount++;

		if (isoInIntCount==2)
        {
			sofIntCountRecSave = sofIntCountRec;
        }

        // Send the data to the endpoint buffer
        SWI_post(&SWI_Send_USB_Output);
    }

    /* ISO OUT, RX endpoint */
    if (pContext->dwIntSourceL & USB_RX_INT_EP_PLAY)
    {
        playIntrRcvd++; /* indicate play packet received to next SOF */

		isoOutIntCount++;
        USB_handleRxIntr(pContext, EP_NUM_PLAY);
            // Get the data from the endpoint buffer
            SWI_post(&SWI_Store_USB_Input);
        }

#ifdef FEEDBACKEP
    /* ISO IN, FBCK endpoint */
    if (pContext->dwIntSourceL & USB_TX_INT_EP_FBCK)
    {
		isoFbckIntCount++;
		// put feedback EP processing code here
    }
#endif //FEEDBACKEP

    if(pContext->dwIntSourceH & CSL_USB_GBL_INT_SOF)
    {
        if (usb_play_mode)
        {
            if (playIntrRcvd >= 1) /* check Rx received since last SOF */
            {
				///if (playIntrRcvd > 1)
				///	sofMoreRxCount++;
                playIntrRcvd = 0;
            }
            else
            {
				sofNoRxCount++;
                ///if (sofNoRxCount>9)
                ///	usb_play_mode = FALSE; /* no Rx received since last SOF -- deactivate playback */
                //LOG_printf(&trace, "R 0x%04X", (asrcOutputFifoInBlk<<8)|asrcOutputFifoOutBlk); //debug
            }
        }

        if (usb_rec_mode)
            {
            if (recIntrRcvd >= 1) /* check record packet since last SOF */
				{
				///if (recIntrRcvd>1)
				///	sofMoreTxCount++;
                recIntrRcvd = 0;
				}
            else
            {
				sofNoTxCount++;
                ///if (sofNoTxCount>9)
                ///	usb_rec_mode = FALSE; /* no Tx sent since last SOF -- deactivate record */
                //LOG_printf(&trace, "R 0x%04X", (asrcOutputFifoInBlk<<8)|asrcOutputFifoOutBlk); //debug
            }
        }
    }

	// only send the interrupt message when they are other interrupts (not USB ISO IN/OUT/SOF)
    if ((pContext->dwIntSourceL & (~(USB_RX_INT_EP_PLAY|USB_TX_INT_EP_REC))) ||
		(pContext->dwIntSourceH & (~CSL_USB_GBL_INT_SOF)))
	{
	    USB_MUSB_Isr();
	} else
	{
	    /* all interrupts are handled, signal 'End Of Interrupt' */
	    usbRegisters->EOIR = CSL_USB_EOIR_RESETVAL;
	}
}

/**
 *  \brief  USB Isr
 *
 *  \param  None
 *
 *  \return None
 */
static void USB_MUSB_Isr(void)
{
    CSL_UsbMsgObj        USBMsg;

    /* enqueue message */
    USBMsg.wMsg = CSL_USB_MSG_USB_INT;
    MBX_post(&MBX_musb, &USBMsg, 0);
}

#ifdef APP_USB_SELF_POWERED
void USB_conn_poll(void)
{
    if ((usbDevDisconnect == TRUE) &&
        (CSL_FEXT(usbRegisters->DEVCTL, USB_DEVCTL_VBUS) ==
        CSL_USB_DEVCTL_VBUS_ABOVEVBUSVALID))
    {
        usbDevDisconnect = FALSE;
		///usbIntCount = 0;
        USB_connectDev(CSL_USB0);
    }
}
#endif //APP_USB_SELF_POWERED

/**
 *  \brief  Wraper for USB main task
 *
 *  \param  None
 *
 *  \return None
 */
void USBMainTask()
{
    MainTask();
}

/**
 *  \brief  USB main task
 *
 *  \param  None
 *
 *  \return None
 */
Uint32 mainTaskNum = 0;
Uint32 inTransferNum = 0;
Uint32 inTransferErrNum = 0;
static void MainTask(void)
{
    pUsbContext     pContext;
    pUsbEpStatus     peps;
    CSL_UsbMsgObj   USBMsg;
    volatile WORD     temp;
    Bool            fExitMainTaskOnUSBError;
    pAcClassHandle       pAcClassHdl;
    CSL_AcCtrlObject     *pCtrlHandle;
    CodecCfgMsgObj      codecCfgMsg;

    pAcClassHdl = AC_AppHandle.pAcObj;
    pCtrlHandle = &pAcClassHdl->ctrlHandle;

    fExitMainTaskOnUSBError = FALSE;
    pContext = &gUsbContext;

    //TSK_settime(TSK_self()); // statistic collection
    while(1)
    {
        /* Wait for mailbox */
        if(MBX_pend(&MBX_musb, &USBMsg, SYS_FOREVER))
        {
            if(USBMsg.wMsg == CSL_USB_MSG_MAIN_TASK_EXIT)
            {
                break;
            }
       
       		mainTaskNum++;
       		     
            switch(USBMsg.wMsg)
            {
                case CSL_USB_MSG_USB_INT:
                    if(pContext->fMUSBIsReady)
                    {
                        /* Handle the interrupts */
                        if(!HandleUSBInterrupt(pContext))
                            fExitMainTaskOnUSBError = TRUE;
                    }
                    break;

                case CSL_USB_MSG_CONNECT_DEVICE:

                    USB_connectDev(CSL_USB0);

                    /* ack */
                    SEM_post(&SEM_ConnectDeviceDone);
                    break;

                case CSL_USB_MSG_DISCONNECT_DEVICE:
                    USB_disconnectDev(CSL_USB0);
                    if (pContext->cableState != CSL_USB_DEVICE_DETACH)
                    {
                        if(!StopDevice(pContext))
                            fExitMainTaskOnUSBError = TRUE;
                    }
                    /* ack */
                    SEM_post(&SEM_DisconnectDeviceDone);
                    break;

                case CSL_USB_MSG_RESET_DEVICE:
                    if(!USB_resetDev(CSL_USB0))
                        fExitMainTaskOnUSBError = TRUE;
                    /* ack */
                    SEM_post(&SEM_ResetDeviceDone);
                    break;

                case CSL_USB_MSG_STALL_ENDPOINT:
                    break;

                case CSL_USB_MSG_CLEAR_ENDPOINT_STALL:
                    /* ack */
                    SEM_post(&SEM_ClearEndpointStalltDone);
                    break;

                case CSL_USB_MSG_ABORT_TRANSFER:

                     SEM_post(&SEM_AbortTransferDone);
                    break;

                case CSL_USB_MSG_SEND_HAND_SHAKE:
                    break;

                case CSL_USB_MSG_DATA:
                    fExitMainTaskOnUSBError = FALSE;
                    /* Just trigger this task. */
                    break;

                default:
                    break;
            }
        }

/* Non-Control endpoints */

        /* process the DATA IN */
        if (pContext->fWaitingOnFlagA == TRUE)
        {
            /* Get the Endpoint currently assigned to Flag A */
#if 0		// move to send_USB_Output
            if (pContext->fEP1InBUFAvailable == TRUE)
            {
                pContext->fEP1InBUFAvailable = FALSE;
                peps = &pContext->pEpStatus[EP_NUM_REC];
                if (pContext->fInitialized)
                {
                    if (peps->pTransfer)
                    {
                        inTransferNum++;
                        if (USB_handleTx(pContext, EP_NUM_REC))
						{
                            fExitMainTaskOnUSBError = TRUE;
							inTransferErrNum++;
						}
                    }
                }
            }
            else if (pContext->fEP3InBUFAvailable == TRUE)
#endif
            if (pContext->fEP3InBUFAvailable == TRUE)
            {
                pContext->fEP3InBUFAvailable = FALSE;
                peps = &pContext->pEpStatus[EP_NUM_HID];
                if (pContext->fInitialized)
                {
                    if (peps->pTransfer)
                    {
                        if (!USB_handleTx(pContext, EP_NUM_HID))
                            fExitMainTaskOnUSBError = TRUE;
                    }
                }
            }
        }

#if 0 // moved to store_USB_Input()
        /* process the DATA OUT */
        if(pContext->fWaitingOnFlagB)
        {
            /* Get the Endpoint currently assigned to Flag B */
            peps = &pContext->pEpStatus[CSL_USB_EP2];

            if(USB_isValidDataInFifoOut(peps))
            {
                if(pContext->fInitialized)
                {
                    if(peps->pTransfer)
                    {
                        if(!USB_handleRx(pContext, CSL_USB_EP2))
                        {
                            fExitMainTaskOnUSBError = TRUE;
                        }
                    }
                }
            }
        }/* fWaitingOnFlagB check */
#endif

        /* process EP0 IN DATA */
        if(pContext->fWaitingOnEP0BUFAvail)
        {
            if(pContext->fEP0BUFAvailable)
            {
                if(!(USB_processEP0In(pContext)))
                    fExitMainTaskOnUSBError = TRUE;
            }
        }

        /* fOutPhaseCmd is active when the MUSB receive EP0 data */
        if(pContext->fOutPhaseCmd)
        {
            if(!USB_processEP0Out(pContext))
                fExitMainTaskOnUSBError = TRUE;
        }

        // check for record sample rate change
		// did we get the new sample rate 
        if ((gRecSampRateChange)&&
			(pCtrlHandle->recSampleRateBuf[1] != 0xffff))
        {
            gRecSampRateChange = FALSE;

			gSetRecSampRateFlag = TRUE;
			gSetRecSampRateTemp = ((long)pCtrlHandle->recSampleRateBuf[2]<<16)|(pCtrlHandle->recSampleRateBuf[1]);
	        /* Update the sample rate state */
	        codecCfgMsg.wMsg = CODEC_CFG_MSG_ADJ_RATE;
	        codecCfgMsg.wData = (void *)&gSetRecSampRateTemp;
	        MBX_post(&MBX_codecConfig, &codecCfgMsg, 0);
        }
        
        if(mute_flag_change == TRUE)
        {
            pAcClassHandle      pAcClassHdl;
            CSL_AcCtrlObject    *pCtrlHandle;
            CodecCfgMsgObj      codecCfgMsg;

            pAcClassHdl = AC_AppHandle.pAcObj;
            pCtrlHandle = &pAcClassHdl->ctrlHandle;

            // has the mute value been updated?
            if (pCtrlHandle->muteCtrlBuf[1] != 0xffff )
            {
                mute_flag_change = FALSE;

                // update the mute state
                codecCfgMsg.wMsg = CODEC_CFG_MSG_ADJ_MUTE;
                codecCfgMsg.wData = (void *)&pCtrlHandle->muteCtrlBuf[1];
                MBX_post(&MBX_codecConfig, &codecCfgMsg, 0);
            }
        }

        if (playback_volume_flag_change_Left == TRUE)
        {
            pAcClassHandle      pAcClassHdl;
            CSL_AcCtrlObject    *pCtrlHandle;
            CodecCfgMsgObj      codecCfgMsg;

            pAcClassHdl = AC_AppHandle.pAcObj;
            pCtrlHandle = &pAcClassHdl->ctrlHandle;

            // has the volume value been updated?
            if (pCtrlHandle->leftVolBuf[1] != 0xffff)
            {
                playback_volume_flag_change_Left = FALSE;

                // adjust volume setting
                codecCfgMsg.wMsg = CODEC_CFG_MSG_ADJ_VOL_L;
                codecCfgMsg.wData = (void *)&pCtrlHandle->leftVolBuf[1];
                MBX_post(&MBX_codecConfig, &codecCfgMsg, 0);
            }
        }

        if (playback_volume_flag_change_Right == TRUE)
        {
            pAcClassHandle      pAcClassHdl;
            CSL_AcCtrlObject    *pCtrlHandle;
            CodecCfgMsgObj      codecCfgMsg;

            pAcClassHdl = AC_AppHandle.pAcObj;
            pCtrlHandle = &pAcClassHdl->ctrlHandle;

            // has the volume value been updated?
            if (pCtrlHandle->rightVolBuf[1] != 0xffff)
            {
                playback_volume_flag_change_Right = FALSE;

                // adjust volume setting
                // adjust volume setting
                codecCfgMsg.wMsg = CODEC_CFG_MSG_ADJ_VOL_R;
                codecCfgMsg.wData = (void *)&pCtrlHandle->rightVolBuf[1];
                MBX_post(&MBX_codecConfig, &codecCfgMsg, 0);
            }
        }

        //TSK_deltatime(TSK_self()); // statistic collection
    }

    if(!fExitMainTaskOnUSBError)
    {

        pContext->fMUSBIsReady = FALSE;

        /* must wait 500 ms for the VBus go down,
        then the MUSB can go to suspend */
//        TSK_sleep(500*1);

        /*suspend the MUSB to save power*/

        /* Ack for exit this task */
        SEM_post(&SEM_MUSBMainTaskExited);
    }
    else /* something wrong with USB*/
    {
        ;
    }
}

/**
 *  \brief  USB delay function
 *
 *  \param  dwMicroSeconds delay in micro secs
 *
 *  \return None
 */
static void Delay(
    DWORD dwMicroSeconds
)
{
    volatile DWORD delay;
    volatile DWORD looper;

    while(dwMicroSeconds--)
    {
        /* CLOCK = 120 MHz */
        for (looper = 0; looper < 6; looper++)
        {
            delay++;
        }
    }
}

