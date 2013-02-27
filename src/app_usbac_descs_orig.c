/*
 * $$$MODULE_NAME app_usbac_descs.c
 *
 * $$$MODULE_DESC app_usbac_descs.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include "app_globals.h"
#include "app_usb.h"
#include "psp_i2s.h"
#include "app_usbac_descs.h"

/* defines code descriptors data table */
#define LEB(x)   LSB(x), MSB(x)

#if 0
#pragma DATA_ALIGN(deviceDescriptor, 4);
const Uint16 deviceDescriptor[DEVICE_DESCR_SZ] = 
{
    0x0112,         //bLength           //bDescriptorType
    0x0200,         //bcdUSB
    0x0000,         //bDeviceClass      //bDeviceSubClass
    0x4000,         //bDeviceProtocol   //bMaxPacketSize0
    DEV_VID,        //idVendor
    DEV_PID,        //idProduct
    DEV_BCD,        //bcdDevice
    0x0201,         //iManufacturer     //iProduct
    0x0103          //iSerialNumber     //bNumConfigurations
};
#endif

#pragma DATA_ALIGN(deviceDescriptorB, 4);
const Uint16 deviceDescriptorB[DEVICE_DESCR_B_SZ] =
{
    0x12,           //bLength
    0x01,           //bDescriptorType
    LEB(0x0200),    //bcdUSB
    0x00,           //bDeviceSubClass
    0x00,           //bDeviceClass
    0x00,           //bDeviceProtocol
    0x40,           //bMaxPacketSize0
    LEB(DEV_VID),   //idVendor
    LEB(DEV_PID),   //idProduct
    LEB(DEV_BCD),   //bcdDevice
    0x01,           //iManufacturer
    0x02,           //iProduct
    0x03,           //iSerialNumber
    0x01            //bNumConfigurations
};

#pragma DATA_ALIGN(deviceQualifierDescr, 4);
const Uint16 deviceQualifierDescr[DEVICE_QUALIFIER_DESCR_SZ] = 
{
    0x060A, 
    0x0200, 
    0x0000, 
    0x4000,
    0x0001
};

