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

#include <std.h>
#include "app_globals.h"
#include "app_usb.h"
#include "psp_i2s.h"
#include "app_usbac_descs.h"

/* defines code descriptors data table */
#define LEB(x)   LSB(x), MSB(x)

// FL: modify for IAD
#pragma DATA_ALIGN(deviceDescriptorB, 4);
const Uint16 deviceDescriptorB[DEVICE_DESCR_B_SZ] =
{
    0x12,           //bLength
    DEVICE,         //bDescriptorType
    LEB(0x0200),    //bcdUSB
    0xEF,           //bDeviceClass
    0x02, 			//AUDIO,     		//bDeviceSubClass
    0x01,           //bDeviceProtocol
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
    0x0A,				//bLength
    DEVICE_QUALIFIER,   //bDescriptorType
    LEB(0x0200),		//bcdUSB
    0xEF,           	//bDeviceClass
    0x02, 				//AUDIO,			//bDeviceSubClass
    0x01,           	//bDeviceProtocol
    0x40,           	//bMaxPacketSize0
    0x01,				//bNumConfigurations
    0x00				//bReserved
};

// The Configuration Descriptor -- audio record only
#pragma DATA_ALIGN(configDescriptor, 4);
const Uint16 configDescriptor[CONFIG_DESCR_SZ] =
{
	// (9+8+9+9)=35
    /* Standard Configuration descriptor */
    0x09,           /* bLength, in bytes */
    CONFIGURATION,  /* Descriptor Type */
    LEB(CONFIG_DESCR_SZ),    /* wTotallength */
    IF_COUNT,       /* bNumInterfaces=2-Audio Control IF-0, IF-1 Record */
    0x01,           /* bConfigurationValue */
    0x00,           /* iConfiguration */
#ifdef APP_USB_SELF_POWERED
    0xC0,           /* bmAttributes */ /* self-powered, no remote wakeup */
    APP_USB_MAX_CURRENT,
#else
    0x80,           /* bmAttributes */ /* bus-powered, no remote wakeup */
    APP_USB_MAX_CURRENT,  /* max Power, units of 2 mA */
#endif //APP_USB_SELF_POWERED

    /*                       */
    /* Audio Class Interface */
    /*                       */
    /* IAD for Audio Class function */
    0x08,           /* bLength, in bytes */
    INTERFACE_ASSOCIATION,  /* bDescriptorType=IAD */
    IF_NUM_AC,      /* bFirstInterface */
    IF_COUNT,       /* bInterfaceCount */
    0x01,           /* bFunctionClass AUDIO_FUNCTION (0x01) */
    0x00,           /* bFunctionSubClass FUNCTION_SUBCLASS_UNDEFINED (0x00) */
    0x20,           /* bFunctionProtocol AF_VERSION_02_00 (0x20) */	
    0x02,           /* iFunction=TI C55x USB Audio */

   /* Standard Interface descriptor - Audio Control */
    0x09,           /* bLength, in bytes */
    INTERFACE,      /* bDescriptorType=INTERFACE */
    IF_NUM_AC,      /* bInterfaceNumber */
    0x00,           /* bAlternateSetting (no optional interrupt endpoint) */
    0x00,           /* bNumEndpoints */
    0x01,           /* bInterfaceClass=Audio Class  */
    0x01,           /* bInterfaceSubClass=Audio Control */
    0x20,           /* bInterfaceProtocol */
    0x02,           /* iInterface=TI C55x USB Audio */

/* Start of Class-Specific AudioControl Interface Descriptor section */
    /* Interface Header Audio Class Descriptor */
    0x09,           /* bLength, in bytes */
    CS_INTERFACE,   /* bDescriptorType */
    HEADER,         /* bDescriptorSubtype */
    LEB(0x0200),    /* bcdADC */
	0x08,			/* bCatagory I/O Box (08)*/

#ifdef RECORD_ONLY
#ifdef USE_FOUR_CODEC
    LEB(0x0060),    /* wTotalLength [(9+16+71)=96], length of this descriptor and all Terminal Dscr's */
#else
#ifdef USE_THREE_CODEC
    LEB(0x0058),    /* wTotalLength [(9+16+63)=88], length of this descriptor and all Terminal Dscr's */
#else
#ifdef USE_TWO_CODEC
    LEB(0x0050),    /* wTotalLength [(9+16+55)=80], length of this descriptor and all Terminal Dscr's */
#else
    LEB(0x0048),    /* wTotalLength [(9+16+47)=72], length of this descriptor and all Terminal Dscr's */
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#endif //RECORD_ONLY

#ifdef PLAY_ONLY
#ifdef USE_FOUR_CODEC
    LEB(0x0060),    /* wTotalLength [(9+16+71)=96], length of this descriptor and all Terminal Dscr's */
#else
#ifdef USE_THREE_CODEC
    LEB(0x0058),    /* wTotalLength [(9+16+63)=88], length of this descriptor and all Terminal Dscr's */
#else
#ifdef USE_TWO_CODEC
    LEB(0x0050),    /* wTotalLength [(9+16+55)=80], length of this descriptor and all Terminal Dscr's */
#else
    LEB(0x0048),    /* wTotalLength [(9+16+47)=72], length of this descriptor and all Terminal Dscr's */
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#endif //PLAY_ONLY

#ifdef PLAY_RECORD 
#ifdef USE_FOUR_CODEC
    LEB(0x00A7),    /* wTotalLength (9+16+71+71)=167, length of this descriptor and all Terminal Dscr's */
#else
#ifdef USE_THREE_CODEC
    LEB(0x0097),    /* wTotalLength (9+16+63+63)=151, length of this descriptor and all Terminal Dscr's */
#else
#ifdef USE_TWO_CODEC
    LEB(0x0087),    /* wTotalLength (9+16+55+55)=135, length of this descriptor and all Terminal Dscr's */
#else
    LEB(0x0077),    /* wTotalLength (9+16+47+47)=119, length of this descriptor and all Terminal Dscr's */
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#endif //PLAY_RECORD
    0x00,     		/* bmControls  */

	// (8+8)=16
    /* Audio Record Terminals Audio Class-Specific Descriptors */
    /* Clock Source Descriptor (4.7.2.1) */ 
    0x08,			/* bLength: 8 */
    CS_INTERFACE,	/* bDescriptorType */
    CLOCK_SOURCE,	/* bDescriptorSubtype */
    T_IT_CLK_SRC,	/* bClockID */
    0x01,			/* bmAttributes:   
					D[1:0] :
						00: External Clock 
						01: Internal Fixed Clock
						10: Internal Variable Clock
						11: Internal Progamable Clock 
					D[2]   : Clock synced to SOF
					D[7:3] : Reserved (0) */
    0x07,			/* 5   bmControls       
					D[1:0] : Clock Freq Control
					D[3:2] : Clock Validity Control
					D[7:4] : Reserved (0) */
    0x00,        	/* 6   bAssocTerminal */
///#ifdef SAMPLE_RATE_RX_96kHz
    0x09,        	/* 7   iClockSource=96KHZ */
///#endif //SAMPLE_RATE_RX_96kHz
  
   /* Clock Selector Descriptor (4.7.2.2) */ 
    0x08,			/* bLength 7+p */
    CS_INTERFACE,	/* bDescriptorType=CS_INTERFACE */
    CLOCK_SELECTOR,	/* bDescriptorSubtype */
    T_IT_CLK_SEL,	/* bClockID */
    0x01,			/* Number of input pins*/
    T_IT_CLK_SRC,
    //0x01,			/* bmControls
    0x03,			/* readable and writable
					D[1:0] : Clock Selector Control
					D[7:4] : Reserved (0) */
    0x05,			/* iClockSel=CLKSEL */  

#if (defined(RECORD_ONLY)||defined(PLAY_RECORD))
	/* Begin of Record Path */ // (17+18(26, 34)+12)=47(55, 63), 
    /* Input Terminal Descriptor (Analogue Input Terminal)  - Record Path */
    0x11,			/* bLength in bytes=17 */
    CS_INTERFACE,	/* bDescriptorType=CS_INTERFACE */
    INPUT_TERMINAL,	/* bDescriptorSubType=INPUT_TERMINAL */
    T_IT_MIC_REC,	/* bTerminalID */
    LEB(0x0201),	/* wTerminalType=MICROPHONE */
    0x00,			/* bAssocTerminal */
    T_IT_CLK_SRC,	/* bCSourceID: ID of Clock Entity */
#ifdef USE_FOUR_CODEC
    0x08,			/* bNrChannels */
#else
#ifdef USE_THREE_CODEC
    0x06,			/* bNrChannels */
#else
#ifdef USE_TWO_CODEC
    0x04,			/* bNrChannels */
#else
    0x02,			/* bNrChannels */
#endif	// USE_TWO_CODEC
#endif	// USE_THREE_CODEC
#endif	// USE_FOUR_CODEC
    0x03, 0x00, 0x00, 0x00,		/* bmChannelConfig */
    0x12,			/* iChannelNames=INLFT1, INRHT1, INLFT2, INRHT2, INLFT2, INRHT3 */
    0x00, 0x00,		/* bmControls */
    0x06,			/* iTerminal=MIC */

    /* Feature Unit Audio Class Descriptor - Record Path */
#ifdef USE_FOUR_CODEC
    0x2A,           /* bLength, in bytes 6+(ch+1)*4 */
#else
#ifdef USE_THREE_CODEC
    0x22,           /* bLength, in bytes 6+(ch+1)*4 */
#else
#ifdef USE_TWO_CODEC
    0x1A,           /* bLength, in bytes 6+(ch+1)*4 */
#else
    0x12,           /* bLength, in bytes 6+(ch+1)*4 */
#endif	// USE_TWO_CODEC
#endif	// USE_THREE_CODEC
#endif	// USE_FOUR_CODEC
    CS_INTERFACE,   /* bDescriptorType=CS_INTERFACE */
    FEATURE_UNIT,   /* bDescriptorSubtype=FEATURE_UNIT */
    T_FU_REC,       /* bUnitID=ID-N */
    T_IT_MIC_REC,   /* bSourceID=ID-N */
#ifdef USE_FOUR_CODEC
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl master*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left2*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right2*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left3*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right3*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left4*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right4*/
#else
#ifdef USE_THREE_CODEC
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl master*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left2*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right2*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left3*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right3*/
#else
#ifdef USE_TWO_CODEC
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl master*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left2*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right2*/
#else
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl master*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right1*/
#endif	// USE_TWO_CODEC
#endif	// USE_THREE_CODEC
#endif	// USE_FOUR_CODEC
    0x08,           /* iFeature=RECORD */

    /* Output Terminal Audio Class Descriptor - Record Path */
    0x0C,           /* bLength, in bytes */
    CS_INTERFACE,   /* bDescriptorType=CS_INTERFACE */
    OUTPUT_TERMINAL,/* bDescriptorSubtype=OUTPUT_TERMINAL */
    T_OT_STRM_REC,  /* bTerminalID=ID-N */
    LEB(0x0101),    /* wTerminalType=USB STREAMING */
    0x00,           /* bAssocTerminal=NONE */
    T_FU_REC,       /* bSourceID=ID-N */
	T_IT_CLK_SRC,	/* bCSourceUD */
	0x00, 0x00,		/* bmControls */
	0x07,           /* iTerminal=USBIN */
	/* End of Record Path */
#endif	// (defined(RECORD_ONLY)||defined(PLAY_RECORD))

#if (defined(PLAY_ONLY)||defined(PLAY_RECORD))
	/* Begin of Playback Path */ // (17+18(26, 34)+12)=47(55, 63)
    /* Input Terminal Descriptor (USB Input Terminal) - Playback Path */
    0x11, 		     	/* 0  bLength in bytes=17 */
    CS_INTERFACE, 		/* 1  bDescriptorType=CS_INTERFACE */
    INPUT_TERMINAL, 	/* 2  bDescriptorSubType=INPUT_TERMINAL */
    T_IT_STRM_PLAY, 	/* 3  bTerminalID */
    LEB(0x0101),		/* 4  wTerminalType: USB Streaming */
    0x00, 		     	/* 6  bAssocTerminal */
    T_IT_CLK_SRC, 		/* 7  bCSourceID: ID of Clock Entity */
#ifdef USE_FOUR_CODEC
    0x08,				/* 8  bNrChannels */
#else
#ifdef USE_THREE_CODEC
    0x06,				/* 8  bNrChannels */
#else
#ifdef USE_TWO_CODEC
    0x04,				/* 8  bNrChannels */
#else
    0x02,				/* 8  bNrChannels */
#endif	// USE_TWO_CODEC
#endif	// USE_THREE_CODEC
#endif	// USE_FOUR_CODEC
    0x03, 0x00, 0x00, 0x00,            /* 9  bmChannelConfig */
    0x1A,           	/* 13 iChannelNames=OUTLF1, OUTRH1, OUTLF2, OUTRH2, OUTLF3, OUTRH3 */
    0x00, 0x00,     	/* 14 bmControls */
    0x0B,           	/* 16 iTerminal= USBOUT */

    /* Feature Unit Audio Class Descriptor - Playback Path */
#ifdef USE_FOUR_CODEC
    0x2A,           /* bLength, in bytes 6+(ch+1)*4 */
#else
#ifdef USE_THREE_CODEC
    0x22,           /* bLength, in bytes 6+(ch+1)*4 */
#else
#ifdef USE_TWO_CODEC
    0x1A,           /* bLength, in bytes 6+(ch+1)*4 */
#else
    0x12,           /* bLength, in bytes 6+(ch+1)*4 */
#endif	// USE_TWO_CODEC
#endif	// USE_THREE_CODEC
#endif	// USE_FOUR_CODEC
    CS_INTERFACE,	/* bDescriptorType=CS_INTERFACE */
    FEATURE_UNIT,   /* bDescriptorSubtype=FEATURE_UNIT */
    T_FU_PLAY,      /* bUnitID=ID-N */
    T_IT_STRM_PLAY,   /* bSourceID=ID-N */
#ifdef USE_FOUR_CODEC
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl master*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left2*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right2*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left3*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right3*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left4*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right4*/
#else
#ifdef USE_THREE_CODEC
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl master*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left2*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right2*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left3*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right3*/
#else
#ifdef USE_TWO_CODEC
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl master*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left2*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right2*/
#else
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl master*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl left1*/
    0x0F, 0x00, 0x00, 0x00,    /* bmaControl right1*/
#endif	// USE_TWO_CODEC
#endif	// USE_THREE_CODEC
#endif	// USE_FOUR_CODEC
    0x0C,           /* iFeature=PLAY */

    /* Output Terminal Audio Class Descriptor - Playback Path */
    0x0C,           /* bLength, in bytes */
    CS_INTERFACE,   /* bDescriptorType=CS_INTERFACE */
    OUTPUT_TERMINAL,/* bDescriptorSubtype=OUTPUT_TERMINAL */
    T_OT_SPKR,		/* bTerminalID=ID-N */
    LEB(0x0301),    /* wTerminalType=Speaker */
    0x00,           /* bAssocTerminal=NONE */
    T_FU_PLAY,       /* bSourceID=ID-N */
	T_IT_CLK_SRC,	/* bCSourceUD */
	0x00, 0x00,		/* bmControls */
	0x0A,           /* iTerminal=SPK */
	/* End of Playback Path */
#endif	// (defined(PLAY_ONLY)||defined(PLAY_RECORD))

/* End of Class-Specific AudioControl Interface Descriptor section */

#if (defined(RECORD_ONLY)||defined(PLAY_RECORD))
    /* Audio Record Interface, IN-Device-to-Host */ // (9+9+16+6+7+8)=55
    /* Standard Interface descriptor - AudioStreaming (IN), Idle setting */
    0x09,           /* bLength, in bytes */
    INTERFACE,           /* bDescriptorType=INTERFACE */
    IF_NUM_REC,     /* bInterfaceNumber=IF-1 RECORD */
    0x00,           /* bAlternateSetting */
    0x00,           /* bNumEndpoints */
    0x01,           /* bInterfaceClass=AUDIO INTERFACE CLASS */
    0x02,           /* bInterfaceSubClass=AUDIO STREAMING */
    0x20,           /* bInterfaceProtocol */
    0x0E,           /* iInterface=INCTL */

    /* Standard Interface descriptor - AudioStreaming (IN) */
    0x09,           /* bLength, in bytes */
    INTERFACE,      /* bDescriptorType=INTERFACE */
    IF_NUM_REC,     /* bInterfaceNumber=IF_NUM_REC */
    0x01,           /* bAlternateSetting=1 */
#ifdef FEEDBACKEP
    0x02,     		/* bNumEndpoints=2 */
#else
    0x01,     		/* bNumEndpoints=1 */
#endif
    0x01,           /* bInterfaceClass=AUDIO INTERFACE CLASS */
    0x02,           /* bInterfaceSubClass=AUDIO STREAMING */
    0x20,           /* bInterfaceProtocol */
    0x0F,           /* iInterface=INSTR */

    /* Class-Specific AS-Audio Stream Interface Descriptor */
    0x10,           /* bLength, in bytes */
    CS_INTERFACE,      /* bDescriptorType=CS_INTERFACE */
    AS_GENERAL,     /* bDescriptorSubtype=AS_GENERAL */
    T_OT_STRM_REC,  /* bTerminalLink, links USB IN EP Streaming to Output Terminal */
    0x07,			/* bmControls alt settings r/w, valid alt settings r */
    0x01,			/* bFormatType = PCM */
    0x01, 0x00, 0x00, 0x00,		/* bmFormats (note this is a bitmap) = PCM only */
#ifdef USE_FOUR_CODEC
    0x08,           /* bNrChannels */
#else
#ifdef USE_THREE_CODEC
    0x06,           /* bNrChannels */
#else
#ifdef USE_TWO_CODEC
    0x04,           /* bNrChannels */
#else
    0x02,           /* bNrChannels */
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
    0x03, 0x00, 0x00, 0x00,    	/* bmChannelConfig */
    0x12,			/* iChannelNames=INLFT1, INRHT1, INLFT2, INRHT2, INLFT2, INRHT3 */

    /* Class-Specific Format Type 1 Audio Descriptor */
    0x06,           /* bLength, in bytes */
    CS_INTERFACE,   /* bDescriptorType=CS_INTERFACE */
    FORMAT_TYPE,    /* bDescriptorSubtype=FORMAT_TYPE */
    0x01,           /* bFormatType = TYPE 1 Data Format */
#ifdef SAMP_24BIT
    //0x03,           /* bSubframeSize in bytes=3 bytes */
    0x04,           /* bSubframeSize in bytes=4 bytes */
    0x18,           /* bBitResolution=24 bit depth */
#else
    0x02,           /* bSubframeSize in bytes=2 bytes */
    0x10,           /* bBitResolution=16 bit depth */
#endif //SAMP_24BIT

    /* Standard Isochronous Endpoint Descriptor */
    0x07,           /* bLength, in bytes */
    ENDPOINT,           /* bDescriptorType=ENDPOINT (from USBAD, Table 4-20) */
    EP_ADDR_REC,    /* bEndpointAddress=EP1-IN Direction, Address 1 */
    0x25,           /* bmAttributes=Usage Type=Implicit feedback Data Endpoint, 
    				Sync Type=Asynchronous, Transfer Type=Isochronous */
    LEB(EP_REC_MAXP), /* wMaxPacketSize=48 Bytes */
    0x01,           /* bInterval is related to uFrames between IN polling */

    /* Class-Specific AS Isochronous Endpoint Audio Class Descriptor */
    0x08,           /* bLength, in bytes */
    CS_ENDPOINT,     /* bDescriptorType=CS_ENDPOINT */
    AS_GENERAL,     /* bDescriptorSubtype */
    0x00,           /* bmAttributes=no controls, full packet not necessary */
    0x00,           /* bmControls (Bitmap: Pitch control, over/underun etc) */
    0x00,           /* bLockDelayUnits, asynchronous audio function, must be 0 */
    LEB(0x0000),    /* wLockDelay, must be 0 */
    /* Audio Record Interface */
#endif // (defined(RECORD_ONLY)||defined(PLAY_RECORD))

#if (defined(PLAY_ONLY)||defined(PLAY_RECORD))
#ifdef FEEDBACKEP
    /* Audio Playback Interface, IN-Device-to-Host */ // (9+9+16+6+7+8+7)=62
#else
    /* Audio Playback Interface, IN-Device-to-Host */ // (9+9+16+6+7+8)=55
#endif //FEEDBACKEP
    /* Standard Interface descriptor - AudioStreaming (OUT), Idle setting */
    0x09,           /* bLength, in bytes */
    INTERFACE,           /* bDescriptorType=INTERFACE */
    IF_NUM_PLAY,     /* bInterfaceNumber=IF-2 PLAY */
    0x00,           /* bAlternateSetting */
    0x00,           /* bNumEndpoints */
    AUDIOINTERFACECLASS,   /* bInterfaceClass=AUDIO INTERFACE CLASS */
    AUDIOSTREAMING, /* bInterfaceSubClass=AUDIO STREAMING */
    0x20,           /* bInterfaceProtocol */
    0x10,           /* iInterface=OUTCTL */

    /* Standard Interface descriptor - AudioStreaming (OUT) */
    0x09,           /* bLength, in bytes */
    INTERFACE,      /* bDescriptorType=INTERFACE */
    IF_NUM_PLAY,    /* bInterfaceNumber=IF_NUM_PLAY */
    0x01,           /* bAlternateSetting=1 */
#ifdef FEEDBACKEP
    0x02,
#else
    0x01,    		/* bNumEndpoints=1 */
#endif
    AUDIOINTERFACECLASS,  /* bInterfaceClass=AUDIO INTERFACE CLASS */
    AUDIOSTREAMING, /* bInterfaceSubClass=AUDIO STREAMING */
    0x20,           /* bInterfaceProtocol */
    0x11,           /* iInterface=OUTSTR */

    /* Class-Specific AS-Audio Stream Interface Descriptor */
    0x10,           /* bLength, in bytes */
    CS_INTERFACE,   /* bDescriptorType=CS_INTERFACE */
    AS_GENERAL,     /* bDescriptorSubtype=GENERAL */
    T_IT_STRM_PLAY,  /* bTerminalLink, links USB OUT EP Streaming to Output Terminal */
    0x07,			/* bmControls = active alt settings r/w, valid alt settings r*/
    0x01,			/* bFormatType */
    0x01, 0x00, 0x00, 0x00,		/* bmFormats (note this is a bitmap) */
#ifdef USE_FOUR_CODEC
    0x08,           /* bNrChannels */
#else
#ifdef USE_THREE_CODEC
    0x06,           /* bNrChannels */
#else
#ifdef USE_TWO_CODEC
    0x04,           /* bNrChannels */
#else
    0x02,           /* bNrChannels */
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
    0x03, 0x00, 0x00, 0x00,    	/* bmChannelConfig */
    0x1A,			/* iChannelNames=OUTLF1, OUTRH1, OUTLF2, OUTRH2, OUTLF3, OUTRH3, OUTLF4, OUTRH4*/

    /* Class-Specific Format Type 1 Audio Descriptor */
    0x06,           /* bLength, in bytes */
    CS_INTERFACE,   /* bDescriptorType=CS_INTERFACE */
    FORMAT_TYPE,    /* bDescriptorSubtype=FORMAT_TYPE */
    FORMAT_TYPE_I,  /* bFormatType = TYPE 1 Data Format */
#ifdef SAMP_24BIT
    //0x03,           /* bSubframeSize in bytes=3 bytes */
    0x04,           /* bSubframeSize in bytes=4 bytes */
    0x18,           /* bBitResolution=24 bit depth */
#else
    0x02,           /* bSubframeSize in bytes=2 bytes */
    0x10,           /* bBitResolution=16 bit depth */
#endif //SAMP_24BIT

    /* Standard Isochronous Endpoint Descriptor */
    0x07,           /* bLength, in bytes */
    ENDPOINT,       /* bDescriptorType=ENDPOINT (from USBAD, Table 4-20) */
    EP_ADDR_PLAY,    /* bEndpointAddress=EP1-IN Direction, Address 1 */
    0x05,           /* bmAttributes=Usage Type=DATA Endpoint, Sync Type=Asynchronous, Transfer Type=Isochronous */
    LEB(EP_PB_MAXP), /* wMaxPacketSize=34 Bytes */
    0x01,           /* bInterval is number of milliseconds between IN polling */

    /* Class-Specific AS Isochronous Endpoint Audio Class Descriptor */
    0x08,           /* bLength, in bytes */
    CS_ENDPOINT,     /* bDescriptorType=CS_ENDPOINT */
    AS_GENERAL,     /* bDescriptorSubtype */
    0x00,           /* bmAttributes=no controls, full packet not necessary */
    0x00,           /* bmControls (Bitmap: Pitch control, over/underun etc) */
    0x00,           /* bLockDelayUnits, asynchronous audio function, must be 0 */
    LEB(0x0000),    /* wLockDelay, must be 0 */

#ifdef FEEDBACKEP
    /* Feedback EP */
    0x07,           				/* 0  bLength: 7 */
    ENDPOINT,           			/* 1  bDescriptorType: ENDPOINT */
    EP_ADDR_FBCK,            		/* 2  bEndpointAddress (D7: 0:out, 1:in) */
    17,              				/* 3  bmAttributes (bitmap)  */
    LEB(0x0004),            		/* 4  wMaxPacketSize */
    7,              				/* 6  bInterval */
#endif // FEEDBACKEP
    /* Audio Playback Interface */
#endif // (defined(PLAY_ONLY)||defined(PLAY_RECORD))
};

