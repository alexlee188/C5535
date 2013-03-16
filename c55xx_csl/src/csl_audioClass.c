/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2008
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
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
/** @file csl_audioClass.c
 *
 *  @brief USB Audio Class functional layer API source file
 *
 *  Path: \(CSLPATH)\ src
 */

/* ============================================================================
 * Revision History
 * ================
 * 10-Dec-2008 Created
 * 08-May-2009 Modifed to fix the string desc ASCII to UNICODE conversion issue
 * 28-May-2009 Modifed as per the review comments
 * ============================================================================
 */
#include <app_usb.h>
#include <csl_audioClass.h>
#include <csl_audioClassAux.h>

#include <log.h>
#include <std.h>
#include <mbx.h>
#include <sem.h>
#include <swi.h>
#include <tsk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <psp_i2s.h>
#include "ezdsp5535_led.h"

Bool gPbSampRateChange = FALSE; // playback sample rate change flag
Bool gRecSampRateChange = FALSE; // record sample rate change flag -- NOTE no record sample rate control

Bool mute_flag_change = FALSE; // playback mute state change flag
Bool rec_mute_flag_change = FALSE; // record mute state change flag

// playback volume change flags
Bool playback_volume_flag_change_Left = FALSE;
Bool playback_volume_flag_change_Right = FALSE;
Bool playback_volume_flag_change_Left_Right = FALSE;

///#ifdef SAMPLE_RATE_RX_96kHz
Int16 numRecSamps = 24;
///#endif

///#ifdef SAMPLE_RATE_TX_96kHz
Int16 numPbSamps = 24;
///#endif

Uint16 alt_setting_rec = 0;
Uint16 alt_setting_play = 0;

/** ============================================================================
 *   @n@b AC_Open
 *
 *   @b Description
 *   @n This function Opens the Audio class module.
 *      It initializes Ac and Ctrl handles with the data given
 *      by the application
 *
 *   @b Arguments
 *   @verbatim
            pAppClassHandle      Application handle to the Audio Class Obj
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK              - Returned for success
 *   @li                    CSL_ESYS_BADHANDLE   - Invalid Audio class handle
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Initializes Ac and Ctrl handles
 *
 *   @b Modifies
 *   @n Ac and Ctrl handle data
 *
 *   @b Example
 *   @verbatim
            CSL_AcInitStructApp    acAppHandle;
            pAcAppClassHandle      pAppClassHandle;
            Uint16                 acAppBuffer[420];
            CSL_Status             status;

            pAppClassHandle = &acAppHandle;

            // Assign values to all the members in acAppHandle struct
            acAppHandle.pAcObj = &acAppBuffer[0];
            .....
            .....
            status = AC_Open(pAppClassHandle);
     @endverbatim
 *  ============================================================================
 */
CSL_Status AC_Open(pAcAppClassHandle    pAppClassHandle)
{
    pAcClassHandle    pHandle;
    CSL_Status        status;
    Uint16            eventMask;
    Uint16			  i;

    status  = CSL_SOK;
    pHandle = (pAcClassHandle)(pAppClassHandle->pAcObj);

    if(pAppClassHandle != NULL)
    {
        /* Initing CSL_AcCtrlObject */
        if (pAppClassHandle->numLun > 0)
        {
            pHandle->acHandle.noOfLun = pAppClassHandle->numLun - 1;
        }

        pHandle->ctrlHandle.devNum = CSL_USB0;

        /* Initilialising all default values */
        pHandle->ctrlHandle.suspendFlag   = 0;
        pHandle->ctrlHandle.curConfigStat = 0x0;

        /* hEpObjArray needs to be setup */
        pHandle->ctrlHandle.hEpObjArray[0] = &pHandle->ctrlHandle.ctrlOutEpObj;
        pHandle->ctrlHandle.hEpObjArray[1] = &pHandle->ctrlHandle.ctrlInEpObj;
        pHandle->ctrlHandle.hEpObjArray[2] = &pHandle->acHandle.isoOutEpObj;
        pHandle->ctrlHandle.hEpObjArray[3] = &pHandle->acHandle.isoInEpObj;
        pHandle->ctrlHandle.hEpObjArray[4] = &pHandle->acHandle.hidIntInEpObj;
#ifdef FEEDBACKEP
        pHandle->ctrlHandle.hEpObjArray[5] = &pHandle->acHandle.isoFbckEpObj;
#endif //FEEDBACKEP

        pHandle->ctrlHandle.getMinBuffer[0] = 0x0000;
        pHandle->ctrlHandle.getMinBuffer[1] = 0xFA00; /* vol min = -6 dB */

        pHandle->ctrlHandle.getMaxBuffer[0] = 0x0000;
        pHandle->ctrlHandle.getMaxBuffer[1] = 0x1D00; /* vol max = 29 dB */

        pHandle->ctrlHandle.getResBuffer[0] = 0x0000;
        pHandle->ctrlHandle.getResBuffer[1] = 0x0100; /* vol res = 1 dB */

        pHandle->ctrlHandle.leftVolBuf[0] = 0x0000;
        pHandle->ctrlHandle.leftVolBuf[1] = 0x0000; /* playback left vol = 0 dB */
        pHandle->ctrlHandle.rightVolBuf[0] = 0x0000;
        pHandle->ctrlHandle.rightVolBuf[1] = 0x0000; /* playback right vol = 0 dB */
        pHandle->ctrlHandle.leftRightVolBuf[0] = 0x0000;
        pHandle->ctrlHandle.leftRightVolBuf[1] = 0x0000; /* playback left vol = 0 dB */
        pHandle->ctrlHandle.leftRightVolBuf[2] = 0x0000; /* playback right vol = 0 dB */

        pHandle->ctrlHandle.recordleftVolBuf[0] = 0x0000;
        pHandle->ctrlHandle.recordleftVolBuf[1] = 0x0000; /* playback left vol = 0 dB */
        pHandle->ctrlHandle.recordrightVolBuf[0] = 0x0000;
        pHandle->ctrlHandle.recordrightVolBuf[1] = 0x0000; /* playback right vol = 0 dB */
        pHandle->ctrlHandle.recordleftRightVolBuf[0] = 0x0000;
        pHandle->ctrlHandle.recordleftRightVolBuf[1] = 0x0000; /* playback left vol = 0 dB */
        pHandle->ctrlHandle.recordleftRightVolBuf[2] = 0x0000; /* playback right vol = 0 dB */

        pHandle->ctrlHandle.muteCtrlBuf[0] = 0x0000;
        pHandle->ctrlHandle.muteCtrlBuf[1] = 0x0000; /* playback mute control unmuted */

        pHandle->ctrlHandle.recordmuteCtrlBuf[0] = 0x0000;
        pHandle->ctrlHandle.recordmuteCtrlBuf[1] = 0x0000; /* record mute control unmuted */

///#ifdef SAMPLE_RATE_TX_96kHz
        pHandle->ctrlHandle.sampleRateBuf[0] = 0x0000;
        pHandle->ctrlHandle.sampleRateBuf[1] = 0x7700; /* playback sampling rate = 96 kHz */
        pHandle->ctrlHandle.sampleRateBuf[2] = 0x0001;
///#endif

///#ifdef SAMPLE_RATE_RX_96kHz
        pHandle->ctrlHandle.recSampleRateBuf[0] = 0x0000;
        pHandle->ctrlHandle.recSampleRateBuf[1] = 0x7700; /* record sampling rate = 96 kHz */
        pHandle->ctrlHandle.recSampleRateBuf[2] = 0x0001;
///#endif
        pHandle->ctrlHandle.strDescr    = pAppClassHandle->strDescrApp;
        pHandle->ctrlHandle.acReqTable = &pAppClassHandle->acReqTableApp[0];
        pHandle->ctrlHandle.acReqTableHID = &pAppClassHandle->acReqTableAppHID[0];

        pHandle->acHandle.playBackActive = FALSE;
        pHandle->acHandle.recordActive = FALSE;
        pHandle->acHandle.mediaInitDone  = FALSE;

        for (i = 0; i < 8; i++) pHandle->ctrlHandle.alt_setting_rec[i] = 0x0000;
        for (i = 0; i < 8; i++) pHandle->ctrlHandle.alt_setting_play[i] = 0x0000;

        /* Initing CSL_AcObject */
        pHandle->acHandle.lbaBufferPb = pAppClassHandle->lbaBufferPbApp;
        pHandle->acHandle.lbaBufferRec = pAppClassHandle->lbaBufferRecApp;
        pHandle->acHandle.lbaBufferHidReport = pAppClassHandle->lbaBufferHidReportApp;

        /* Initialising the default values for all the Audio Class Object */
        AC_initAcVal(&pHandle->acHandle);

        pHandle->acHandle.playAudio = pAppClassHandle->playAudioApp;
        pHandle->acHandle.recordAudio = pAppClassHandle->recordAudioApp;
        pHandle->acHandle.initPlayAudio = pAppClassHandle->initPlayAudioApp;
        pHandle->acHandle.initRecordAudio = pAppClassHandle->initRecordAudioApp;
        pHandle->acHandle.stopPlayAudio = pAppClassHandle->stopPlayAudioApp;
        pHandle->acHandle.stopRecordAudio = pAppClassHandle->stopRecordAudioApp;
        pHandle->acHandle.mediaGetPresentState = pAppClassHandle->mediaGetPresentStateApp;
        pHandle->acHandle.mediaInit = pAppClassHandle->mediaInitApp;
        pHandle->acHandle.mediaEject = pAppClassHandle->mediaEjectApp;
        pHandle->acHandle.mediaLockUnit = pAppClassHandle->mediaLockUnitApp;
        pHandle->acHandle.getMediaSize = pAppClassHandle->getMediaSizeApp;
        pHandle->acHandle.getHidReport = pAppClassHandle->getHidReportApp;

        /* Copying  user's VID and PID into the device descriptor */
        pHandle->ctrlHandle.deviceDescr[5] = pAppClassHandle->vId;
        pHandle->ctrlHandle.deviceDescr[6] = pAppClassHandle->pId;

        /* Initialize the Control OUT Endpoint 0 */
        eventMask = (CSL_USB_EVENT_RESET | CSL_USB_EVENT_SETUP |
                     CSL_USB_EVENT_SUSPEND | CSL_USB_EVENT_RESUME |
                     CSL_USB_EVENT_RESET | CSL_USB_EVENT_EOT);
        USB_initEndptObj(pHandle->ctrlHandle.devNum,
                         &pHandle->ctrlHandle.ctrlOutEpObj,
                         CSL_USB_OUT_EP0,
                         CSL_USB_CTRL,
                         CSL_USB_EP0_PACKET_SIZE,
                         eventMask,
                         pAppClassHandle->ctrlHandler);

        /* Initialize the Control IN Endpoint 0 */
        USB_initEndptObj(pHandle->ctrlHandle.devNum,
                         &pHandle->ctrlHandle.ctrlInEpObj,
                         CSL_USB_IN_EP0,
                         CSL_USB_CTRL,
                         CSL_USB_EP0_PACKET_SIZE,
                         CSL_USB_EVENT_EOT,
                         pAppClassHandle->ctrlHandler);

        /* Initialize the Iso OUT Endpoint */
        USB_initEndptObj(pHandle->ctrlHandle.devNum,
                         &pHandle->acHandle.isoOutEpObj,
                         (CSL_UsbEpNum)pAppClassHandle->rxEpNum, 
                         CSL_USB_ISO,
                         pAppClassHandle->rxPktSize,
                         CSL_USB_EVENT_EOT,
                         pAppClassHandle->isoHandler);

#ifdef FEEDBACKEP
        /* Initialize the Iso IN feedback Endpoint */
        USB_initEndptObj(pHandle->ctrlHandle.devNum,
                         &pHandle->acHandle.isoFbckEpObj,
                         (CSL_UsbEpNum)(pAppClassHandle->fbEpNum + CSL_USB_IN_EP0),
                         CSL_USB_ISO,
                         pAppClassHandle->fbckTxPktSize,
                         CSL_USB_EVENT_EOT,
                         pAppClassHandle->isoHandler);
#endif //FEEDBACKEP

        /* Initialize the Iso IN Endpoint */
        USB_initEndptObj(pHandle->ctrlHandle.devNum,
                         &pHandle->acHandle.isoInEpObj,
                         (CSL_UsbEpNum)(pAppClassHandle->txEpNum + CSL_USB_IN_EP0), 
                         CSL_USB_ISO,
                         pAppClassHandle->txPktSize,
                         //(CSL_USB_EVENT_SOF |CSL_USB_EVENT_EOT),
                         CSL_USB_EVENT_EOT,
                         pAppClassHandle->isoHandler);

        /* Initialize the HID Interrupt IN Endpoint */
        USB_initEndptObj(pHandle->ctrlHandle.devNum,
                         &pHandle->acHandle.hidIntInEpObj,
                         (CSL_UsbEpNum)(pAppClassHandle->hidTxEpNum + CSL_USB_IN_EP0), 
                         CSL_USB_INTR,
                         pAppClassHandle->hidTxPktSize,
                         CSL_USB_EVENT_EOT,
                         pAppClassHandle->hidHandler);

        pHandle->ctrlHandle.acHidIfNum = pAppClassHandle->acHidIfNum; // HID interface number
        pHandle->ctrlHandle.acHidReportId = pAppClassHandle->acHidReportId; // HID report ID
        pHandle->ctrlHandle.acHidReportLen = pAppClassHandle->acHidReportLen; // HID report length (bytes)
        pHandle->ctrlHandle.acHidIdleRate = 0; // HID idle rate -- only indefinite duration supported

        /* USB API setParams called for setup */
        USB_setParams(CSL_USB0, pHandle->ctrlHandle.hEpObjArray, 0x0);

        USB_connectDev(pHandle->ctrlHandle.devNum);
    }
    else
    {
        status = CSL_ESYS_BADHANDLE;
    }

    return(status);
}