#if 0
// The Configuration Descriptor
#pragma DATA_ALIGN(configDescriptor, 4);
const Uint16 configDescriptor[CONFIG_DESCR_SZ] =
{
    /* Standard Configuration descriptor */
    0x09,           /* bLength, in bytes */
    0x02,           /* Descriptor Type */
    LEB(CONFIG_DESCR_SZ),    /* wTotallength */
    IF_COUNT,       /* bNumInterfaces=4-Audio Control IF-0, IF-1 Record, IF-2 Playback, IF-3 HID */
    0x01,           /* bConfigurationValue */
    0x00,           /* iConfiguration */
#ifdef APP_USB_SELF_POWERED
    0xC0,           /* bmAttributes */ /* self-powered, no remote wakeup */
#else
    0x80,           /* bmAttributes */ /* bus-powered, no remote wakeup */
#endif
    APP_USB_MAX_CURRENT,  /* max Power, units of 2 mA */

    /* Standard Interface descriptor - Audio Control */
    0x09,           /* bLength, in bytes */
    0x04,           /* bDescriptorType */
    IF_NUM_AC,      /* bInterfaceNumber */
    0x00,           /* bAlternateSetting */
    0x00,           /* bNumEndpoints */
    0x01,           /* bInterfaceClass=Audio Class  */
    0x01,           /* bInterfaceSubClass=Audio Control */
    0x00,           /* bInterfaceProtocol */
    0x00,           /* iInterface */

/* Start of Class-Specific AudioControl Interface Descriptor section */

    /* Interface Header Audio Class Descriptor */
    0x0A,           /* bLength, in bytes */
    0x24,           /* bDescriptorType */
    0x01,           /* bDescriptorSubtype */
    LEB(0x0100),    /* bcdADC */
#ifndef ENABLE_STEREO_RECORD
    LEB(0x004A),    /* wTotalLength (74), length of this descriptor and all Terminal Dscr's */
#else
    LEB(0x004B),    /* wTotalLength (75), length of this descriptor and all Terminal Dscr's */
#endif
    IF_STRM_CNT,    /* bInCollection=2-Audio Streaming Interfaces; Record and Playback */
    IF_NUM_REC,     /* baInterfaceNr=IF-1, First Interface Number-Record Interface */
    IF_NUM_PLAY,    /* baInterfaceNr=IF-2, Second Interface Number-Playback Interface */

    /* Audio Record Terminals Audio Class-Specific Descriptors */
    /* Output Terminal Audio Class Descriptor */
    0x09,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x03,           /* bDescriptorSubtype=OUTPUT_TERMINAL */
    T_OT_STRM_REC,  /* bTerminalID=ID-N */
    LEB(0x0101),    /* wTerminalType=USB STREAMING */
    0x00,           /* bAssocTerminal=NONE */
    T_FU_REC,       /* bSourceID=ID-N */
    0x00,           /* iTerminal=NONE */

    /* Feature Unit Audio Class Descriptor - Record Path */
#ifndef ENABLE_STEREO_RECORD
    0x09,           /* bLength, in bytes */
#else
    0x0A,           /* bLength, in bytes */
#endif
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x06,           /* bDescriptorSubtype=FEATURE_UNIT */
    T_FU_REC,       /* bUnitID=ID-N */
    T_IT_MIC_REC,   /* bSourceID=ID-N */
    0x01,           /* bControlSize=1 bytes per control */
#ifndef ENABLE_STEREO_RECORD
    LEB(0x0001),    /* bmaControlMaster=MUTE */
#else
    0x01,           /* bmaControlMaster=MUTE */
    0x00,           /* bmaControl-Left Mute & Channel Volume */
    0x00,           /* bmaControl-Right Mute & Channel Volume */
#endif
    0x00,           /* iFeature=NONE */

    /* Input Terminal Audio Class Descriptor */
    0x0C,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x02,           /* bDescriptorSubtype=INPUT_TERMINAL */
    T_IT_MIC_REC,   /* bTerminalID=ID-N */
    LEB(0x0201),    /* wTerminalType=MICROPHONE */
    0x00,           /* bAssocTerminal=NONE */
#ifndef ENABLE_STEREO_RECORD
    0x01,           /* bNrChannels=1-MONO */
    LEB(0x0001),    /* wChannelConfig=LEFT FRONT */
#else
    0x02,           /* bNrChannels=1-STEREO */
    LEB(0x0003),    /* wChannelConfig=LEFT & RIGHT FRONT */
#endif
    0x00,           /* iChannelNames=NONE */
    0x00,           /* iTerminal=NONE */

    /* End Audio Record Terminals */

    // Audio Playback Terminals Audio Class-Specific Descriptors
    /* Input Terminal Audio Class Descriptor */
    0x0C,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x02,           /* bDescriptorSubtype=INPUT_TERMINAL */
    T_IT_STRM_PLAY, /* bTerminalID=ID-N */
    LEB(0x0101),    /* wTerminalType */
    0x00,           /* bAssocTerminal=NONE */
    0x02,           /* bNrChannels= 2, STEREO L&R */
    LEB(0x0003),    /* wChannelConfig Bitmap=RDN */
    0x00,           /* iChannelNames=NONE */
    0x00,           /* iTerminal=NONE */

    /* Feature Unit Audio Class Descriptor - Playback Audio Path */
    0x0D,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x06,           /* bDescriptorSubtype=FEATURE_UNIT */
    T_FU_PLAY,      /* bUnitID=ID-N */
    T_IT_STRM_PLAY, /* bSourceID=ID-N */
    0x02,           /* bControlSize=2-Array size of three controls-Master Left & Right Channel */
    LEB(0x0001),    /* bmaControlMaster-Mute */
    LEB(0x0002),    /* bmaControl-Mute & Left Channel Volume */
    LEB(0x0002),    /* bmaControl-Mute & Right Channel Volume */
    0x00,           /* iFeature=NONE */

    /* Output Terminal Audio Class Descriptor */
    0x09,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x03,           /* bDescriptorSubtype=OUTPUT_TERMINAL */
    T_OT_SPKR,      /* bTerminalID=ID-N */
    LEB(0x0301),    /* wTerminalType=SPEAKER */
    0x00,           /* bAssocTerminal=NONE */
    T_FU_PLAY,      /* bSourceID=ID-N */
    0x00,           /* iTerminal=NONE */

    /* End Audio Playback Terminals */

/* End of Class-Specific AudioControl Interface Descriptor section */


    /* Audio Record Interface, IN-Device-to-Host */
    /* Standard Interface descriptor - AudioStreaming (IN), Idle setting */
    0x09,           /* bLength, in bytes */
    0x04,           /* bDescriptorType */
    IF_NUM_REC,     /* bInterfaceNumber=IF-1 RECORD */
    0x00,           /* bAlternateSetting */
    0x00,           /* bNumEndpoints */
    0x01,           /* bInterfaceClass=AUDIO INTERFACE CLASS */
    0x02,           /* bInterfaceSubClass=AUDIO STREAMING */
    0x00,           /* bInterfaceProtocol */
    0x00,           /* iInterface */

    /* Standard Interface descriptor - AudioStreaming (IN) */
    0x09,           /* bLength, in bytes */
    0x04,           /* bDescriptorType */
    IF_NUM_REC,     /* bInterfaceNumber=IF-N RECORD */
    0x01,           /* bAlternateSetting=1 */
    0x01,           /* bNumEndpoints=1, FULL BW INTERFACE */
    0x01,           /* bInterfaceClass=AUDIO INTERFACE CLASS */
    0x02,           /* bInterfaceSubClass=AUDIO STREAMING */
    0x00,           /* bInterfaceProtocol */
    0x00,           /* iInterface */

    /* Class-Specific AS-Audio Stream Interface Descriptor */
    0x07,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x01,           /* bDescriptorSubtype=GENERAL */
    T_OT_STRM_REC,   /* bTerminalLink, links USB IN EP Streaming to Output Terminal */
    0x01,           /* bDelay=1 ms (need to measure) */
    LEB(0x0001),    /* wFormatTag=PCM DATA FORMAT */

    /* Class-Specific Format Type 1 Audio Descriptor */
    0x0B,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x02,           /* bDescriptorSubtype=FORMAT_TYPE */
    0x01,           /* bFormatType = TYPE 1 Data Format */
#ifndef ENABLE_STEREO_RECORD
    0x01,           /* bNrChannels= 1, MONO */
#else
    0x02,           /* bNrChannels= 1, STEREO */
#endif
    0x02,           /* bSubframeSize in bytes=2 bytes */
    0x10,           /* bBitResolution=16 bit depth */
    0x01,           /* bSamFreqType=SINGLE Sample */
    LEB(REC_SAMPLE_RATE), /* bSamFreq=N */
    0x00,           /* bSamFreq MSB (Not used) */

    /* Standard Isochronous Endpoint Descriptor */
    0x09,           /* bLength, in bytes */
    0x05,           /* bDescriptorType=ENDPOINT (from USBAD, Table 4-20) */
    EP_ADDR_REC,    /* bEndpointAddress=EP1-IN Direction, Address 1 */
    0x05,           /* bmAttributes=Usage Type=DATA Endpoint, Sync Type=Asynchronous, Transfer Type=Isochronous */
    LEB(EP_REC_MAXP), /* wMaxPacketSize=34 Bytes */
    0x01,           /* bInterval is number of milliseconds between IN polling */
    0x00,           /* bRefresh */
    0x00,           /* bSynchAddress */

    /* Class-Specific AS Isochronous Endpoint Audio Class Descriptor */
    0x07,           /* bLength, in bytes */
    0x25,           /* bDescriptorType=CS_ENPOINT */
    0x01,           /* bDescriptorSubtype */
    //0x01,           /* bmAttributes=no controls, full packet not necessary */
    0x00,           /* bmAttributes=no controls, full packet not necessary */
    0x00,           /* bLockDelayUnits, asynchronous audio function, must be 0 */
    LEB(0x0000),    /* wLockDelay, must be 0 */
    /* Audio Record Interface */


    /* Audio Playback Interface IF-2, OUT-Host-to-Device */
    /* Standard Interface descriptor - AudioStreaming (OUT), Idle Setting */
    0x09,           /* bLength, in bytes */
    0x04,           /* bDescriptorType */
    IF_NUM_PLAY,    /* bInterfaceNumber=IF-N */
    0x00,           /* bAlternateSetting=0, Idle Setting, Zero Bandwidth */
    0x00,           /* bNumEndpoints=No Endpoints */
    0x01,           /* bInterfaceClass=Audio Class */
    0x02,           /* bInterfaceSubClass=Audio Streaming */
    0x00,           /* bInterfaceProtocol=None */
    0x00,           /* iInterface=None */

    /* Standard Interface descriptor - AudioStreaming, Active Setting */
    0x09,           /* bLength, in bytes */
    0x04,           /* bDescriptorType */
    IF_NUM_PLAY,    /* bInterfaceNumber */
    0x01,           /* bAlternateSetting */
    0x01,           /* bNumEndpoints */
    0x01,           /* bInterfaceClass=Audio Class */
    0x02,           /* bInterfaceSubClass=Audio Streaming */
    0x00,           /* bInterfaceProtocol=None */
    0x00,           /* iInterface */

    /* Class-Specific Audio Stream Audio Class Descriptor */
    0x07,           /* bLength, in bytes */
    0x24,           /* bDescriptorType = CS_INTERFACE */
    0x01,           /* bDescriptorSubtype = GENERAL */
    T_IT_STRM_PLAY, /* bTerminalLink=ID-N, links USB OUT EP Streaming to Input Terminal */
    0x01,           /* bDelay=1 ms (need to measure) */
    LEB(0x0001),    /* wFormatTag=PCM */

    /* Class-Specific Format Type 1 Audio Descriptor */
#ifdef ENABLE_PLAYBACK_TWO_SAMPLE_RATES
    0x0E,           /* bLength, in bytes */
#else
    0x0B,           /* bLength, in bytes */
#endif
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x02,           /* bDescriptorSubtype=FORMAT_TYPE */
    0x01,           /* bFormatType=TYPE I Data Format */
    0x02,           /* bNrChannels=2, STEREO, L&R channels */
    0x02,           /* bSubframeSize=2 bytes */
    0x10,           /* bBitResolution=16-bit */
#ifdef ENABLE_PLAYBACK_TWO_SAMPLE_RATES
    0x02,           /* bSamFreqType=2 Discrete sample */
    LEB(PLAY_SAMPLE_RATE_1), /* bSamFreq=N Hz */
    0x00,           /* bSamFreq upper-most byte of 3-byte value */
    LEB(PLAY_SAMPLE_RATE_2), /* bSamFreq=N Hz */
    0x00,           /* bSamFreq upper-most byte of 3-byte value */
#else // ENABLE_PLAYBACK_TWO_SAMPLE_RATES
    0x01,           /* bSamFreqType=Single (1) Discrete sample */
    LEB(PLAY_SAMPLE_RATE), /* bSamFreq=N Hz */
    0x00,           /* bSamFreq upper-most byte of 3-byte value */
#endif // ENABLE_PLAYBACK_TWO_SAMPLE_RATES

    /* Class-Specific Isochronous Endpoint Descriptor */
    0x09,           /* bLength, in bytes */
    0x05,           /* bDescriptorType=ENDPOINT */
    EP_ADDR_PLAY,   /* bEndpointAddress=EP2-OUT Direction, Address 2 */
    0x09,           /* bmAttributes=Usage Type=DATA Endpoint, Sync Type=Adaptive, Transfer Type=Isochronous */
    LEB(EP_PB_MAXP),    /* wMaxPacketSize=N */
    0x01,           /* bInterval=1.0 MS */
    0x00,           /* bRefresh=NONE */
    0x00,           /* bSynchAddress=NONE */

    /* Class-Specific Isochronous Endpoint Audio Class Descriptor */
    0x07,           /* bLength, in bytes */
    0x25,           /* bDescriptorType=CS_ENPOINT */
    0x01,           /* bDescriptorSubtype=N */
#ifdef ENABLE_PLAYBACK_TWO_SAMPLE_RATES
    0x01,           /* bmAttributes=Sampling Frequency Control, full packet not necessary */
#else
    0x00,           /* bmAttributes=no controls, full packet not necessary */
#endif
    0x00,           /* bLockDelayUnits (need to measure)*/
    LEB(0x0000),    /* wLockDelay, must be 0 */
    /* Playback Inteface */

    /* HID Interface IF-3 */ 
    /* HID Standard Interface Descriptor */
    0x09,           /* bLength, in bytes */
    0x04,           /* bDescriptorType=INTERFACE */
    IF_NUM_HID,     /* bInterfaceNumber */
    0x00,           /* bAlternateSetting -- HID interface only supports Alternate Setting zero */
    0x01,           /* bNumEndpoints -- one interrupt endpoint */
    0x03,           /* bInterfaceClass=HID */
    0x00,           /* bInterfaceSubClass -- not used */
    0x00,           /* bInterfaceProtocol -- not used */
    0x00,           /* iInterface */

    /* HID Descriptor */
    0x09,           /* bLength, in bytes */
    0x21,           /* bDescriptorType=Class-specific HID descriptor */
    LEB(0x0111),    /* bcdHID=HID Class Specification release 1.11 */
    0x00,           /* bCountryCode=None */
    0x01,           /* bNumDescriptors -- 1 Report Descriptor */
    0x22,           /* bDescriptorType -- Report */
    LEB(0x0035),    /* wDescriptorLength -- length of Report Descriptor */

    /* HID Interrupt Endpoint Descriptor */
    0x07,           /* bLength, in bytes */
    0x05,           /* bDescriptorType=ENDPOINT */
    EP_ADDR_HID,    /* bEndpointAddress=EP3-IN Direction, Address 3 */
    0x03,           /* bmAttributes=Usage Type=DATA Endpoint, Transfer Type=Interrupt */ 
    LEB(EP_HID_MAXP), /* wMaxPacketSize -- size of Report */
    0x20            /* bInterval -- 32 msec. */
    /* HID Interface */
};
#endif