const Uint16 stringLanId[STRING_LAN_ID_SZ] = 
{
    0x0000, 
    0x0304, 
    0x0409, 
    0x0000
};

char strDesc[STRING_DESCR_SZ][20] = 
{
    // string 0 English-USA
    {0x0304, 0x0304, 0x0409},
    // string 1 "Texas Instruments"
    {0x0324, 0x0324, 'T', 'e', 'x', 'a', 's', ' ', 'I', 'n', 's', 
    't', 'r', 'u', 'm', 'e', 'n', 't', 's'},
    // string 2 "TI C55x USB Audio"
    {0x0324, 0x0324, 'T', 'I', ' ', 'C', '5', '5', 'x', ' ',  
    'U', 'S', 'B', ' ', 'A', 'u', 'd', 'i', 'o'},
    // string 3 "320001"
    {0x030E, 0x030E, '3', '2', '0', '0', '0', '1'},
    // string 4 "48KHZ"
    {0x030C, 0x030C, '4', '8', 'K', 'H', 'Z',},
    // string 5 "CLKSEL"
    {0x030E, 0x030E, 'C', 'L', 'K', 'S', 'E', 'L'},
    // string 6 "MIC"
    {0x0308, 0x0308, 'M', 'I', 'C'},
    // string 7 "USBIN"
    {0x030C, 0x030C, 'U', 'S', 'B', 'I', 'N'},
    // string 8 "RECORD"
    {0x030E, 0x030E, 'R', 'E', 'C', 'O', 'R', 'D'},
    // string 9 "96KHZ"
    ///{0x030C, 0x030C, '9', '6', 'K', 'H', 'Z',},
    {0x030C, 0x030C, 'M', 'U', 'L', 'T', 'I',},
    // string 10 "SPK"
    {0x0308, 0x0308, 'S', 'P', 'K'},
    // string 11 "USBOUT"
    {0x030E, 0x030E, 'U', 'S', 'B', 'O', 'U', 'T'},
    // string 12 "PLAY"
    {0x030A, 0x030A, 'P', 'L', 'A', 'Y'},
    // string 13 "192KHZ"
    {0x030E, 0x030E, '1', '9', '2', 'K', 'H', 'Z'},
    // string 14 "INCTL"
    {0x030E, 0x030E, 'I', 'N', '_', 'C', 'T', 'L'},
    // string 15 "INSTR"
    {0x030E, 0x030E, 'I', 'N', '_', 'S', 'T', 'R'},
    // string 16 "OUTCTL"
    {0x030E, 0x030E, 'O', 'U', 'T', 'C', 'T', 'L'},
    // string 17 "OUTSTR"
    {0x030E, 0x030E, 'O', 'U', 'T', 'S', 'T', 'R'},
    // string 18 "INLFT1"
    {0x030E, 0x030E, 'I', 'N', 'L', 'F', 'T', '1'},
    // string 19 "INRHT1"
    {0x030E, 0x030E, 'I', 'N', 'R', 'H', 'T', '1'},
    // string 20 "INLFT2"
    {0x030E, 0x030E, 'I', 'N', 'L', 'F', 'T', '2'},
    // string 21 "INRHT2"
    {0x030E, 0x030E, 'I', 'N', 'R', 'H', 'T', '2'},
    // string 22 "INLFT3"
    {0x030E, 0x030E, 'I', 'N', 'L', 'F', 'T', '3'},
    // string 23 "INRHT3"
    {0x030E, 0x030E, 'I', 'N', 'R', 'H', 'T', '3'},
    // string 24 "INLFT4"
    {0x030E, 0x030E, 'I', 'N', 'L', 'F', 'T', '4'},
    // string 25 "INRHT4"
    {0x030E, 0x030E, 'I', 'N', 'R', 'H', 'T', '4'},
    // string 26 "OUTLF1"
    {0x030E, 0x030E, 'O', 'U', 'T', 'L', 'F', '1'},
    // string 27 "OUTRT1"
    {0x030E, 0x030E, 'O', 'U', 'T', 'R', 'T', '1'},
    // string 28 "OUTLF2"
    {0x030E, 0x030E, 'O', 'U', 'T', 'L', 'F', '2'},
    // string 29 "OUTRT2"
    {0x030E, 0x030E, 'O', 'U', 'T', 'R', 'T', '2'},
    // string 30 "OUTLF3"
    {0x030E, 0x030E, 'O', 'U', 'T', 'L', 'F', '3'},
    // string 31 "OUTRT3"
    {0x030E, 0x030E, 'O', 'U', 'T', 'R', 'T', '3'},
    // string 32 "OUTLF3"
    {0x030E, 0x030E, 'O', 'U', 'T', 'L', 'F', '4'},
    // string 33 "OUTRT3"
    {0x030E, 0x030E, 'O', 'U', 'T', 'R', 'T', '4'},
};