/** ============================================================================
 *   @n@b AC_Close
 *
 *   @b Description
 *   @n This function Closes the audio class module.
 *
 *   @b Arguments
 *   @verbatim
            pAppClassHandle       Application handle to the Audio Class Obj
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK              - Returned for success
 *   @li                    CSL_ESYS_BADHANDLE   - Invalid Audio class handle
 *
 *   <b> Pre Condition </b>
 *   @n  AC_Open function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Closes the Audio Class module
 *
 *   @b Modifies
 *   @n Ac and Ctl handles
 *
 *   @b Example
 *   @verbatim
            CSL_AcInitStructApp    acAppHandle;
            pAcAppClassHandle      pAppClassHandle;
            Uint16                 acAppBuffer[420];
            CSL_Status             status;

            pAppClassHandle = &acAppHandle;

            // Assign values to all the members in acAppHandle struct
            acAppHandle.pAcObj = &acAppBuffer[0];
            .....
            .....
            status = AC_Open(pAppClassHandle);
            ....
            ....
            status = AC_Close(pAppClassHandle);
     @endverbatim
 *  ============================================================================
 */
CSL_Status AC_Close(pAcAppClassHandle    pAppClassHandle)
{
    pAcClassHandle       pAcClassHdl;
    CSL_AcCtrlObject     *pCtrlHandle;
    CSL_Status           status;

    pAcClassHdl = (pAcClassHandle)(pAppClassHandle->pAcObj);
    pCtrlHandle  = &pAcClassHdl->ctrlHandle;
    status       = CSL_SOK;

    if(pAppClassHandle != NULL)
    {
        status = USB_disconnectDev(pCtrlHandle->devNum);
        pAppClassHandle->pAcObj = NULL;
    }
    else
    {
        status = CSL_ESYS_BADHANDLE;
    }

    return(status);
}

/** ============================================================================
 *   @n@b AC_Ctrl
 *
 *   @b Description
 *   @n This function handles all USB traffic on the Control Endpoint.
 *      Detects the events on the Control Endpoint and takes
 *      the appropriate action
 *
 *   @b Arguments
 *   @verbatim
            pAcObj        Handle to the Audio Class Obj
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li       CSL_SOK                          - Returned for success
 *   @li       CSL_ESYS_BADHANDLE               - Invalid Audio class handle
 *   @li       CSL_AC_INVALID_REQUEST_HANDLER   - Invalid request class handler
 *
 *   <b> Pre Condition </b>
 *   @n  AC_Open function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Control end point events are serviced
 *
 *   @b Modifies
 *   @n USB Registers.
 *
 *   @b Example
 *   @verbatim
            CSL_AcInitStructApp    acAppHandle;
            pAcAppClassHandle      pAppClassHandle;
            Uint16                 acAppBuffer[420];
            CSL_Status             status;

            pAppClassHandle = &acAppHandle;

            // Assign values to all the members in acAppHandle struct
            acAppHandle.pAcObj = &acAppBuffer[0];
            .....
            .....
            status = AC_Open(pAppClassHandle);
            ....
            ....
            status = AC_Ctrl(pAppClassHandle->pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_Status AC_Ctrl(void    *pAcObj)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcCtrlObject    *pCtrlHandle;
    CSL_AcRequestRet    reqHandlerRet;
    CSL_UsbEventMask    usbCtrlEvents;
    pUsbEpHandle        hCtrlEpIn;
    pUsbEpHandle        hCtrlEpOut;
    CSL_Status          status;
    Uint16              request;

    if(pAcObj == NULL)
    {
        return(CSL_ESYS_BADHANDLE);
    }

    pAcClassHdl   = (pAcClassHandle)(pAcObj);
    pCtrlHandle   = &pAcClassHdl->ctrlHandle;
    hCtrlEpIn     = &pCtrlHandle->ctrlInEpObj;
    hCtrlEpOut    = &pCtrlHandle->ctrlOutEpObj;
    reqHandlerRet = CSL_AC_REQUEST_DONE;

    pCtrlHandle->fpRequestHandler = AC_reqUnknown;
    /* Finding out which Endpoint caused this function to be called by responding */
    usbCtrlEvents = (USB_getEvents(hCtrlEpOut, &status) |
                     USB_getEvents(hCtrlEpIn, &status));

    /* Check for the RESET event, abort Endpoint Activities and Reconfigure
    the USB Module */
    if(usbCtrlEvents & CSL_USB_EVENT_RESET)
    {
        pCtrlHandle->suspendFlag = FALSE;
        /* Stop all Data transfer Activities */
        status |= USB_abortAllTransaction(pCtrlHandle->devNum);
        /* Reinitialize the Device */
        status |= USB_setParams(pCtrlHandle->devNum,
                                pCtrlHandle->hEpObjArray, FALSE);
        /* Reset device Config No */
        pCtrlHandle->curConfigStat = FALSE;
    }

    if(usbCtrlEvents & CSL_USB_EVENT_SUSPEND)
    {
        pCtrlHandle->suspendFlag = TRUE;
    }

    if(usbCtrlEvents & CSL_USB_EVENT_RESUME)
    {
        pCtrlHandle->suspendFlag = FALSE;
    }

     /* If Event Received is Setup Packet,read the Packet and Lookup the
     Request Table for the Appropriate Request Handler */
     if((usbCtrlEvents & CSL_USB_EVENT_SETUP) == CSL_USB_EVENT_SETUP)
     {

        status |= USB_getSetupPacket(pCtrlHandle->devNum,
                                    &pCtrlHandle->usbSetup, FALSE);
        request = ((((pCtrlHandle->usbSetup.bmRequestType ) <<
                   CSL_AC_8BIT_SHIFT) & 0xF000) |
                   pCtrlHandle->usbSetup.bRequest);

        if (pCtrlHandle->usbSetup.wIndex==IF_NUM_HID)
		{
	        pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
	                                                    pCtrlHandle->acReqTableHID);
		} else
		{
	        pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
	                                                    pCtrlHandle->acReqTable);
		}

        if(reqHandlerRet == CSL_AC_REQUEST_DONE)
        {
            reqHandlerRet = (pCtrlHandle->fpRequestHandler)(pCtrlHandle->devNum,
                             &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAcObj);
        }

        switch(reqHandlerRet)
        {
            /* The Request Handler Routine successfully completed the task,
            so 0 byte ACK is sent */
            case CSL_AC_REQUEST_SEND_ACK:
                status |= USB_postTransaction(hCtrlEpIn, 0, NULL,
                                              CSL_USB_IOFLAG_NONE);
                pCtrlHandle->fpRequestHandler = AC_reqUnknown;
                break;

            /* For Future Use */
            case CSL_AC_REQUEST_DATA_OUT:
                //printf("datout\n");
                break;

            /* The Request Handler Routine successfully completed the task,
            so 0 byte ACK is received */
            case CSL_AC_REQUEST_GET_ACK:
                status |= USB_postTransaction(hCtrlEpOut, 0, NULL,
                                              CSL_USB_IOFLAG_NONE);
                pCtrlHandle->fpRequestHandler = AC_reqUnknown;
                break;

            /* For Future Use */
            case CSL_AC_REQUEST_DATA_IN:
                //printf("datin\n");
                break;

            /* Request Handler is Confused about the Setup Packet, so Stall
            the Control Endpoints */
            case CSL_AC_REQUEST_STALL:
                status |= USB_stallEndpt(hCtrlEpOut);
                status |= USB_stallEndpt(hCtrlEpIn);

                // RDN: Try This
                //pCtrlHandle->fpRequestHandler = AC_reqUnknown;
                break;

            case CSL_AC_REQUEST_DONE:
                pCtrlHandle->fpRequestHandler = AC_reqUnknown;
                break;

            default:
                status = CSL_AC_INVALID_REQUEST_HANDLER;
                //printf("invalreq\n");
                break;
        }

        /* Clear the flags if New Setup Packet is Received */
        if(pCtrlHandle->usbSetup.newPacket)
        {
            pCtrlHandle->usbSetup.newPacket = FALSE;
        }
    }

    return(status);
}

/** ============================================================================
 *   @n@b AC_Iso
 *
 *   @b Description
 *   @n This function handles all UBS traffic on the Isochronous Endpoint
 *      This maintains the state of the Audio class application.
 *      Provides Media Access Data to the low level USB module for transfer.
 *
 *   @b Arguments
 *   @verbatim
            pAcObj        Handle to the Audio Class Obj
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li         CSL_SOK              - Returned for success
 *   @li         CSL_ESYS_BADHANDLE   - Invalid Audio class handle
 *   @li         CSL_ESYS_INVPARAMS   - Returned for invalid parameters
 *
 *   <b> Pre Condition </b>
 *   @n  AC_Open function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Transfers data on iso end points
 *
 *   @b Modifies
 *   @n USB data buffeters
 *
 *   @b Example
 *   @verbatim
            CSL_AcInitStructApp    acAppHandle;
            pAcAppClassHandle      pAppClassHandle;
            Uint16                 acAppBuffer[420];
            CSL_Status             status;

            pAppClassHandle = &acAppHandle;

            // Assign values to all the members in acAppHandle struct
            acAppHandle.pAcObj = &acAppBuffer[0];
            .....
            .....
            status = AC_Open(pAppClassHandle);
            ....
            ....
            status = AC_Iso(pAppClassHandle->pAcObj);
     @endverbatim
 *  ============================================================================
 */
