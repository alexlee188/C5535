/*
 * $$$MODULE_NAME app_globals.h
 *
 * $$$MODULE_DESC app_globals.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef _APP_GLOBALS_H_
#define _APP_GLOBALS_H_

#define START_REC_WAIT_FRAMES ( 50 )    /* no. frames to wait before initiating record */

/* Get the most significant byte of a word */
#define MSB(s)          ((Uint8)((0xFF00 & ((Uint16)(s))) >> 8))
/* Get the least significant byte of a word */
#define LSB(s)          ((Uint8)(0x00FF & ((Uint16)(s))))

#endif /* _APP_GLOBALS_H_ */
