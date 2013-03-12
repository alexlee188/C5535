/*
 * $$$MODULE_NAME app_usb.h
 *
 * $$$MODULE_DESC app_usb.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _APP_USB_H_
#define _APP_USB_H_

#include <std.h>
#include "csl_usb.h"
#include "app_globals.h"
#include "psp_i2s.h"

/* USB interface numbers */
#define IF_NUM_AC       ( 0 ) /* Interface Number Audio Control, must be zero */
#define IF_NUM_REC      ( 1 ) /* Interface Number Audio Record */
#ifdef PLAY_ONLY
#define IF_NUM_PLAY		( 1 )
#define IF_NUM_HID		( 2 )
#define IF_NUM_FBCK		( 3 )
#else
#define IF_NUM_PLAY     ( 2 ) /* Interface Number Audio Playback */
#define IF_NUM_HID      ( 3 ) /* Interface Number HID */
#ifdef FEEDBACKEP
#define IF_NUM_FBCK     ( 4 ) /* Interface Number FBCK */
#endif //FEEDBACKEP
#endif //PLAY_ONLY
/* USB endpoint numbers & interrupts */
#define EP_NUM_REC      ( CSL_USB_EP1 )
#define EP_NUM_PLAY     ( CSL_USB_EP2 )
#define EP_NUM_HID      ( CSL_USB_EP3 )
#ifdef FEEDBACKEP
#define EP_NUM_FBCK     ( CSL_USB_EP4 )
#endif //FEEDBACKEP
#define USB_TX_INT_EP_REC   ( 1<<EP_NUM_REC )
#define USB_RX_INT_EP_PLAY  ( 1<<(8+EP_NUM_PLAY) )
#define USB_TX_INT_EP_HID   ( 1<<EP_NUM_HID )
#ifdef FEEDBACKEP
#define USB_TX_INT_EP_FBCK  ( 1<<(EP_NUM_FBCK) )
#endif //FEEDBACKEP

/* USB descriptor sample rate settings */
#define RATE_16_KHZ     ( 0x3E80 )
#define RATE_32_KHZ     ( 0x7D00 )
#define RATE_44_1_KHZ   ( 0xAC44 )
#define RATE_48_KHZ     ( 0xBB80 )
#define RATE_96_KHZ     ( 0x7700 )

///#ifdef SAMPLE_RATE_RX_96kHz
#ifdef SAMP_24BIT
#ifdef USE_FOUR_CODEC
//#define EP_REC_MAXP     ( 0x0180 ) /* maximum packet size for record endpoint */
#define EP_REC_MAXP     ( 0x0300 ) /* maximum packet size for record endpoint */
#else
#ifdef USE_THREE_CODEC
//#define EP_REC_MAXP     ( 0x0120 ) /* maximum packet size for record endpoint */
#define EP_REC_MAXP     ( 0x0240 ) /* maximum packet size for record endpoint */
#else
#ifdef USE_TWO_CODEC
//#define EP_REC_MAXP     ( 0x00C0 ) /* maximum packet size for record endpoint */
#define EP_REC_MAXP     ( 0x0180 ) /* maximum packet size for record endpoint */
#else
//#define EP_REC_MAXP     ( 0x0060 ) /* maximum packet size for record endpoint */
#define EP_REC_MAXP     ( 0x00C0 ) /* maximum packet size for record endpoint */
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#else
#ifdef USE_FOUR_CODEC
//#define EP_REC_MAXP     ( 0x00C0 ) /* maximum packet size for record endpoint */
#define EP_REC_MAXP     ( 0x0180 ) /* maximum packet size for record endpoint */
#else
#ifdef USE_THREE_CODEC
//#define EP_REC_MAXP     ( 0x0090 ) /* maximum packet size for record endpoint */
#define EP_REC_MAXP     ( 0x0120 ) /* maximum packet size for record endpoint */
#else
#ifdef USE_TWO_CODEC
//#define EP_REC_MAXP     ( 0x0060 ) /* maximum packet size for record endpoint */
#define EP_REC_MAXP     ( 0x00C0 ) /* maximum packet size for record endpoint */
#else
//#define EP_REC_MAXP     ( 0x0030 ) /* maximum packet size for record endpoint */
#define EP_REC_MAXP     ( 0x0060 ) /* maximum packet size for record endpoint */
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#endif //SAMP_24BIT
///#endif //SAMPLE_RATE_RX_96kHz

///#ifdef SAMPLE_RATE_TX_96kHz
#ifdef SAMP_24BIT
#ifdef USE_FOUR_CODEC
//#define EP_PB_MAXP      ( 0x0180 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#define EP_PB_MAXP      ( 0x0300 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#else
#ifdef USE_THREE_CODEC
//#define EP_PB_MAXP      ( 0x0120 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#define EP_PB_MAXP      ( 0x0240 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#else
#ifdef USE_TWO_CODEC
//#define EP_PB_MAXP      ( 0x00C0 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#define EP_PB_MAXP      ( 0x0180 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#else
//#define EP_PB_MAXP      ( 0x0060 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#define EP_PB_MAXP      ( 0x00C0 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#else
#ifdef USE_FOUR_CODEC
//#define EP_PB_MAXP      ( 0x00C0 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#define EP_PB_MAXP      ( 0x0180 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#else
#ifdef USE_THREE_CODEC
//#define EP_PB_MAXP      ( 0x0090 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#define EP_PB_MAXP      ( 0x0120 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#else
#ifdef USE_TWO_CODEC
//#define EP_PB_MAXP      ( 0x0060 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#define EP_PB_MAXP      ( 0x00C0 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#else
//#define EP_PB_MAXP      ( 0x0030 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#define EP_PB_MAXP      ( 0x0060 ) /* maximum packet size for playback endpoint */ // note can't currently be changed
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#endif //SAMP_24BIT
///#endif //SAMPLE_RATE_TX_96kHz