Uint32 inPacketNum = 0;
CSL_Status AC_Iso(void    *pAcObj)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcObject        *pAcHandle;
    pUsbEpHandle        hUsbOutEp;
    pUsbEpHandle        hUsbInEp;
    pUsbEpHandle		hUsbFbckEp;
    CSL_Status          status;
    CSL_UsbEventMask    usbEvent;

    status = CSL_SOK;

    if(pAcObj != NULL)
    {
        pAcClassHdl = (pAcClassHandle)(pAcObj);
        pAcHandle   = &pAcClassHdl->acHandle;
        hUsbOutEp   = &pAcHandle->isoOutEpObj;
        hUsbInEp   = &pAcHandle->isoInEpObj;
        hUsbFbckEp = &pAcHandle->isoFbckEpObj;

        usbEvent = (USB_getEvents(hUsbOutEp, &status) |
                     USB_getEvents(hUsbInEp, &status) |
                     USB_getEvents(hUsbFbckEp, &status));

        if((usbEvent & CSL_USB_EVENT_READ_MEDIA) == CSL_USB_EVENT_READ_MEDIA)
        {
			// move to SWI SWI_Send_USB_Output
            /* record moved to SWI so set flags and return*/
            pAcHandle->recordActive = TRUE;
            status = CSL_AC_MEDIACCESS_SUCCESS;
        }

        if((usbEvent & CSL_USB_EVENT_WRITE_MEDIA) == CSL_USB_EVENT_WRITE_MEDIA)
        {
            /* playpack moved to SWI so set flags and return*/
            pAcHandle->playBackActive = TRUE;
            status = CSL_AC_MEDIACCESS_SUCCESS;
        }
    }
    else
    {
        status = CSL_ESYS_BADHANDLE;
    }

    return(status);
}

/** ============================================================================
 *   @n@b AC_Hid
 *
 *   @b Description
 *   @n This function handles all USB traffic on the HID Interrupt Endpoint.
 *      Detects the events on the HID Endpoint and takes the appropriate action
 *
 *   @b Arguments
 *   @verbatim
            pAcObj        Handle to the Audio Class Obj
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li       CSL_SOK                          - Returned for success
 *   @li       CSL_ESYS_BADHANDLE               - Invalid Audio class handle
 *   @li       CSL_AC_INVALID_REQUEST_HANDLER   - Invalid request class handler
 *
 *   <b> Pre Condition </b>
 *   @n  AC_Open function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  HID interrupt end point events are serviced
 *
 *   @b Modifies
 *   @n USB Registers.
 *
 *   @b Example
 *   @verbatim
CSL_AcInitStructApp    acAppHandle;
pAcAppClassHandle      pAppClassHandle;
Uint16                 acAppBuffer[420];
CSL_Status             status;

pAppClassHandle = &acAppHandle;

// Assign values to all the members in acAppHandle struct
acAppHandle.pAcObj = &acAppBuffer[0];
.....
.....
status = AC_Open(pAppClassHandle);
....
....
            status = AC_Hid(pAppClassHandle->pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_Status AC_Hid(void    *pAcObj)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcCtrlObject    *pCtrlHandle;
    CSL_AcObject        *pAcHandle;
    pUsbEpHandle        hUsbInEp;
    CSL_Status          status;
    CSL_UsbEventMask    usbEvent;

    status = CSL_SOK;

    if (pAcObj != NULL)
    {
        pAcClassHdl = (pAcClassHandle)(pAcObj);
        pCtrlHandle = &pAcClassHdl->ctrlHandle;
        pAcHandle   = &pAcClassHdl->acHandle;
        hUsbInEp    = &pAcHandle->hidIntInEpObj;

        usbEvent = USB_getEvents(hUsbInEp, &status);

        if ((usbEvent & CSL_USB_EVENT_HID_REPORT_TX) == CSL_USB_EVENT_HID_REPORT_TX)
        {
            status = pAcHandle->getHidReport(pCtrlHandle->acHidReportLen, 
                &pAcHandle->lbaBufferHidReport[0]);

            /* Post transaction to USB */
            USB_postTransaction(&pAcHandle->hidIntInEpObj, pCtrlHandle->acHidReportLen, 
                (void *)(&pAcHandle->lbaBufferHidReport[0]), CSL_USB_IOFLAG_NONE);
        }
    }
    else
    {
        status = CSL_ESYS_BADHANDLE;
    }

    return CSL_SOK;
}

/** ============================================================================
 *   @n@b AC_GetactivityFlagStatus
 *
 *   @b Description
 *   @n This function gives the status of the activity Flag
 *
 *   @b Arguments
 *   @verbatim
            pAcObj        Handle to the Audio Class Obj
     @endverbatim
 *
 *   <b> Return Value </b>  - Status of the activity flag
 *   @li                     '1' - If the activity is present
 *   @li                     '0' - If the activity is not present
 *
 *   <b> Pre Condition </b>
 *   @n  AC_Open function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Returns Activity flag status
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            Uint16                 activityFlagStat;
            CSL_AcInitStructApp    acAppHandle;
            pAcAppClassHandle      pAppClassHandle;
            Uint16                 acAppBuffer[420];
            CSL_Status             status;

            pAppClassHandle = &acAppHandle;

            // Assign values to all the members in acAppHandle struct
            acAppHandle.pAcObj = &acAppBuffer[0];
            .....
            .....
            status = AC_Open(pAppClassHandle);
            ....
            ....
            activityFlagStat = AC_GetactivityFlagStatus(pAppClassHandle->pAcObj);
     @endverbatim
 *  ============================================================================
 */
Uint16 AC_GetactivityFlagStatus(void    *pAcObj)
{
   pAcClassHandle    pAcClassHdl;
   CSL_AcObject      *pAcHandle;

   pAcClassHdl = (pAcClassHandle)(pAcObj);
   pAcHandle   = &pAcClassHdl->acHandle;

   return(pAcHandle->activityPresentFlag);
}

/** ============================================================================
 *   @n@b AC_GetsuspendFlagStatus
 *
 *   @b Description
 *   @n This function gives the status of the suspend Flag
 *
 *   @b Arguments
 *   @verbatim
            pAcObj        Handle to the Audio Class Obj
     @endverbatim
 *
 *   <b> Return Value </b>  - Status of the suspend flag
 *   @li                     '1' - If the device is suspended
 *   @li                     '0' - If the device is not suspended
 *
 *   <b> Pre Condition </b>
 *   @n  AC_Open function should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Returns suspend flag status
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
            Uint16                 suspendFlagStat;
            CSL_AcInitStructApp    acAppHandle;
            pAcAppClassHandle      pAppClassHandle;
            Uint16                 acAppBuffer[420];
            CSL_Status             status;

            pAppClassHandle = &acAppHandle;

            // Assign values to all the members in acAppHandle struct
            acAppHandle.pAcObj = &acAppBuffer[0];
            .....
            .....
            status = AC_Open(pAppClassHandle);
            ....
            ....
            suspendFlagStat = AC_GetsuspendFlagStatus(pAppClassHandle->pAcObj);
     @endverbatim
 *  ============================================================================
 */
Uint16 AC_GetsuspendFlagStatus(void    *pAcObj)
{
   pAcClassHandle       pAcClassHdl;
   CSL_AcCtrlObject     *pCtrlHandle;

   pAcClassHdl = (pAcClassHandle)(pAcObj);
   pCtrlHandle = &pAcClassHdl->ctrlHandle;

   return(pCtrlHandle->suspendFlag);
}

/** ============================================================================
 *   @n@b AC_initDescriptors
 *
 *   @b Description
 *   @n This function initializes the descriptors used by the USB audio class
 *
 *   @b Arguments
 *   @verbatim
            pAcObj      Audio Class object
            descBuf     Descriptor buffer pointer
            descrId     Id of the Descriptor to initialize
            descrLen    Length of the descriptor to initialize (in bytes)
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK            - Returned for success
 *   @li                    CSL_ESYS_BADHANDLE - Invalid Ctrl handle
 *   @li                    CSL_ESYS_INVPARAMS - Invalid parameter
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Initializes descriptors
 *
 *   @b Modifies
 *   @n Descriptor buffers
 *
 *   @b Example
 *   @verbatim
 *          CSL_AcInitStructApp    acAppHandle;
            pAcAppClassHandle      pAppClassHandle;
            Uint16                 acAppBuffer[420];
            CSL_Status             status;

            pAppClassHandle = &acAppHandle;

            // Assign values to all the members in acAppHandle struct
            acAppHandle.pAcObj = &acAppBuffer[0];

            status = AC_initDescriptors(pAppClassHandle->pAcObj,
                                        CSL_AC_DEVICE_DESCR, 18);
     @endverbatim
 *  ============================================================================
 */
CSL_Status AC_initDescriptors(void             *pAcObj,
                              Uint16           *descBuf,
                              CSL_AcDescrId    descrId,
                              Uint16           descrLen)
{
    pAcClassHandle       pAcClassHdl;
    CSL_AcCtrlObject     *pCtrlHandle;
    CSL_Status           status;
    Uint16               length;

    status = CSL_ESYS_INVPARAMS;

    if(pAcObj != NULL)
    {
        pAcClassHdl = (pAcClassHandle)(pAcObj);
        pCtrlHandle  = &pAcClassHdl->ctrlHandle;

        switch(descrId)
        {
            /* Initialize the device descriptor */
            case CSL_AC_DEVICE_DESCR:
                if(descrLen <= CSL_AC_DEVICE_DESCR_MAX_LEN)
                {
                    pCtrlHandle->deviceDescr[0] = 0x0000;
                    for(length = 0; length < (descrLen+1)/2; length++)
                    {
                        //pCtrlHandle->deviceDescr[length+1] = descBuf[length];
                        pCtrlHandle->deviceDescr[length+1] = descBuf[(length*2)] | (descBuf[(length*2) + 1] << 8);
                    }
                    status = CSL_SOK;
                }
                break;

            /* Initialize the device qualifier descriptor */
            case CSL_AC_DEVICE_QUAL_DESCR:
                if(descrLen <= CSL_AC_DEV_QUAL_DESCR_MAX_LEN)
                {
                    pCtrlHandle->deviceQualifierDescr[0] = 0x0000;
                    for(length = 0; length < (descrLen+1)/2; length++)
                    {
                        pCtrlHandle->deviceQualifierDescr[length+1] =  descBuf[(length*2)] | (descBuf[(length*2) + 1] << 8);;
                        //pCtrlHandle->deviceQualifierDescr[length+1] = descBuf[length];
                    }
                    status = CSL_SOK;
                }
                break;


            /* Initialize the configuration descriptor */
            case CSL_AC_CONFIG_DESCR:
                if(descrLen <= CSL_AC_CONFIG_DESCR_MAX_LEN)
                {
                    pCtrlHandle->confDescr[0] = 0x0000;
                    for(length = 0; length < (descrLen+1)/2; length++)
                    {
                        //pCtrlHandle->confDescr[length+1] = descBuf[length];
                        pCtrlHandle->confDescr[length+1] = descBuf[(length*2)] | (descBuf[(length*2) + 1] << 8);
                    }
                    status = CSL_SOK;
                }
                break;
            /* Initialize the interface descriptor */
            case CSL_AC_INTERFACE_DESCR:
                if(descrLen <= CSL_AC_INTER_DESCR_MAX_LEN)
                {
                    pCtrlHandle->audioClassIntDescr[0] = 0x0000;
                    for(length = 0; length < (descrLen+1)/2; length++)
                    {
                        pCtrlHandle->audioClassIntDescr[length+1] =  descBuf[(length*2)] | (descBuf[(length*2) + 1] << 8);;
                    }
                    status = CSL_SOK;
                }
                break;
            /* Initialize the string descriptor language Id */
            case CSL_AC_STRING_LANGID_DESC:
                if(descrLen < CSL_AC_STRING_DESCR_LANGID_MAX_LEN)
                {
                    pCtrlHandle->stringDescrLangid[0] = 0x0000;
                    for(length = 0; length < (descrLen+1)/2; length++)
                    {
                        pCtrlHandle->stringDescrLangid[length+1] =  descBuf[(length*2)] | (descBuf[(length*2) + 1] << 8);;
                    }
                    status = CSL_SOK;
                }
                break;

            /* Initialize the AC HID report descriptor */
            case CSL_AC_HID_REPORT_DESC:
                if (descrLen < CSL_AC_HID_REPORT_DESCR_MAX_LEN)
                {
                    pCtrlHandle->audioClassHidReportDescrLen = descrLen; /* store descriptor length since contained at variable offset in configuration descriptor */
                    pCtrlHandle->audioClassHidReportDescr[0] = 0x0000;
                    for(length = 0; length < (descrLen+1)/2; length++)
                    {
                        pCtrlHandle->audioClassHidReportDescr[length+1] = descBuf[(length*2)] | (descBuf[(length*2) + 1] << 8);
                    }
                    status = CSL_SOK;
                }
                break;

            default :
                break;
        }
    }
    else
    {
        return(CSL_ESYS_BADHANDLE);
    }

    return(status);

}

