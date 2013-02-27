/*
 * $$$MODULE_NAME app_usbac_descs.h
 *
 * $$$MODULE_DESC app_usbac_descs.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _APP_USBAC_DESCS_H_
#define _APP_USBAC_DESCS_H_

#include <std.h>
#include "psp_i2s.h"
#include "app_usb.h"

/* Enable adding second playback sample rate */
//#define ENABLE_PLAYBACK_TWO_SAMPLE_RATES

/* Descriptor sizes */
#define DEVICE_DESCR_SZ             ( 9 )       /* device descriptor */
#define DEVICE_DESCR_B_SZ           ( 18 )      /* device descriptor B */
#define DEVICE_QUALIFIER_DESCR_SZ   ( 10 )       /* device qualifier */
#define STRING_LAN_ID_SZ            ( 4 )       /* string language Id */
#define STRING_DESCR_SZ             ( 34 )       /* string descriptor */
#define AC_HID_REPORT_DESCR_SZ      ( 53 )      /* HID report descriptor */

/* Configuration descriptor size */
#define CONFIG_DESCR_BASE_SZ        ( 221 )

#define CONFIG_DESCR_INC_SZ_PB2SR   ( 0 )

#define CONFIG_DESCR_INC_SZ_SR      ( 1 )       /* incremental size for stereo record */

//#define CONFIG_DESCR_SZ             ( CONFIG_DESCR_BASE_SZ + CONFIG_DESCR_INC_SZ_PB2SR + CONFIG_DESCR_INC_SZ_SR) 
#ifdef RECORD_ONLY
#ifdef USE_FOUR_CODEC
#define CONFIG_DESCR_SZ             ( 177 )	//(35+16+71+55)
#else
#ifdef USE_THREE_CODEC
#define CONFIG_DESCR_SZ             ( 169 )	//(35+16+63+55)
#else
#ifdef USE_TWO_CODEC
#define CONFIG_DESCR_SZ             ( 161 )	//(35+16+55+55)
#else
#define CONFIG_DESCR_SZ             ( 153 )	//(35+16+47+55)
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#endif //RECORD_ONLY

#ifdef PLAY_ONLY
#ifdef USE_FOUR_CODEC
#ifdef FEEDBACKEP
#define CONFIG_DESCR_SZ             ( 184 )	//(35+16+71+62)
#else
#define CONFIG_DESCR_SZ             ( 177 )	//(35+16+71+55)
#endif //FEEDBACKEP
#else
#ifdef USE_THREE_CODEC
#ifdef FEEDBACKEP
#define CONFIG_DESCR_SZ             ( 176 )	//(35+16+63+62)
#else
#define CONFIG_DESCR_SZ             ( 169 )	//(35+16+63+55)
#endif //FEEDBACKEP
#else
#ifdef USE_TWO_CODEC
#ifdef FEEDBACKEP
#define CONFIG_DESCR_SZ             ( 168 )	//(35+16+55+62)
#else
#define CONFIG_DESCR_SZ             ( 161 )	//(35+16+55+55)
#endif //FEEDBACKEP
#else
#ifdef FEEDBACKEP
#define CONFIG_DESCR_SZ             ( 160 )	//(35+16+47+62)
#else
#define CONFIG_DESCR_SZ             ( 153 )	//(35+16+47+55)
#endif //FEEDBACKEP
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#endif //PLAY_ONLY

#ifdef PLAY_RECORD
#ifdef USE_FOUR_CODEC
#ifdef FEEDBACKEP
#define CONFIG_DESCR_SZ             ( 177+133 ) //(35+16+71+55)+(71+62)
#else
#define CONFIG_DESCR_SZ             ( 177+126 ) //(35+16+71+55)+(71+55)
#endif //FEEDBACKEP
#else
#ifdef USE_THREE_CODEC
#ifdef FEEDBACKEP
#define CONFIG_DESCR_SZ             ( 169+125 ) //(35+16+63+55)+(63+62)
#else
#define CONFIG_DESCR_SZ             ( 169+118 ) //(35+16+63+55)+(63+55)
#endif //FEEDBACKEP
#else
#ifdef USE_TWO_CODEC
#ifdef FEEDBACKEP
#define CONFIG_DESCR_SZ             ( 161+117 ) //(35+16+55+55)+(55+62)
#else
#define CONFIG_DESCR_SZ             ( 161+110 ) //(35+16+55+55)+(55+55)
#endif //FEEDBACKEP
#else
#ifdef FEEDBACKEP
#define CONFIG_DESCR_SZ             ( 153+109 ) //(35+16+47+55)+(47+62)
#else
#define CONFIG_DESCR_SZ             ( 153+102 ) //(35+16+47+55)+(47+55)
#endif //FEEDBACKEP
#endif //USE_TWO_CODEC
#endif //USE_THREE_CODEC
#endif //USE_FOUR_CODEC
#endif //PLAY_RECORD

/* Vendor ID (idVendor) */
#define DEV_VID ( 0x0451 )
/* Product ID (idProduct) */
#define DEV_PID ( 0x9010 )
/* bcdDevice USB Device Firmware version number example: 0x1234 = version 12.34 */
#define DEV_BCD ( 0x0001 )

/* constants for total number of interface count */
#define IF_AUDIO_CTRL   (1) /* audio control interface */
#define IF_AUDIO_REC    (1) /* audio record interface */
#define IF_AUDIO_PLAY   (1) /* audio playback interface */
#define IF_HID          (1) /* HID interface */
//#define IF_COUNT        (IF_AUDIO_CTRL + IF_AUDIO_REC + IF_AUDIO_PLAY + IF_HID)
#ifdef RECORD_ONLY
#define IF_COUNT        (IF_AUDIO_CTRL + IF_AUDIO_REC)
#endif
#ifdef PLAY_ONLY
#define IF_COUNT        (IF_AUDIO_CTRL + IF_AUDIO_PLAY)
#endif
#ifdef PLAY_RECORD
#define IF_COUNT        (IF_AUDIO_CTRL + IF_AUDIO_REC + IF_AUDIO_PLAY)
#endif