// The Configuration Descriptor -- audio record only
#pragma DATA_ALIGN(configDescriptor, 4);
const Uint16 configDescriptor[CONFIG_DESCR_SZ] =
{
    /* Standard Configuration descriptor */
    0x09,           /* bLength, in bytes */
    0x02,           /* Descriptor Type */
    LEB(CONFIG_DESCR_SZ),    /* wTotallength */
    IF_COUNT,       /* bNumInterfaces=2-Audio Control IF-0, IF-1 Record */
    0x01,           /* bConfigurationValue */
    0x00,           /* iConfiguration */
#ifdef APP_USB_SELF_POWERED
    0xC0,           /* bmAttributes */ /* self-powered, no remote wakeup */
#else
    0x80,           /* bmAttributes */ /* bus-powered, no remote wakeup */
#endif
    APP_USB_MAX_CURRENT,  /* max Power, units of 2 mA */

    /* Standard Interface descriptor - Audio Control */
    0x09,           /* bLength, in bytes */
    0x04,           /* bDescriptorType */
    IF_NUM_AC,      /* bInterfaceNumber */
    0x00,           /* bAlternateSetting */
    0x00,           /* bNumEndpoints */
    0x01,           /* bInterfaceClass=Audio Class  */
    0x01,           /* bInterfaceSubClass=Audio Control */
    0x00,           /* bInterfaceProtocol */
    0x00,           /* iInterface */

/* Start of Class-Specific AudioControl Interface Descriptor section */

    /* Interface Header Audio Class Descriptor */
    0x09,           /* bLength, in bytes */
    0x24,           /* bDescriptorType */
    0x01,           /* bDescriptorSubtype */
    LEB(0x0100),    /* bcdADC */
#ifndef ENABLE_STEREO_RECORD
    LEB(0x0027),    /* wTotalLength (39), length of this descriptor and all Terminal Dscr's */
#else
    LEB(0x0028),    /* wTotalLength (40), length of this descriptor and all Terminal Dscr's */
#endif
    IF_STRM_CNT,    /* bInCollection=2-Audio Streaming Interfaces; Record and Playback */
    IF_NUM_REC,     /* baInterfaceNr=IF-1, First Interface Number-Record Interface */

    /* Audio Record Terminals Audio Class-Specific Descriptors */
    /* Output Terminal Audio Class Descriptor */
    0x09,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x03,           /* bDescriptorSubtype=OUTPUT_TERMINAL */
    T_OT_STRM_REC,  /* bTerminalID=ID-N */
    LEB(0x0101),    /* wTerminalType=USB STREAMING */
    0x00,           /* bAssocTerminal=NONE */
    T_FU_REC,       /* bSourceID=ID-N */
    0x00,           /* iTerminal=NONE */

    /* Feature Unit Audio Class Descriptor - Record Path */
#ifndef ENABLE_STEREO_RECORD
    0x09,           /* bLength, in bytes */
#else
    0x0A,           /* bLength, in bytes */
#endif
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x06,           /* bDescriptorSubtype=FEATURE_UNIT */
    T_FU_REC,       /* bUnitID=ID-N */
    T_IT_MIC_REC,   /* bSourceID=ID-N */
    0x01,           /* bControlSize=1 bytes per control */
#ifndef ENABLE_STEREO_RECORD
    LEB(0x0001),    /* bmaControlMaster=MUTE */
#else
    0x01,           /* bmaControlMaster=MUTE */
    0x00,           /* bmaControl-Left Mute & Channel Volume */
    0x00,           /* bmaControl-Right Mute & Channel Volume */
#endif
    0x00,           /* iFeature=NONE */

    /* Input Terminal Audio Class Descriptor */
    0x0C,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x02,           /* bDescriptorSubtype=INPUT_TERMINAL */
    T_IT_MIC_REC,   /* bTerminalID=ID-N */
    LEB(0x0201),    /* wTerminalType=MICROPHONE */
    0x00,           /* bAssocTerminal=NONE */
#ifndef ENABLE_STEREO_RECORD
    0x01,           /* bNrChannels=1-MONO */
    LEB(0x0001),    /* wChannelConfig=LEFT FRONT */
#else
    0x02,           /* bNrChannels=1-STEREO */
    LEB(0x0003),    /* wChannelConfig=LEFT & RIGHT FRONT */
#endif
    0x00,           /* iChannelNames=NONE */
    0x00,           /* iTerminal=NONE */

    /* End Audio Record Terminals */

/* End of Class-Specific AudioControl Interface Descriptor section */


    /* Audio Record Interface, IN-Device-to-Host */
    /* Standard Interface descriptor - AudioStreaming (IN), Idle setting */
    0x09,           /* bLength, in bytes */
    0x04,           /* bDescriptorType */
    IF_NUM_REC,     /* bInterfaceNumber=IF-1 RECORD */
    0x00,           /* bAlternateSetting */
    0x00,           /* bNumEndpoints */
    0x01,           /* bInterfaceClass=AUDIO INTERFACE CLASS */
    0x02,           /* bInterfaceSubClass=AUDIO STREAMING */
    0x00,           /* bInterfaceProtocol */
    0x00,           /* iInterface */

    /* Standard Interface descriptor - AudioStreaming (IN) */
    0x09,           /* bLength, in bytes */
    0x04,           /* bDescriptorType */
    IF_NUM_REC,     /* bInterfaceNumber=IF-N RECORD */
    0x01,           /* bAlternateSetting=1 */
    0x01,           /* bNumEndpoints=1, FULL BW INTERFACE */
    0x01,           /* bInterfaceClass=AUDIO INTERFACE CLASS */
    0x02,           /* bInterfaceSubClass=AUDIO STREAMING */
    0x00,           /* bInterfaceProtocol */
    0x00,           /* iInterface */

    /* Class-Specific AS-Audio Stream Interface Descriptor */
    0x07,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x01,           /* bDescriptorSubtype=GENERAL */
    T_OT_STRM_REC,   /* bTerminalLink, links USB IN EP Streaming to Output Terminal */
    0x01,           /* bDelay=1 ms (need to measure) */
    LEB(0x0001),    /* wFormatTag=PCM DATA FORMAT */

    /* Class-Specific Format Type 1 Audio Descriptor */
    0x0B,           /* bLength, in bytes */
    0x24,           /* bDescriptorType=CS_INTERFACE */
    0x02,           /* bDescriptorSubtype=FORMAT_TYPE */
    0x01,           /* bFormatType = TYPE 1 Data Format */
#ifndef ENABLE_STEREO_RECORD
    0x01,           /* bNrChannels= 1, MONO */
#else
    0x02,           /* bNrChannels= 1, STEREO */
#endif
    0x02,           /* bSubframeSize in bytes=2 bytes */
    0x10,           /* bBitResolution=16 bit depth */
    0x01,           /* bSamFreqType=SINGLE Sample */
    LEB(REC_SAMPLE_RATE), /* bSamFreq=N */
    0x00,           /* bSamFreq MSB (Not used) */

    /* Standard Isochronous Endpoint Descriptor */
    0x09,           /* bLength, in bytes */
    0x05,           /* bDescriptorType=ENDPOINT (from USBAD, Table 4-20) */
    EP_ADDR_REC,    /* bEndpointAddress=EP1-IN Direction, Address 1 */
    0x05,           /* bmAttributes=Usage Type=DATA Endpoint, Sync Type=Asynchronous, Transfer Type=Isochronous */
    LEB(EP_REC_MAXP), /* wMaxPacketSize=34 Bytes */
    0x01,           /* bInterval is number of milliseconds between IN polling */
    0x00,           /* bRefresh */
    0x00,           /* bSynchAddress */

    /* Class-Specific AS Isochronous Endpoint Audio Class Descriptor */
    0x07,           /* bLength, in bytes */
    0x25,           /* bDescriptorType=CS_ENPOINT */
    0x01,           /* bDescriptorSubtype */
    //0x01,           /* bmAttributes=no controls, full packet not necessary */
    0x00,           /* bmAttributes=no controls, full packet not necessary */
    0x00,           /* bLockDelayUnits, asynchronous audio function, must be 0 */
    LEB(0x0000),    /* wLockDelay, must be 0 */
    /* Audio Record Interface */
};