/** ============================================================================
 *   @n@b AC_initAcVal
 *
 *   @b Description
 *   @n This function initializes the Audio Class object
 *
 *   @b Arguments
 *   @verbatim
            pAcHandle        Handle to the Audio class Object
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK              - Returned for success
 *   @li                    CSL_ESYS_BADHANDLE   - Invalid Audio class handle
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Initializes Audio Class object
 *
 *   @b Modifies
 *   @n Audio Class object structure
 *
 *   @b Example
 *   @verbatim
            CSL_AcObject    acObj;
            CSL_Status      status;

            status = AC_initAcVal(&acObj);

     @endverbatim
 *  ============================================================================
 */
static CSL_Status AC_initAcVal(CSL_AcObject    *pAcHandle)
{
    Uint16       copyCnt;
    CSL_Status   status;

    status = CSL_SOK;

    if(pAcHandle != NULL)
    {
        pAcHandle->mediaState = 0;
        pAcHandle->activityPresentFlag =0;

        /* Sense Data */
        /* size */
        pAcHandle->senseData[0] = CSL_AC_SENSE_DATA0_DEFVAL;
        /* Dummy for USB_postTransaction */
        pAcHandle->senseData[1] = CSL_AC_SENSE_DATA1_DEFVAL;
        /* Error code = 0x70 (0-1) */
        pAcHandle->senseData[2] = CSL_AC_SENSE_DATA2_DEFVAL;
        /* Sense key (2-3) */
        pAcHandle->senseData[3] = CSL_AC_SENSE_DATA3_DEFVAL;
        /* bytes 4-5 */
        pAcHandle->senseData[4] = CSL_AC_SENSE_DATA4_DEFVAL;
        /* Additional length = 18 (6-7) */
        pAcHandle->senseData[5] = CSL_AC_SENSE_DATA5_DEFVAL;

        pAcHandle->readCapacity[0] = CSL_AC_READ_CAPACITY0_DEFVAL;

        for(copyCnt = 6; copyCnt < 15; copyCnt++)
        {
            pAcHandle->senseData[copyCnt]  = 0x0000;
        }

        /* CSW */
        /* Dummy for USB_postTransaction */
        pAcHandle->csw[0] = CSL_AC_CSW0_DEFVAL;
        pAcHandle->csw[1] = CSL_AC_CSW1_DEFVAL;
        pAcHandle->csw[2] = CSL_AC_CSW2_DEFVAL;
        for (copyCnt = 3; copyCnt < 8; copyCnt++)
        {
            pAcHandle->csw[copyCnt] = 0;
        }
    }
    else
    {
        status = CSL_ESYS_BADHANDLE;
    }

    return(status);
}

/** ============================================================================
 *   @n@b  AC_reqGetDescriptor
 *
 *   @b Description
 *   @n This function handles the request from host to get the descriptor
 *
 *   @b Arguments
 *   @verbatim
            CSL_AC_REQUEST_ARGS    - AC request arguments
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li        CSL_AC_REQUEST_GET_ACK  - Get acknowledgement for request\
 *   @li        CSL_AC_REQUEST_STALL    - Request stall signal (Error)
 *
 *   <b> Pre Condition </b>
 *   @n  AC_lookupReqHandler should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Sends the descriptor
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
             CSL_AcInitStructApp    acAppHandle;
             CSL_AcObject           *pAcHandle;
             CSL_AcCtrlObject       *pCtrlHandle;
             pUsbEpHandle           hUbOutEp;
             Uint16                 lunNum;
             Uint16                 acAppBuffer[420];
             CSL_Status             status;
             Uint16                 request;

             // Assign values to all the members in acAppHandle struct
             acAppHandle.pAcObj = &acAppBuffer[0];
             .....
             .....
             status = AC_Open(&acAppHandle);
             ....
             ....
             pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
                                                       pCtrlHandle->acReqTable);

             pCtrlHandle->fpRequestHandler(pCtrlHandle->devNum,
                                           &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAC);
     @endverbatim
 *  ============================================================================
 */
CSL_AcRequestRet AC_reqGetDescriptor(CSL_UsbDevNum         devNum,
                                     CSL_UsbSetupStruct    *usbSetup,
                                     pUsbEpHandle          hInEp,
                                     pUsbEpHandle          hOutEp,
                                     void                  *pAcObj)
{
    Uint16    tempLen;
    Uint16    inOutFlag;
    Uint16    descrLen;
    pAcClassHandle       pACClassHdl;
    CSL_AcCtrlObject*    pCtrlHandle;

    pACClassHdl = (pAcClassHandle)(pAcObj);
    pCtrlHandle  = &pACClassHdl->ctrlHandle;

    switch(usbSetup->wValue >> CSL_AC_8BIT_SHIFT)
    {
        case CSL_USB_DEVICE_DESCRIPTOR_TYPE:

            tempLen = pCtrlHandle->deviceDescr[1]&0xFF;
            /* select the smaller of two  */
            tempLen =
                (tempLen < usbSetup->wLength) ? tempLen : usbSetup->wLength;
            USB_postTransaction(hInEp, tempLen,
                                (void*)&pCtrlHandle->deviceDescr[0],
                                CSL_USB_IOFLAG_NONE);
            break;

        case CSL_USB_CONFIGURATION_DESCRIPTOR_TYPE:
             inOutFlag = (CSL_USB_IOFLAG_LNK | CSL_USB_IOFLAG_CAT);
             tempLen = pCtrlHandle->confDescr[2];
             if(tempLen == usbSetup->wLength)
             {
                inOutFlag |= CSL_USB_IOFLAG_NOSHORT;
            }

              /* select the smaller of two */
             tempLen =
                 (tempLen < usbSetup->wLength) ? tempLen : usbSetup->wLength;

            USB_postTransaction(hInEp, tempLen, &pCtrlHandle->confDescr[0],
                                 inOutFlag);
             break;

        case CSL_USB_STRING_DESCRIPTOR_TYPE:
			if ((usbSetup->wValue&0x00FF)>=34)
	            return(CSL_AC_REQUEST_STALL);
				
                 /* strlen of the string requested */
			descrLen = pCtrlHandle->strDescr[usbSetup->wValue&0x00FF][0]&0x00FF;
                   /* select the smallest of two */
			tempLen = (descrLen < usbSetup->wLength) ? descrLen : usbSetup->wLength;

			USB_postTransaction(hInEp, tempLen , &(pCtrlHandle->strDescr[usbSetup->wValue&0x00FF][0]), CSL_USB_IOFLAG_NONE);
            break;

        case CSL_USB_DEVICE_QUAL_DESCRIPTOR_TYPE:
            tempLen = pCtrlHandle->deviceQualifierDescr[1] & 0xFF;
            /* select the smaller of two  */
            tempLen =
                (tempLen < usbSetup->wLength) ? tempLen : usbSetup->wLength;
            USB_postTransaction(hInEp, tempLen, (void*)&pCtrlHandle->deviceQualifierDescr,
                                CSL_USB_IOFLAG_NONE);
            break;

        case CSL_USB_HID_REPORT_DESCRIPTOR_TYPE:
            tempLen = pCtrlHandle->audioClassHidReportDescrLen;
            /* select the smaller of two  */
            tempLen =
                (tempLen < usbSetup->wLength) ? tempLen : usbSetup->wLength;
            USB_postTransaction(hInEp, tempLen, (void*)&pCtrlHandle->audioClassHidReportDescr,
                                CSL_USB_IOFLAG_NONE);
            break;

        default:
            return(CSL_AC_REQUEST_STALL);
    }

    return(CSL_AC_REQUEST_GET_ACK);
}