#define EP_HID_MAXP     ( 0x0003 ) /* maximum packet size for HID endpoint */

#define APP_USB_SELF_POWERED    /* define if self-powered USB device */
#ifdef APP_USB_SELF_POWERED
#define APP_USB_MAX_CURRENT     ( 200/2 ) /* max current in units of 2 mA */
#else
#define APP_USB_MAX_CURRENT     ( 500/2 ) /* max current in units of 2 mA */
#endif

/* USB Setup packet bit fields for USB bmRequestType */
#define CSL_USB_REQTYPE_XFER_HTOD      ( 0x0 << 7 )
#define CSL_USB_REQTYPE_XFER_DTOH      ( 0x1 << 7 )
#define CSL_USB_REQTYPE_STANDARD       ( 0x0 << 5 )
#define CSL_USB_REQTYPE_CLASS          ( 0x1 << 5 )
#define CSL_USB_REQTYPE_VENDOR         ( 0x2 << 5 )
#define CSL_USB_REQTYPE_RCVR_DEVICE    ( 0x0 << 0 )
#define CSL_USB_REQTYPE_RCVR_INTERFACE ( 0x1 << 0 )
#define CSL_USB_REQTYPE_RCVR_ENDPOINT  ( 0x2 << 0 )

      //! \name AudioStreaming Interface Control Selectors pp. A.17.11
      //! @{
#define  AUDIO_AS_UNDEFINED                                0x00
#define  AUDIO_AS_ACT_ALT_SETTINGS                         0x01
#define  AUDIO_AS_VAL_ALT_SETTINGS                         0x02
#define  AUDIO_AS_AUDIO_DATA_FORMAT                        0x03
      //! @}


extern Bool usb_rec_mode;
extern Bool usb_rec_start;

extern Bool usb_play_mode;
extern Bool usb_play_start;

extern Bool  no_main_task;

/* HID endpoint control variable */
extern Bool hidIntInEpReady;

#if 0 // debug
extern Uint16 gFaddr_PowerDbg;
extern Uint16 gRxFifoSz_TxFifoSzDbg;
extern Uint16 gTxFifoAddrDbg;
extern Uint16 gTxMaxpDbg;
extern Uint16 gTxCsrDbg;
extern Uint16 gRxFifoAddrDbg;
extern Uint16 gRxMaxpDbg;
extern Uint16 gRxCsrDbg;
extern Uint16 gCsr0Dbg;
extern Uint16 gSaveIndex;
#endif

Bool StartDevice(
    pUsbContext pContext
);

Bool StopDevice(
    pUsbContext pContext
);

Bool HandleUSBInterrupt(
    pUsbContext pContext
);

void  MUSB_Handle_EP0_Intr(
    pUsbContext pContext
);

void MUSB_Handle_Resume_Intr(void);

void CSL_suspendCallBack(
    CSL_Status    status
);

void CSL_selfWakeupCallBack(
    CSL_Status    status
);

CSL_Status StartTransfer(
    void    *vpContext,
    void    *vpeps
);

CSL_Status CompleteTransfer(
    void    *vpContext,
    void    *vpeps
);

/**
 *  \brief  Function to send device notification
 *
 *  \param  pContext    - USB context structure pointer
 *  \param  wUSBEvents  - USB events
 *
 *  \return None
 */
void DeviceNotification(
    pUsbContext    pContext,
    WORD           wUSBEvents
);

void USB_configEndpointDataSize(
    pUsbContext    pContext,
    DWORD          dwEndpoint
);

/* Reads TXMAXP for selected endpoint */
Uint16 USB_getTxMaxp(Uint16 epNum);
/* Writes TXMAXP for selected endpoint */
Uint16 USB_setTxMaxp(Uint16 epNum, Uint16 val);
/* Reads RXMAXP for selected endpoint */
Uint16 USB_getRxMaxp(Uint16 epNum);
/* Writes RXMAXP for selected endpoint */
Uint16 USB_setRxMaxp(Uint16 epNum, Uint16 val);
/* Reads TXCSR for selected endpoint */
Uint16 USB_getTxCsr(Uint16 epNum);
/* Writes TXCSR for selected endpoint */
Uint16 USB_setTxCsr(Uint16 epNum, Uint16 val);
/* Reads RXCSR for selected endpoint */
Uint16 USB_getRxCsr(Uint16 epNum);
/* Writes RXCSR for selected endpoint */
Uint16 USB_setRxCsr(Uint16 epNum, Uint16 val);

#endif /* _APP_USB_H_ */