const Uint16 stringLanId[STRING_LAN_ID_SZ] = 
{
    0x0000, 
    0x0304, 
    0x0409, 
    0x0000
};

char strDesc[4][20] = 
{
    // string 0 English-USA
    {0x0304, 0x0304, 0x0409},
    // string 1 "Texas Instruments"
    {0x0324, 0x0324, 0x0054, 0x0065, 0x0078, 0x0061, 0x0073, 0x0020, 0x0049, 0x006E, 0x0073, 
    0x0074, 0x0072, 0x0075, 0x006D, 0x0065, 0x006E, 0x0074, 0x0073},
    // string 2 "TI C55x USB Audio"
    {0x0324, 0x0324, 0x0054, 0x0049, 0x0020, 0x0043, 0x0035, 0x0035, 0x0078, 0x0020,  
    0x0055, 0x0053, 0x0042, 0x0020, 0x0041, 0x0075, 0x0064, 0x0069, 0x006F},
    // string 3 "320001"
    {0x030E, 0x030E, 0x0033, 0x0032, 0x0030, 0x0030, 0x0030, 0x0031}
};

const char *string_descriptor[STRING_DESCR_SZ] =
{
    &strDesc[0][0],     /* Language String */
    &strDesc[1][0],     /* iManufacturer */
    &strDesc[2][0],     /* iProductName */
    &strDesc[3][0],     /* iSerial No :RomId - F# */
    NULL,               /*  */
    NULL,               /*  */
    NULL
};