/** ============================================================================
 *   @n@b  AC_reqSetCurrent
 *
 *   @b Description
 *   @n This function handles the request from host to set current
 *
 *   @b Arguments
 *   @verbatim
            devNum       - USB device number
            usbSetup     - USb setup structure
            hInEp        - USB ISO In end point handle
            hOutEp       - USB ISO Out end point object
            pAcObj       - USB Audio Class object
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li        CSL_AC_REQUEST_SEND_ACK - Send acknowledgement for request
 *   @li        CSL_AC_REQUEST_STALL    - Request stall signal (Error)
 *
 *   <b> Pre Condition </b>
 *   @n  AC_lookupReqHandler should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Sends acknowledgement
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
             CSL_AcInitStructApp    acAppHandle;
             CSL_AcObject           *pAcHandle;
             CSL_AcCtrlObject       *pCtrlHandle;
             pUsbEpHandle           hUbOutEp;
             Uint16                 lunNum;
             Uint16                 acAppBuffer[420];
             CSL_Status             status;
             Uint16                 request;

             // Assign values to all the members in acAppHandle struct
             acAppHandle.pAcObj = &acAppBuffer[0];
             .....
             .....
             status = AC_Open(&acAppHandle);
             ....
             ....
             pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
                                                       pCtrlHandle->acReqTable);

             pCtrlHandle->fpRequestHandler(pCtrlHandle->devNum,
                                           &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_AcRequestRet AC_reqSetCurrent(CSL_UsbDevNum           devNum,
                                    CSL_UsbSetupStruct    *usbSetup,
                                    pUsbEpHandle          hInEp,
                                    pUsbEpHandle          hOutEp,
                                    void                  *pAcObj)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcCtrlObject    *pCtrlHandle;
    CSL_AcObject        *pAcHandle;
    CSL_UsbEpNum        epNum;
    Uint16              tempLen;
    CSL_AcRequestRet    requestRet;
    CSL_Status			status;
    Uint16				dummy = 0;

    requestRet  = CSL_AC_REQUEST_SEND_ACK;
    pAcClassHdl = (pAcClassHandle)(pAcObj);
    pCtrlHandle = &pAcClassHdl->ctrlHandle;
    pAcHandle = &pAcClassHdl->acHandle;

    tempLen = usbSetup->wLength;

    if ((usbSetup->bmRequestType & 0x0F) == 0x1) /* request sent to interface */
    {
#ifndef PLAY_ONLY
      	if (usbSetup->wIndex == IF_NUM_REC){
			if ((usbSetup->wValue >> 8) == AUDIO_AS_ACT_ALT_SETTINGS){
				USB_postTransaction(hOutEp, tempLen, (void*)&pCtrlHandle->alt_setting_rec[0],
									CSL_USB_IOFLAG_NONE);
				alt_setting_rec = pCtrlHandle->alt_setting_rec[1];
		        if (alt_setting_rec == 0)
		        {
		            if (pAcHandle->recordActive == TRUE)
		            {
		                /* stop recording */
		                status = pAcHandle->stopRecordAudio(dummy, (void*)&dummy);
		                if (status == CSL_SOK)
		                {
		                    pAcHandle->recordActive = FALSE;
		                    requestRet = CSL_AC_REQUEST_SEND_ACK;
		                }
		                else
		                {
		                    requestRet = CSL_AC_REQUEST_STALL;
		                }
		            }
		        }
		        else
		        {
		            if (pAcHandle->recordActive == FALSE)
		            {
		                /* start record */
		                status = pAcHandle->initRecordAudio(dummy, &pAcHandle->isoInEpObj.maxPktSiz);
		                if (status == CSL_SOK)
		                {
		                    pAcHandle->recordActive = TRUE;
		                    requestRet = CSL_AC_REQUEST_SEND_ACK;
		                }
		                else
		                {
		                    requestRet = CSL_AC_REQUEST_STALL;
		                }
		            }

		        }

			}
			else return(CSL_AC_REQUEST_STALL);
      	}

      	else if (usbSetup->wIndex == IF_NUM_PLAY){
#else
      	if (usbSetup->wIndex == IF_NUM_PLAY){
#endif
			if ((usbSetup->wValue >> 8) == AUDIO_AS_ACT_ALT_SETTINGS){
				USB_postTransaction(hOutEp, tempLen, (void*)&pCtrlHandle->alt_setting_play[0],
									CSL_USB_IOFLAG_NONE);
				alt_setting_play = pCtrlHandle->alt_setting_play[1];
		        if (alt_setting_play == 0)
		        {
		            if (pAcHandle->playBackActive == TRUE)
		            {
		                /* stop playback */
		                status = pAcHandle->stopPlayAudio(dummy, (void*)&dummy);
		                if (status == CSL_SOK)
		                {
		                    pAcHandle->playBackActive = FALSE;
		                    requestRet = CSL_AC_REQUEST_SEND_ACK;
		                }
		                else
		                {
		                    requestRet = CSL_AC_REQUEST_STALL;
		                }
		            }
		        }
		        else
		        {
		            if (pAcHandle->playBackActive == FALSE)
		            {
		                /* start playback */
		                status = pAcHandle->initPlayAudio(dummy, (void*)&dummy);
		                if (status == CSL_SOK)
		                {
		                    pAcHandle->playBackActive = TRUE;
		                    requestRet = CSL_AC_REQUEST_SEND_ACK;
		                }
		                else
		                {
		                    requestRet = CSL_AC_REQUEST_STALL;
		                }
		            }
		        }
		    }
			else return(CSL_AC_REQUEST_STALL);
      	}

    	else if ((usbSetup->wIndex>>8)==0x41) // input clock source ID
		{
			if ((usbSetup->wValue>>8)==1) // CS_SAM_FREQ_CONTROL
			{
	            gRecSampRateChange = TRUE;

				/* host wants to set the current sampling frequency (in 4 bytes) */
		        pCtrlHandle->sampleRateBuf[1] = 0xFFFF;
		        pCtrlHandle->sampleRateBuf[2] = 0xFFFF;
				// get the assigned sample frequency from host (in 4 bytes)
	        	USB_postTransaction(hOutEp, tempLen, (void*)&pCtrlHandle->sampleRateBuf[0],
							        CSL_USB_IOFLAG_NONE);
			} else
			{
				return(CSL_AC_REQUEST_STALL);
			}
		} else if ((usbSetup->wIndex >> CSL_AC_8BIT_SHIFT) == 0x2) /* FU ID=2 => playback */
        {
            switch (usbSetup->wValue)
            {
                case ((USB_AC_FEATURE_UNIT_MUTE_CTRL << 8) | USB_AC_FEATURE_UNIT_MASTER):
                    mute_flag_change = TRUE;

                    // get playback mute
                    pCtrlHandle->muteCtrlBuf[0] = 0xffff;
                    pCtrlHandle->muteCtrlBuf[1] = 0xffff;
                    USB_postTransaction(hOutEp, tempLen, 
                        (void*)&pCtrlHandle->muteCtrlBuf[0],
                        CSL_USB_IOFLAG_NONE);
                    break;

                case ((USB_AC_FEATURE_UNIT_MUTE_CTRL << 8) | USB_AC_FEATURE_UNIT_LEFT):
                    mute_flag_change = TRUE;

                    // get playback mute
                    pCtrlHandle->muteCtrlBuf[0] = 0xffff;
                    pCtrlHandle->muteCtrlBuf[1] = 0xffff;
                    USB_postTransaction(hOutEp, tempLen, 
                        (void*)&pCtrlHandle->muteCtrlBuf[0],
                        CSL_USB_IOFLAG_NONE);
                    break;

                case ((USB_AC_FEATURE_UNIT_MUTE_CTRL << 8) | USB_AC_FEATURE_UNIT_RIGHT):
                    mute_flag_change = TRUE;

                    // get playback mute
                    pCtrlHandle->muteCtrlBuf[0] = 0xffff;
                    pCtrlHandle->muteCtrlBuf[1] = 0xffff;
                    USB_postTransaction(hOutEp, tempLen, 
                        (void*)&pCtrlHandle->muteCtrlBuf[0],
                        CSL_USB_IOFLAG_NONE);
                    break;

                case ((USB_AC_FEATURE_UNIT_VOLUME_CTRL << 8) | USB_AC_FEATURE_UNIT_LEFT): 
                    playback_volume_flag_change_Left = TRUE;

                    // get playback left vol
                    pCtrlHandle->leftVolBuf[0] = 0xffff;
                    pCtrlHandle->leftVolBuf[1] = 0xffff;
                    USB_postTransaction(hOutEp, tempLen,
                        (void*)&pCtrlHandle->leftVolBuf[0],
                        CSL_USB_IOFLAG_NONE);
                    break;

                case ((USB_AC_FEATURE_UNIT_VOLUME_CTRL << 8) | USB_AC_FEATURE_UNIT_RIGHT): 
                    playback_volume_flag_change_Right = TRUE;

                    // get playback right vol
                    pCtrlHandle->rightVolBuf[0] = 0xffff;
                    pCtrlHandle->rightVolBuf[1] = 0xffff;
                    USB_postTransaction(hOutEp, tempLen,
                        (void*)&pCtrlHandle->rightVolBuf[0],
                        CSL_USB_IOFLAG_NONE);
                    break;

                case ((USB_AC_FEATURE_UNIT_VOLUME_CTRL << 8) | USB_AC_FEATURE_UNIT_MASTER):
                    playback_volume_flag_change_Left_Right = TRUE;

                    // get playback left/right vol
                    pCtrlHandle->leftRightVolBuf[0] = 0xffff;
                    pCtrlHandle->leftRightVolBuf[1] = 0xffff;
                    pCtrlHandle->leftRightVolBuf[2] = 0xffff;
                    USB_postTransaction(hOutEp, tempLen, 
                        (void*)&pCtrlHandle->leftRightVolBuf[0], 
                        CSL_USB_IOFLAG_NONE);
                    break;

                default:
                    requestRet = CSL_AC_REQUEST_STALL;
                    break;
            }
        }
        else if ((usbSetup->wIndex >> CSL_AC_8BIT_SHIFT) == 0x5) /* FU ID=5 => record */
        {
            switch (usbSetup->wValue)
            {
                case (USB_AC_FEATURE_UNIT_MUTE_CTRL << 8):
                    rec_mute_flag_change = TRUE;

                    // get playback mute
                    pCtrlHandle->recordmuteCtrlBuf[0] = 0xffff;
                    pCtrlHandle->recordmuteCtrlBuf[1] = 0xffff;
                    USB_postTransaction(hOutEp, tempLen, 
                        (void*)&pCtrlHandle->recordmuteCtrlBuf[0],
                        CSL_USB_IOFLAG_NONE);
                    break;

                default:
                    requestRet = CSL_AC_REQUEST_STALL;
                    break;
            }
        }
        else
        {
            requestRet = CSL_AC_REQUEST_STALL;
        }
    }
    else if ((usbSetup->bmRequestType & 0x0F) == 0x2)  /* request sent to endpoint */
    {
        epNum = (CSL_UsbEpNum)(usbSetup->wIndex & 0xF);
        if (usbSetup->wIndex & CSL_USB_IN_TRANSFER)
        {
            epNum += CSL_USB_IN_EP0;
        }
        if (epNum == pAcHandle->isoOutEpObj.epNum) /* is data for playback endpoint? */
        {
            gPbSampRateChange = TRUE;

            /* get sample rate */
            pCtrlHandle->sampleRateBuf[0] = 0xffff;
            pCtrlHandle->sampleRateBuf[1] = 0xffff;
            USB_postTransaction(hOutEp, 4,
                (void*)&pCtrlHandle->sampleRateBuf[0],
                CSL_USB_IOFLAG_NONE);
        }
        else if (epNum == pAcHandle->isoInEpObj.epNum) /* is data for record endpoint? */
        {
            gRecSampRateChange = TRUE;

            /* get sample rate */
            pCtrlHandle->recSampleRateBuf[0] = 0xffff;
            pCtrlHandle->recSampleRateBuf[1] = 0xffff;
            USB_postTransaction(hOutEp, 4,
                (void*)&pCtrlHandle->recSampleRateBuf[0],
                CSL_USB_IOFLAG_NONE);
        }
        else
        {
            requestRet = CSL_AC_REQUEST_STALL;
        }
    }
    else
    {
        requestRet = CSL_AC_REQUEST_STALL;
    }

    return(requestRet);
}