const char *string_descriptor[STRING_DESCR_SZ] =
{
    &strDesc[0][0],     /* Language String English-USA */
    &strDesc[1][0],     /* iManufacturer: "Texas Instruments" */
    &strDesc[2][0],     /* iProductName: "TI C55x USB Audio" */
    &strDesc[3][0],     /* iSerial No: "320001" */
    &strDesc[4][0],     /* Clock ID 1: "48KHZ" */
    &strDesc[5][0],     /* Clock ID 2: "CLKSEL" */
    &strDesc[6][0],     /* Default: "MIC" */
    &strDesc[7][0],     /* Default: "USBIN" */
    &strDesc[8][0],     /* Default: "RECORD" */
    &strDesc[9][0],     /* Default: "96KHZ" */
    &strDesc[10][0],     /* Default: "SPK" */
    &strDesc[11][0],     /* Default: "USBOUT" */
    &strDesc[12][0],     /* Default: "PLAY" */
    &strDesc[13][0],     /* Default: "192KHZ" */
    &strDesc[14][0],     /* Default: "INCTL" */
    &strDesc[15][0],     /* Default: "INSTR" */
    &strDesc[16][0],     /* Default: "OUTCTL" */
    &strDesc[17][0],     /* Default: "OUTSTR" */
    &strDesc[18][0],     /* Default: "INLFT1" */
    &strDesc[19][0],     /* Default: "INRHT1" */
    &strDesc[20][0],     /* Default: "INLFT2" */
    &strDesc[21][0],     /* Default: "INRHT2" */
    &strDesc[22][0],     /* Default: "INLFT3" */
    &strDesc[23][0],     /* Default: "INRHT3" */
    &strDesc[24][0],     /* Default: "INLFT4" */
    &strDesc[25][0],     /* Default: "INRHT4" */
    &strDesc[26][0],     /* Default: "OUTLF1" */
    &strDesc[27][0],     /* Default: "OUTRT1" */
    &strDesc[28][0],     /* Default: "OUTLF2" */
    &strDesc[29][0],     /* Default: "OUTRT2" */
    &strDesc[30][0],     /* Default: "OUTLF3" */
    &strDesc[31][0],     /* Default: "OUTRT3" */
    &strDesc[32][0],     /* Default: "OUTLF4" */
    &strDesc[33][0],     /* Default: "OUTRT4" */
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