/* constant for audio streaming interface count, NOTE: audio control interface is not included */
#ifdef RECORD_ONLY
#define IF_STRM_CNT     (IF_AUDIO_REC)
#endif
#ifdef PLAY_ONLY
#define IF_STRM_CNT     (IF_AUDIO_PLAY)
#endif
#ifdef PLAY_RECORD
#define IF_STRM_CNT     (IF_AUDIO_REC + IF_AUDIO_PLAY)
#endif

/* USB audio terminal ID definitions, MUST be one based and unique */
#define T_IT_STRM_PLAY  (0x01)  /* Input Terminal ID Streaming Playback */
#define T_FU_PLAY       (0x02)  /* Feature Unit Terminal ID Plyaback */
#define T_OT_SPKR       (0x03)  /* Output Terminal ID Playback Speaker */

#define T_IT_MIC_REC    (0x04)  /* Input Terminal ID Microphone Record */
#define T_FU_REC        (0x05)  /* Feature Unit Terminal ID Record */
#define T_OT_STRM_REC   (0x06)  /* Output Terminal ID Streaming Record */

/* USB audio clock ID definitions, MUST be one based and unique */
#define T_IT_CLK_SEL    (0x40)  /* Input Terminal ID Clock Selector */
#define T_IT_CLK_SRC    (0x41)  /* Input Terminal ID Clock Source*/
#define T_IT_CLK_SRC_FREQ    (0x0000BB80)  /* Input Terminal ID Clock Source Frequency 48Khz*/

#define REC_SAMPLE_RATE (RATE_48_KHZ)    /* audio record sample rate */
//#define REC_SAMPLE_RATE (RATE_16_KHZ)    /* audio record sample rate */

#ifdef ENABLE_PLAYBACK_TWO_SAMPLE_RATES
#define PLAY_SAMPLE_RATE_1 (RATE_48_KHZ)   /* audio playback sample rate */
#define PLAY_SAMPLE_RATE_2 (RATE_16_KHZ)   /* audio playback sample rate */
#else
#define PLAY_SAMPLE_RATE (RATE_48_KHZ)   /* audio playback sample rate */
//#define PLAY_SAMPLE_RATE (RATE_16_KHZ)   /* audio playback sample rate */
#endif

/* Full-speed endpoint addresses (direction, number) */
#define EP_ADDR_REC     (CSL_USB_IN_TRANSFER  | EP_NUM_REC)
#define EP_ADDR_PLAY    (CSL_USB_OUT_TRANSFER | EP_NUM_PLAY)
#ifdef FEEDBACKEP
#define EP_ADDR_FBCK    (CSL_USB_IN_TRANSFER | EP_NUM_FBCK)
#endif //FEEDBACKEP
#define EP_ADDR_HID     (CSL_USB_IN_TRANSFER  | EP_NUM_HID)

// USB 2.0 Constants
#define USE_CLASS                   0x00
#define AUDIO                       0x01
#define COMMUNICATIONS              0x02
#define HID                         0x03

#define DEVICE                      0x01
#define CONFIGURATION               0x02
#define STRING                      0x03
#define INTERFACE                   0x04
#define ENDPOINT                    0x05
#define DEVICE_QUALIFIER            0x06
#define INTERFACE_ASSOCIATION		0x0B

// USB Audio Class Constants
#define IP_VERSION_02_00	0x20

#define AUDIOINTERFACECLASS	0x01

#define AUDIOCONTROL		0x01
#define AUDIOSTREAMING		0x02
#define MIDISTREAMING		0x03

#define DESKTOP_SPEAKER		0x01
#define HOME_THEATER		0x02
#define MICROPHONE			0x03
#define HEADSET				0x04

#define CS_DEVICE			0x21
#define CS_CONFIGURATION	0x22
#define CS_STRING			0x23
#define CS_INTERFACE		0x24
#define CS_ENDPOINT			0x25

#define HEADER					0x01
#define INPUT_TERMINAL			0x02
#define OUTPUT_TERMINAL			0x03
#define MIXER_UNIT				0x04
#define SELECTOR_UNIT			0x05
#define FEATURE_UNIT			0x06
#define EFFECT_UNIT				0x07
#define PROCESSING_UNIT			0x08
#define EXTENSION_UNIT			0x09
#define CLOCK_SOURCE			0x0A
#define CLOCK_SELECTOR			0x0B
#define CLOCK_MULTIPLIER		0x0C
#define SAMPLE_RATE_CONVERTER	0x0D

#define AS_GENERAL			0x01
#define FORMAT_TYPE			0x02
#define ENCODER				0x03

#define FORMAT_TYPE_I		0x01

extern const Uint16 deviceDescriptor[DEVICE_DESCR_SZ];
extern const Uint16 deviceDescriptorB[DEVICE_DESCR_B_SZ];
extern const Uint16 deviceQualifierDescr[DEVICE_QUALIFIER_DESCR_SZ];
extern const Uint16 configDescriptor[CONFIG_DESCR_SZ];
extern const char *string_descriptor[STRING_DESCR_SZ];
extern const Uint16 stringLanId[STRING_LAN_ID_SZ];
extern const Uint16 acHidReportDescriptor[AC_HID_REPORT_DESCR_SZ];

#endif /* _APP_USBAC_DESCS_H_ */