/** ============================================================================
 *   @n@b  AC_reqGetCurrent
 *
 *   @b Description
 *   @n This function handles the request from host to get current
 *
 *   @b Arguments
 *   @verbatim
            devNum       - USB device number
            usbSetup     - USb setup structure
            hInEp        - USB ISO In end point handle
            hOutEp       - USB ISO Out end point object
            pAcObj       - USB Audio Class object
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li        CSL_AC_REQUEST_GET_ACK  - Get acknowledgement for request
 *   @li        CSL_AC_REQUEST_STALL    - Request stall signal (Error)
 *
 *   <b> Pre Condition </b>
 *   @n  AC_lookupReqHandler should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Sends requested current value
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
             CSL_AcInitStructApp    acAppHandle;
             CSL_AcObject           *pAcHandle;
             CSL_AcCtrlObject       *pCtrlHandle;
             pUsbEpHandle           hUbOutEp;
             Uint16                 lunNum;
             Uint16                 acAppBuffer[420];
             CSL_Status             status;
             Uint16                 request;

             // Assign values to all the members in acAppHandle struct
             acAppHandle.pAcObj = &acAppBuffer[0];
             .....
             .....
             status = AC_Open(&acAppHandle);
             ....
             ....
             pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
                                                       pCtrlHandle->acReqTable);

             pCtrlHandle->fpRequestHandler(pCtrlHandle->devNum,
                                           &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_AcRequestRet AC_reqGetCurrent(CSL_UsbDevNum         devNum,
                                  CSL_UsbSetupStruct    *usbSetup,
                                  pUsbEpHandle          hInEp,
                                  pUsbEpHandle          hOutEp,
                                  void                  *pAcObj)
{

    pAcClassHandle      pAcClassHdl;
    CSL_AcCtrlObject    *pCtrlHandle;
    CSL_AcObject        *pAcHandle;
    CSL_UsbEpNum        epNum;
    Uint16              tempLen;
    CSL_AcRequestRet    requestRet;

    requestRet  = CSL_AC_REQUEST_GET_ACK;
    pAcClassHdl = (pAcClassHandle)(pAcObj);
    pCtrlHandle = &pAcClassHdl->ctrlHandle;
    pAcHandle = &pAcClassHdl->acHandle;

    tempLen = usbSetup->wLength;

    if ((usbSetup->bmRequestType & 0x0F) == 0x01)		// request sent to interface
    {
#ifndef PLAY_ONLY
    	// first check whether anything sent to the audio streaming interfaces
    	if (usbSetup->wIndex == IF_NUM_REC){
    		if ((usbSetup->wValue >> 8) == AUDIO_AS_VAL_ALT_SETTINGS){
				pCtrlHandle->ctrlBuffer[1] = 0x0301; // alt 0 and 1 valid
				USB_postTransaction(hInEp, tempLen, (void*)&pCtrlHandle->ctrlBuffer[0],
							        CSL_USB_IOFLAG_NONE | CSL_USB_IOFLAG_NOSHORT);
    		}
    		else if ((usbSetup->wValue >> 8) == AUDIO_AS_ACT_ALT_SETTINGS){
				USB_postTransaction(hInEp, tempLen, (void*)&pCtrlHandle->alt_setting_rec[0],
							        CSL_USB_IOFLAG_NONE | CSL_USB_IOFLAG_NOSHORT);
    		}
			else if ((usbSetup->wValue >> 8) == AUDIO_AS_AUDIO_DATA_FORMAT){
				pCtrlHandle->ctrlBuffer[1] = 0x0001;
				pCtrlHandle->ctrlBuffer[2] = 0x0000;
				USB_postTransaction(hInEp, tempLen, (void*)&pCtrlHandle->ctrlBuffer[0],
									CSL_USB_IOFLAG_NONE | CSL_USB_IOFLAG_NOSHORT);
			}
			else return(CSL_AC_REQUEST_STALL);
    	}
    	else if (usbSetup->wIndex == IF_NUM_PLAY){
#else
    	if (usbSetup->wIndex == IF_NUM_PLAY){
#endif

			if ((usbSetup->wValue >> 8) == AUDIO_AS_VAL_ALT_SETTINGS){
				pCtrlHandle->ctrlBuffer[1] = 0x0301; // alt 0 and 1 valid
				USB_postTransaction(hInEp, tempLen, (void*)&pCtrlHandle->ctrlBuffer[0],
									CSL_USB_IOFLAG_NONE | CSL_USB_IOFLAG_NOSHORT);
			}
			else if ((usbSetup->wValue >> 8) == AUDIO_AS_ACT_ALT_SETTINGS){
				USB_postTransaction(hInEp, tempLen, (void*)&pCtrlHandle->alt_setting_play[0],
										CSL_USB_IOFLAG_NONE | CSL_USB_IOFLAG_NOSHORT);
			}
			else if ((usbSetup->wValue >> 8) == AUDIO_AS_AUDIO_DATA_FORMAT){
				pCtrlHandle->ctrlBuffer[1] = 0x0001;
				pCtrlHandle->ctrlBuffer[2] = 0x0000;
				USB_postTransaction(hInEp, tempLen, (void*)&pCtrlHandle->ctrlBuffer[0],
									CSL_USB_IOFLAG_NONE | CSL_USB_IOFLAG_NOSHORT);
			}
			else return(CSL_AC_REQUEST_STALL);
    	}

    	else if ((usbSetup->wIndex>>8)==0x41) // input clock source ID
		{
			if ((usbSetup->wValue>>8)==1) // CS_SAM_FREQ_CONTROL
			{
				/* Send the current sampling frequency (in 4 bytes) */
				// dCUR	
				//pCtrlHandle->ctrlBuffer[1] = (Uint16)(gSetRecSampRate&0xFFFF);
				//pCtrlHandle->ctrlBuffer[2] = (Uint16)(gSetRecSampRate>>16);
				/* Send the current sampling frequency value (in 4 bytes) */
				//USB_postTransaction(hInEp, 4, (void*)&pCtrlHandle->ctrlBuffer[0],
				//			        CSL_USB_IOFLAG_NONE | CSL_USB_IOFLAG_NOSHORT);
				USB_postTransaction(hInEp, tempLen, (void*)&pCtrlHandle->sampleRateBuf[0],
							        CSL_USB_IOFLAG_NONE | CSL_USB_IOFLAG_NOSHORT);
			} else if ((usbSetup->wValue>>8)==2) // CS_CLOCK_VALID_CONTROL
			{

				pCtrlHandle->ctrlBuffer[1] = 0x0001;
				USB_postTransaction(hInEp, tempLen, (void*)&pCtrlHandle->ctrlBuffer[0],
							        CSL_USB_IOFLAG_NONE | CSL_USB_IOFLAG_NOSHORT);
			} else
			{
				return(CSL_AC_REQUEST_STALL);
			}

		} else if ((usbSetup->wIndex >> CSL_AC_8BIT_SHIFT) == 0x2) /* FU ID=2 => playback */
        {
            switch (usbSetup->wValue)
            {
                /* Send mute control value */
                case ((USB_AC_FEATURE_UNIT_MUTE_CTRL << 8) | USB_AC_FEATURE_UNIT_MASTER):
                    USB_postTransaction(hInEp, tempLen, 
                        (void*)&pCtrlHandle->muteCtrlBuf[0], 
                        CSL_USB_IOFLAG_NONE);
                    break;

                /* Send mute control value */
                case ((USB_AC_FEATURE_UNIT_MUTE_CTRL << 8) | USB_AC_FEATURE_UNIT_LEFT):
                    USB_postTransaction(hInEp, tempLen, 
                        (void*)&pCtrlHandle->muteCtrlBuf[0], 
                        CSL_USB_IOFLAG_NONE);
                    break;

                /* Send mute control value */
                case ((USB_AC_FEATURE_UNIT_MUTE_CTRL << 8) | USB_AC_FEATURE_UNIT_RIGHT):
                    USB_postTransaction(hInEp, tempLen, 
                        (void*)&pCtrlHandle->muteCtrlBuf[0], 
                        CSL_USB_IOFLAG_NONE);
                    break;

                    /* Send mute control value for situation when the host driver
                     * asks for both left and right simultaneously - as does the Linux driver
                     */

				case ((USB_AC_FEATURE_UNIT_MUTE_CTRL << 8) | USB_AC_FEATURE_UNIT_RIGHT
								| USB_AC_FEATURE_UNIT_LEFT):
					USB_postTransaction(hInEp, tempLen,
						(void*)&pCtrlHandle->muteCtrlBuf[0],
						CSL_USB_IOFLAG_NONE);
					break;

                /* Send volume control value */
                case ((USB_AC_FEATURE_UNIT_VOLUME_CTRL << 8) | USB_AC_FEATURE_UNIT_LEFT): 
                    USB_postTransaction(hInEp, tempLen,
                        (void*)&pCtrlHandle->leftVolBuf[0],
                        CSL_USB_IOFLAG_NONE);
                    break;

                /* Send volume control value */
                case ((USB_AC_FEATURE_UNIT_VOLUME_CTRL << 8) | USB_AC_FEATURE_UNIT_RIGHT): 
                    USB_postTransaction(hInEp, tempLen,
                        (void*)&pCtrlHandle->rightVolBuf[0],
                        CSL_USB_IOFLAG_NONE);
                    break;

                /* Send volume control value */
                case ((USB_AC_FEATURE_UNIT_VOLUME_CTRL << 8) | USB_AC_FEATURE_UNIT_MASTER):
                    USB_postTransaction(hInEp, tempLen, 
                        (void*)&pCtrlHandle->leftRightVolBuf[0], 
                        CSL_USB_IOFLAG_NONE);
                    break;

                default:
                    requestRet = CSL_AC_REQUEST_STALL;
                    break;
            }
        }
        else if ((usbSetup->wIndex >> CSL_AC_8BIT_SHIFT) == 0x5) /* FU ID=5 => record */
        {
            switch (usbSetup->wValue)
            {
                /* Send mute control value */
                case ((USB_AC_FEATURE_UNIT_MUTE_CTRL << 8) | USB_AC_FEATURE_UNIT_MASTER):
                    USB_postTransaction(hInEp, tempLen, 
                        (void*)&pCtrlHandle->recordmuteCtrlBuf[0], 
                        CSL_USB_IOFLAG_NONE);
                    break;

                /* Send mute control value */
                case ((USB_AC_FEATURE_UNIT_MUTE_CTRL << 8) | USB_AC_FEATURE_UNIT_LEFT):
                    USB_postTransaction(hInEp, tempLen, 
                        (void*)&pCtrlHandle->recordmuteCtrlBuf[0], 
                        CSL_USB_IOFLAG_NONE);
                    break;

                /* Send mute control value */
                case ((USB_AC_FEATURE_UNIT_MUTE_CTRL << 8) | USB_AC_FEATURE_UNIT_RIGHT):
                    USB_postTransaction(hInEp, tempLen, 
                        (void*)&pCtrlHandle->recordmuteCtrlBuf[0], 
                        CSL_USB_IOFLAG_NONE);
                    break;

                /* Send volume control value */
                case ((USB_AC_FEATURE_UNIT_VOLUME_CTRL << 8) | USB_AC_FEATURE_UNIT_LEFT): 
                    USB_postTransaction(hInEp, tempLen,
                        (void*)&pCtrlHandle->recordleftVolBuf[0],
                        CSL_USB_IOFLAG_NONE);
                    break;

                /* Send volume control value */
                case ((USB_AC_FEATURE_UNIT_VOLUME_CTRL << 8) | USB_AC_FEATURE_UNIT_RIGHT): 
                    USB_postTransaction(hInEp, tempLen,
                        (void*)&pCtrlHandle->recordrightVolBuf[0],
                        CSL_USB_IOFLAG_NONE);
                    break;

                /* Send volume control value */
                case ((USB_AC_FEATURE_UNIT_VOLUME_CTRL << 8) | USB_AC_FEATURE_UNIT_MASTER):
                    USB_postTransaction(hInEp, tempLen, 
                        (void*)&pCtrlHandle->recordleftRightVolBuf[0], 
                        CSL_USB_IOFLAG_NONE);
                    break;

                default:
                    requestRet = CSL_AC_REQUEST_STALL;
                    break;
            }
        }
        else
        {
            requestRet = CSL_AC_REQUEST_STALL;
        }
    }
    else if ((usbSetup->bmRequestType & 0x0F) == 0x2)  /* request sent to endpoint */
    {
        epNum = (CSL_UsbEpNum)(usbSetup->wIndex & 0xF);
        if (usbSetup->wIndex & CSL_USB_IN_TRANSFER)
        {
            epNum += CSL_USB_IN_EP0;
        }
        if (epNum == pAcHandle->isoOutEpObj.epNum) /* is data for playback endpoint? */
        {
            /* Send sample rate */
            USB_postTransaction(hInEp, 4,
                (void*)&pCtrlHandle->sampleRateBuf[0], 
                CSL_USB_IOFLAG_NONE);
        }
        else if (epNum == pAcHandle->isoInEpObj.epNum) /* is data for record endpoint? */
        {
            /* Send  sample rate */
            USB_postTransaction(hInEp, 4,
                (void*)&pCtrlHandle->recSampleRateBuf[0], 
                CSL_USB_IOFLAG_NONE);
        }
        else
        {
            requestRet = CSL_AC_REQUEST_STALL;
        }
    }
    else
    {
        requestRet = CSL_AC_REQUEST_STALL;
    }

    return(requestRet);
}