// The AC HID Report Descriptor
#pragma DATA_ALIGN(acHidReportDescriptor,4);
const Uint16 acHidReportDescriptor[AC_HID_REPORT_DESCR_SZ] =
{
    /* HID Report Descriptor */
    0x05, 0x0C,     /* USAGE_PAGE (Consumer Devices) */
    0x09, 0x01,     /* USAGE (Consumer Remote Control) */
    0xA1, 0x01,     /* COLLECTION (Application) */
    0x85, 0x01,     /* Report ID (0x01) */
    0x15, 0x00,     /* LOGICAL_MINIMUM (0) */
    0x25, 0x01,     /* LOGICAL_MAXIMUM (1) */
    0x75, 0x01,     /* REPORT_SIZE (1) */
    0x95, 0x01,     /* REPORT_COUNT (1) */
    0x09, 0xE9,     /* USAGE (Volume Up) */
    0x81, 0x02,     /* INPUT (Data, Var, Abs) */
    0x09, 0xEA,     /* Usage (Volume Down) */
    0x81, 0x02,     /* INPUT (Data, Var, Abs) */
    0x09, 0xE2,     /* USAGE (Mute) */
    0x81, 0x06,     /* INPUT (Data, Var, Rel) */
    0x81, 0x01,     /* Input (Cnst) */
    0x81, 0x01,     /* Input (Cnst) */
    0x81, 0x01,     /* Input (Cnst) */
    0x81, 0x01,     /* Input (Cnst) */
    0x81, 0x01,     /* Input (Cnst) */
    0x05, 0x0B,     /* USAGE_PAGE (Telephony Devices) */
    0x81, 0x01,     /* Input (Cnst) */
    0x81, 0x01,     /* Input (Cnst) */
    0x09, 0x2F,     /* USAGE (Microphone Mute) */
    0x81, 0x06,     /* INPUT (Data, Var, Rel) */
    0x95, 0x05,     /* REPORT_COUNT (5) */
    0x81, 0x01,     /* INPUT (Cnst) (Pad report to 8 bits) */
    0xC0,           /* END_COLLECTION */
};