/** ============================================================================
 *   @n@b  AC_reqGetMinimum
 *
 *   @b Description
 *   @n This function handles the get minimum request from host
 *
 *   @b Arguments
 *   @verbatim
            devNum       - USB device number
            usbSetup     - USb setup structure
            hInEp        - USB ISO In end point handle
            hOutEp       - USB ISO Out end point object
            pAcObj       - USB Audio Class object
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li        CSL_AC_REQUEST_GET_ACK  - Get acknowledgement for request
 *   @li        CSL_AC_REQUEST_STALL    - Request stall signal (Error)
 *
 *   <b> Pre Condition </b>
 *   @n  AC_lookupReqHandler should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Sends requested minimum value
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
             CSL_AcInitStructApp    acAppHandle;
             CSL_AcObject           *pAcHandle;
             CSL_AcCtrlObject       *pCtrlHandle;
             pUsbEpHandle           hUbOutEp;
             Uint16                 lunNum;
             Uint16                 acAppBuffer[420];
             CSL_Status             status;
             Uint16                 request;

             // Assign values to all the members in acAppHandle struct
             acAppHandle.pAcObj = &acAppBuffer[0];
             .....
             .....
             status = AC_Open(&acAppHandle);
             ....
             ....
             pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
                                                       pCtrlHandle->acReqTable);

             pCtrlHandle->fpRequestHandler(pCtrlHandle->devNum,
                                           &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_AcRequestRet AC_reqGetMinimum(CSL_UsbDevNum         devNum,
                                  CSL_UsbSetupStruct    *usbSetup,
                                  pUsbEpHandle          hInEp,
                                  pUsbEpHandle          hOutEp,
                                  void                  *pAcObj)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcCtrlObject    *pCtrlHandle;
    Uint16              tempLen;
    CSL_Status          status;

    pAcClassHdl = (pAcClassHandle)(pAcObj);
    pCtrlHandle = &pAcClassHdl->ctrlHandle;
    status      = CSL_SOK;

    tempLen = usbSetup->wLength;

    status = USB_postTransaction(hInEp, tempLen,
                                 (void*)(&pCtrlHandle->getMinBuffer[0]),
                                  CSL_USB_IOFLAG_NONE);
    if(status != CSL_SOK)
    {
        return(CSL_AC_REQUEST_STALL);
    }

    return(CSL_AC_REQUEST_GET_ACK);
}

/** ============================================================================
 *   @n@b  AC_reqGetMaximum
 *
 *   @b Description
 *   @n This function handles the get maximum request from host
 *
 *   @b Arguments
 *   @verbatim
            devNum       - USB device number
            usbSetup     - USb setup structure
            hInEp        - USB ISO In end point handle
            hOutEp       - USB ISO Out end point object
            pAcObj       - USB Audio Class object
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li        CSL_AC_REQUEST_GET_ACK  - Get acknowledgement for request
 *   @li        CSL_AC_REQUEST_STALL    - Request stall signal (Error)
 *
 *   <b> Pre Condition </b>
 *   @n  AC_lookupReqHandler should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Sends requested maximum value
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
             CSL_AcInitStructApp    acAppHandle;
             CSL_AcObject           *pAcHandle;
             CSL_AcCtrlObject       *pCtrlHandle;
             pUsbEpHandle           hUbOutEp;
             Uint16                 lunNum;
             Uint16                 acAppBuffer[420];
             CSL_Status             status;
             Uint16                 request;

             // Assign values to all the members in acAppHandle struct
             acAppHandle.pAcObj = &acAppBuffer[0];
             .....
             .....
             status = AC_Open(&acAppHandle);
             ....
             ....
             pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
                                                       pCtrlHandle->acReqTable);

             pCtrlHandle->fpRequestHandler(pCtrlHandle->devNum,
                                           &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_AcRequestRet AC_reqGetMaximum(CSL_UsbDevNum         devNum,
                                  CSL_UsbSetupStruct    *usbSetup,
                                  pUsbEpHandle          hInEp,
                                  pUsbEpHandle          hOutEp,
                                  void                  *pAcObj)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcCtrlObject    *pCtrlHandle;
    Uint16              tempLen;
    CSL_Status          status;

    pAcClassHdl = (pAcClassHandle)(pAcObj);
    pCtrlHandle = &pAcClassHdl->ctrlHandle;
    status      = CSL_SOK;

    tempLen = usbSetup->wLength;

    status = USB_postTransaction(hInEp, tempLen,
                                 (void*)(&pCtrlHandle->getMaxBuffer[0]),
                                  CSL_USB_IOFLAG_NONE);
    if(status != CSL_SOK)
    {
        return(CSL_AC_REQUEST_STALL);
    }

    return(CSL_AC_REQUEST_GET_ACK);
}

/** ============================================================================
 *   @n@b  AC_reqGetResolution
 *
 *   @b Description
 *   @n This function handles the get resolution request from host
 *
 *   @b Arguments
 *   @verbatim
            devNum       - USB device number
            usbSetup     - USb setup structure
            hInEp        - USB ISO In end point handle
            hOutEp       - USB ISO Out end point object
            pAcObj       - USB Audio Class object
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li        CSL_AC_REQUEST_GET_ACK  - Get acknowledgement for request
 *   @li        CSL_AC_REQUEST_STALL    - Request stall signal (Error)
 *
 *   <b> Pre Condition </b>
 *   @n  AC_lookupReqHandler should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Sends requested resolution value
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
             CSL_AcInitStructApp    acAppHandle;
             CSL_AcObject           *pAcHandle;
             CSL_AcCtrlObject       *pCtrlHandle;
             pUsbEpHandle           hUbOutEp;
             Uint16                 lunNum;
             Uint16                 acAppBuffer[420];
             CSL_Status             status;
             Uint16                 request;

             // Assign values to all the members in acAppHandle struct
             acAppHandle.pAcObj = &acAppBuffer[0];
             .....
             .....
             status = AC_Open(&acAppHandle);
             ....
             ....
             pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
                                                       pCtrlHandle->acReqTable);

             pCtrlHandle->fpRequestHandler(pCtrlHandle->devNum,
                                           &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_AcRequestRet AC_reqGetResolution(CSL_UsbDevNum         devNum,
                                     CSL_UsbSetupStruct    *usbSetup,
                                     pUsbEpHandle          hInEp,
                                     pUsbEpHandle          hOutEp,
                                     void                  *pAcObj)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcCtrlObject    *pCtrlHandle;
    Uint16              tempLen;
    CSL_Status          status;

    pAcClassHdl = (pAcClassHandle)(pAcObj);
    pCtrlHandle = &pAcClassHdl->ctrlHandle;
    status      = CSL_SOK;

    tempLen = usbSetup->wLength;

    status = USB_postTransaction(hInEp, tempLen,
                                 (void*)(&pCtrlHandle->getResBuffer[0]),
                                  CSL_USB_IOFLAG_NONE);
    if(status != CSL_SOK)
    {
        return(CSL_AC_REQUEST_STALL);
    }

    return(CSL_AC_REQUEST_GET_ACK);
}

/** ============================================================================
 *   @n@b  AC_reqSetInterface
 *
 *   @b Description
 *   @n This function handles the request from host to set interafce
 *   unit number
 *
 *   @b Arguments
 *   @verbatim
             devNum       - USB device number
             usbSetup     - USb setup structure
             hInEp        - USB ISO In end point handle
             hOutEp       - USB ISO Out end point object
             pAcObj       - USB Audio Class object
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li        CSL_AC_REQUEST_SEND_ACK - Send acknowledgement for request
 *   @li        CSL_AC_REQUEST_STALL    - Request stall signal (Error)
 *
 *   <b> Pre Condition </b>
 *   @n  AC_lookupReqHandler should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Sends acknowledgement
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
             CSL_AcInitStructApp    acAppHandle;
             CSL_AcObject           *pAcHandle;
             CSL_AcCtrlObject       *pCtrlHandle;
             pUsbEpHandle           hUbOutEp;
             Uint16                 lunNum;
             Uint16                 acAppBuffer[420];
             CSL_Status             status;
             Uint16                 request;

             // Assign values to all the members in acAppHandle struct
             acAppHandle.pAcObj = &acAppBuffer[0];
             .....
             .....
             status = AC_Open(&acAppHandle);
             ....
             ....
             pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
                                                       pCtrlHandle->acReqTable);

             pCtrlHandle->fpRequestHandler(pCtrlHandle->devNum,
                                           &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_AcRequestRet AC_reqSetInterface(CSL_UsbDevNum         devNum,
                                    CSL_UsbSetupStruct    *usbSetup,
                                    pUsbEpHandle          hInEp,
                                    pUsbEpHandle          hOutEp,
                                    void                  *pAcObj)
{
    CSL_AcRequestRet    retStat;
    CSL_Status          status;
    pAcClassHandle      pAcClassHdl;
    CSL_AcObject        *pAcHandle;
    CSL_AcCtrlObject    *pCtrlHandle;
    Uint16              dummy;

    pAcClassHdl = (pAcClassHandle)(pAcObj);
    pAcHandle   = &pAcClassHdl->acHandle;
    pCtrlHandle = &pAcClassHdl->ctrlHandle;


    retStat     = CSL_AC_REQUEST_SEND_ACK;
    dummy       = 0;

    // wIndex = Interface
    // wValue = Alternate setting
    if (usbSetup->wIndex == IF_NUM_AC)
    {
        if (usbSetup->wValue == 0)
        {
            retStat = CSL_AC_REQUEST_SEND_ACK;
        }
    }
#ifndef PLAY_ONLY
    else if (usbSetup->wIndex == IF_NUM_REC) /* host enable and disable record audio path */
    {
    	alt_setting_rec = pCtrlHandle->alt_setting_rec[1] = usbSetup->wValue;
        if (alt_setting_rec == 0)
        {
            if (pAcHandle->recordActive == TRUE)
            {
                /* stop recording */
                status = pAcHandle->stopRecordAudio(dummy, (void*)&dummy);
                if (status == CSL_SOK)
                {
                    pAcHandle->recordActive = FALSE;
                    retStat = CSL_AC_REQUEST_SEND_ACK;
                }
                else
                {
                    retStat = CSL_AC_REQUEST_STALL;
                }
            }
        }
        else if (alt_setting_rec == 1)
        {
            if (pAcHandle->recordActive == FALSE)
            {
                /* start record */
                status = pAcHandle->initRecordAudio(dummy, &pAcHandle->isoInEpObj.maxPktSiz);
                if (status == CSL_SOK)
                {
                    pAcHandle->recordActive = TRUE;
                    retStat = CSL_AC_REQUEST_SEND_ACK;
                }
                else
                {
                    retStat = CSL_AC_REQUEST_STALL;
                }
            }
        }
        else
        {
            retStat = CSL_AC_REQUEST_STALL;
        }
    }
#endif
    else if (usbSetup->wIndex == IF_NUM_PLAY)      /* host enable and disable playback path */
    {
    	alt_setting_play = pCtrlHandle->alt_setting_play[1] = usbSetup->wValue;
        if (alt_setting_play == 0)
        {
            //if (pAcHandle->playBackActive == TRUE)
            {
                /* stop playback */
                status = pAcHandle->stopPlayAudio(dummy, (void*)&dummy);
                if (status == CSL_SOK)
                {
                    pAcHandle->playBackActive = FALSE;
                    retStat = CSL_AC_REQUEST_SEND_ACK;
                }
                else
                {
                    retStat = CSL_AC_REQUEST_STALL;
                }
            }
        }
        else if (alt_setting_play == 1)
        {
            //if (pAcHandle->playBackActive == FALSE)
            {
                /* start playback */
                status = pAcHandle->initPlayAudio(dummy, (void*)&dummy);
                if (status == CSL_SOK)
                {
                    pAcHandle->playBackActive = TRUE;
                    retStat = CSL_AC_REQUEST_SEND_ACK;
                }
                else
                {
                    retStat = CSL_AC_REQUEST_STALL;
                }
            }
        }
        else
        {
        	retStat = CSL_AC_REQUEST_STALL;
        }
    }
    else
    {
        /* Interface not supported, STALL the endpoint */
        retStat = CSL_AC_REQUEST_STALL;
    }
    if (alt_setting_play) EZDSP5535_LED_on(1);
    else EZDSP5535_LED_off(1);

    return(retStat);
}

/** ============================================================================
 *   @n@b  AC_reqHidGetReport
 *
 *   @b Description
 *   @n This function handles the request from host to get HID report
 *
 *   @b Arguments
 *   @verbatim
             devNum       - USB device number
             usbSetup     - USb setup structure
             hInEp        - USB ISO In end point handle
             hOutEp       - USB ISO Out end point object
             pAcObj       - USB Audio Class object
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li        CSL_AC_REQUEST_SEND_ACK - Send acknowledgement for request
 *   @li        CSL_AC_REQUEST_STALL    - Request stall signal (Error)
 *
 *   <b> Pre Condition </b>
 *   @n  AC_lookupReqHandler should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Sends acknowledgement
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
             CSL_AcInitStructApp    acAppHandle;
             CSL_AcObject           *pAcHandle;
             CSL_AcCtrlObject       *pCtrlHandle;
             pUsbEpHandle           hUbOutEp;
             Uint16                 lunNum;
             Uint16                 acAppBuffer[420];
             CSL_Status             status;
             Uint16                 request;

             // Assign values to all the members in acAppHandle struct
             acAppHandle.pAcObj = &acAppBuffer[0];
             .....
             .....
             status = AC_Open(&acAppHandle);
             ....
             ....
             pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
                                                       pCtrlHandle->acReqTable);

             pCtrlHandle->fpRequestHandler(pCtrlHandle->devNum,
                                           &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_AcRequestRet AC_reqHidGetReport(
    CSL_UsbDevNum         devNum,
    CSL_UsbSetupStruct    *usbSetup,
    pUsbEpHandle          hInEp,
    pUsbEpHandle          hOutEp,
    void                  *pAcObj
)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcCtrlObject    *pCtrlHandle;
    CSL_AcObject        *pAcHandle;
    Uint16              tempLen;
    CSL_Status          status;
	CSL_AcRequestRet    retStat;


    pAcClassHdl = (pAcClassHandle)(pAcObj);
    pCtrlHandle = &pAcClassHdl->ctrlHandle;

    if ((usbSetup->wIndex == pCtrlHandle->acHidIfNum) && /* check HID interface */
        (usbSetup->wValue == (CSL_AC_HID_INPUT_REPORT_TYPE << 8) | pCtrlHandle->acHidReportId)) /* check valid Report Type (MSB) and ID (LSB) */
    {
        pAcHandle   = &pAcClassHdl->acHandle;

        /* Get report data */
        status = pAcHandle->getHidReport(pCtrlHandle->acHidReportLen, 
            &pAcHandle->lbaBufferHidReport[0]);

        /* Send data */
        if (status == CSL_SOK)
        {
            tempLen = pCtrlHandle->acHidReportLen; /* length of report descriptor */
            /* select the smaller of two  */
            tempLen =
                (tempLen < usbSetup->wLength) ? tempLen : usbSetup->wLength;

            /* Post transaction to USB */
            USB_postTransaction(hInEp, tempLen, 
                (void *)(&pAcHandle->lbaBufferHidReport[0]), 
                CSL_USB_IOFLAG_NONE);

            retStat = CSL_AC_REQUEST_GET_ACK;
        }
        else
        {
            retStat = CSL_AC_REQUEST_STALL;
        }
    }
    else
    {
	    retStat = CSL_AC_REQUEST_STALL;
    }

	return retStat;
}

/** ============================================================================
 *   @n@b  AC_reqHidGetIdle
 *
 *   @b Description
 *   @n This function handles the request from host to get the HID idle rate
 *
 *   @b Arguments
 *   @verbatim
             devNum       - USB device number
             usbSetup     - USb setup structure
             hInEp        - USB ISO In end point handle
             hOutEp       - USB ISO Out end point object
             pAcObj       - USB Audio Class object
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li        CSL_AC_REQUEST_SEND_ACK - Send acknowledgement for request
 *   @li        CSL_AC_REQUEST_STALL    - Request stall signal (Error)
 *
 *   <b> Pre Condition </b>
 *   @n  AC_lookupReqHandler should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Sends acknowledgement
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
             CSL_AcInitStructApp    acAppHandle;
             CSL_AcObject           *pAcHandle;
             CSL_AcCtrlObject       *pCtrlHandle;
             pUsbEpHandle           hUbOutEp;
             Uint16                 lunNum;
             Uint16                 acAppBuffer[420];
             CSL_Status             status;
             Uint16                 request;

             // Assign values to all the members in acAppHandle struct
             acAppHandle.pAcObj = &acAppBuffer[0];
             .....
             .....
             status = AC_Open(&acAppHandle);
             ....
             ....
             pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
                                                       pCtrlHandle->acReqTable);

             pCtrlHandle->fpRequestHandler(pCtrlHandle->devNum,
                                           &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_AcRequestRet AC_reqHidGetIdle(
    CSL_UsbDevNum         devNum,
    CSL_UsbSetupStruct    *usbSetup,
    pUsbEpHandle          hInEp,
    pUsbEpHandle          hOutEp,
    void                  *pAcObj
)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcCtrlObject    *pCtrlHandle;
	CSL_AcRequestRet    retStat;

    pAcClassHdl = (pAcClassHandle)(pAcObj);
    pCtrlHandle = &pAcClassHdl->ctrlHandle;

    if ((usbSetup->wIndex == pCtrlHandle->acHidIfNum) && /* check HID interface */
        ((usbSetup->wValue & CSL_AC_8BIT_MASK) == 0x0) || /* check valid Report ID */
        ((usbSetup->wValue & CSL_AC_8BIT_MASK) == pCtrlHandle->acHidReportId))
    {

        USB_postTransaction(hInEp, 1, (void*)&pCtrlHandle->acHidIdleRate, 
            CSL_USB_IOFLAG_NONE);
        retStat = CSL_AC_REQUEST_GET_ACK;
    }
    else
    {
	    retStat = CSL_AC_REQUEST_STALL;
    }

	return retStat;
}

/** ============================================================================
 *   @n@b  AC_reqHidSetIdle
 *
 *   @b Description
 *   @n This function handles the request from host to set the HID idle rate
 *
 *   @b Arguments
 *   @verbatim
             devNum       - USB device number
             usbSetup     - USb setup structure
             hInEp        - USB ISO In end point handle
             hOutEp       - USB ISO Out end point object
             pAcObj       - USB Audio Class object
     @endverbatim
 *
 *   <b> Return Value </b>
 *   @li        CSL_AC_REQUEST_SEND_ACK - Send acknowledgement for request
 *   @li        CSL_AC_REQUEST_STALL    - Request stall signal (Error)
 *
 *   <b> Pre Condition </b>
 *   @n  AC_lookupReqHandler should be called successfully
 *
 *   <b> Post Condition </b>
 *   @n  Sends acknowledgement
 *
 *   @b Modifies
 *   @n None
 *
 *   @b Example
 *   @verbatim
             CSL_AcInitStructApp    acAppHandle;
             CSL_AcObject           *pAcHandle;
             CSL_AcCtrlObject       *pCtrlHandle;
             pUsbEpHandle           hUbOutEp;
             Uint16                 lunNum;
             Uint16                 acAppBuffer[420];
             CSL_Status             status;
             Uint16                 request;

             // Assign values to all the members in acAppHandle struct
             acAppHandle.pAcObj = &acAppBuffer[0];
             .....
             .....
             status = AC_Open(&acAppHandle);
             ....
             ....
             pCtrlHandle->fpRequestHandler = AC_lookupReqHandler(request,
                                                       pCtrlHandle->acReqTable);

             pCtrlHandle->fpRequestHandler(pCtrlHandle->devNum,
                                           &pCtrlHandle->usbSetup,hCtrlEpIn,hCtrlEpOut,pAcObj);
     @endverbatim
 *  ============================================================================
 */
CSL_AcRequestRet AC_reqHidSetIdle(
    CSL_UsbDevNum         devNum,
    CSL_UsbSetupStruct    *usbSetup,
    pUsbEpHandle          hInEp,
    pUsbEpHandle          hOutEp,
    void                  *pAcObj
)
{
    pAcClassHandle      pAcClassHdl;
    CSL_AcCtrlObject    *pCtrlHandle;
    Uint16              duration;
	CSL_AcRequestRet    retStat;

    pAcClassHdl = (pAcClassHandle)(pAcObj);
    pCtrlHandle = &pAcClassHdl->ctrlHandle;

    if ((usbSetup->wIndex == pCtrlHandle->acHidIfNum) && /* check HID interface */
        ((usbSetup->wValue & CSL_AC_8BIT_MASK) == 0x0) || /* check valid Report ID */
        ((usbSetup->wValue & CSL_AC_8BIT_MASK) == pCtrlHandle->acHidReportId))
    {
        duration = usbSetup->wValue >> CSL_AC_8BIT_SHIFT;
        if (duration == 0) /* check duration is 0 (indefinite) */
        {
            pCtrlHandle->acHidIdleRate = duration;

            retStat = CSL_AC_REQUEST_SEND_ACK;
        }
        else
        {
	        retStat = CSL_AC_REQUEST_STALL;
        }
    }
    else
    {
	    retStat = CSL_AC_REQUEST_STALL;
    }

	return retStat;
}
