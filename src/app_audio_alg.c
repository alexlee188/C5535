/*
 * $$$MODULE_NAME app_audio_alg.c
 *
 * $$$MODULE_DESC app_audio_alg.c
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
#include <string.h>
#include "i2s_sample.h"
#include "app_usb.h"
#include "app_usbac.h"
#include "app_audio_alg.h"
#include "VC5505_CSL_BIOS_cfg.h"
#include "dbg_sdram.h"
#include "psp_i2s.h"
#include "sample_rate.h"
#include "codec_config.h"

extern Int16 *sdramPtr0;
Uint32 sdramIndex = 0;

/* Perform Record (Rx) audio algorithm processing */
Uint32 recAudioTaskCount = 0;
Uint32 recAudioCopyCount = 0;
void RecAudioAlgTsk(void)
{
    Uint16 *ptrRxLeft;
#ifdef USE_TWO_CODEC
    Uint16 *ptrRxLeft2;
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
    Uint16 *ptrRxLeft2;
    Uint16 *ptrRxLeft3;
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
    Uint16 *ptrRxLeft2;
    Uint16 *ptrRxLeft3;
    Uint16 *ptrRxLeft4;
#endif //USE_FOUR_CODEC
    Uint16 i;
    Uint16 *ptrRxRight;
#ifdef USE_TWO_CODEC
    Uint16 *ptrRxRight2;
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
    Uint16 *ptrRxRight2;
    Uint16 *ptrRxRight3;
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
    Uint16 *ptrRxRight2;
    Uint16 *ptrRxRight3;
    Uint16 *ptrRxRight4;
#endif //USE_FOUR_CODEC

    while (1)
    {
        SEM_pend(&SEM_PingPongRxLeftComplete, SYS_FOREVER);
#ifdef USE_TWO_CODEC
        SEM_pend(&SEM_PingPongRxLeftComplete2, SYS_FOREVER);
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
        SEM_pend(&SEM_PingPongRxLeftComplete2, SYS_FOREVER);
        SEM_pend(&SEM_PingPongRxLeftComplete3, SYS_FOREVER);
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
        SEM_pend(&SEM_PingPongRxLeftComplete2, SYS_FOREVER);
        SEM_pend(&SEM_PingPongRxLeftComplete3, SYS_FOREVER);
        SEM_pend(&SEM_PingPongRxLeftComplete4, SYS_FOREVER);
#endif //USE_FOUR_CODEC

        SEM_pend(&SEM_PingPongRxRightComplete, SYS_FOREVER);
#ifdef USE_TWO_CODEC
        SEM_pend(&SEM_PingPongRxRightComplete2, SYS_FOREVER);
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
        SEM_pend(&SEM_PingPongRxRightComplete2, SYS_FOREVER);
        SEM_pend(&SEM_PingPongRxRightComplete3, SYS_FOREVER);
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
        SEM_pend(&SEM_PingPongRxRightComplete2, SYS_FOREVER);
        SEM_pend(&SEM_PingPongRxRightComplete3, SYS_FOREVER);
        SEM_pend(&SEM_PingPongRxRightComplete4, SYS_FOREVER);
#endif //USE_FOUR_CODEC

		recAudioTaskCount++;
        /* Get pointer to ping/pong buffer */
        ptrRxLeft = &ping_pong_i2sRxLeftBuf[0];
        if (left_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxLeft += I2S_RXBUFF_SZ;
        }

        /* Get pointer to right ping/pong buffer */
        ptrRxRight = &ping_pong_i2sRxRightBuf[0];
        if (right_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxRight+= I2S_RXBUFF_SZ;
        }

#ifdef USE_TWO_CODEC
        /* Get pointer to ping/pong buffer */
        ptrRxLeft2 = &ping_pong_i2sRxLeftBuf2[0];
        if (left_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxLeft2 += I2S_RXBUFF_SZ;
        }

        /* Get pointer to right ping/pong buffer */
        ptrRxRight2 = &ping_pong_i2sRxRightBuf2[0];
        if (right_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxRight2+= I2S_RXBUFF_SZ;
        }
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
        /* Get pointer to ping/pong buffer */
        ptrRxLeft2 = &ping_pong_i2sRxLeftBuf2[0];
        if (left_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxLeft2 += I2S_RXBUFF_SZ;
        }

        /* Get pointer to right ping/pong buffer */
        ptrRxRight2 = &ping_pong_i2sRxRightBuf2[0];
        if (right_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxRight2+= I2S_RXBUFF_SZ;
        }

        /* Get pointer to ping/pong buffer */
        ptrRxLeft3 = &ping_pong_i2sRxLeftBuf3[0];
        if (left_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxLeft3 += I2S_RXBUFF_SZ;
        }

        /* Get pointer to right ping/pong buffer */
        ptrRxRight3 = &ping_pong_i2sRxRightBuf3[0];
        if (right_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxRight3+= I2S_RXBUFF_SZ;
        }
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
        /* Get pointer to ping/pong buffer */
        ptrRxLeft2 = &ping_pong_i2sRxLeftBuf2[0];
        if (left_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxLeft2 += I2S_RXBUFF_SZ;
        }

        /* Get pointer to right ping/pong buffer */
        ptrRxRight2 = &ping_pong_i2sRxRightBuf2[0];
        if (right_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxRight2+= I2S_RXBUFF_SZ;
        }

        /* Get pointer to ping/pong buffer */
        ptrRxLeft3 = &ping_pong_i2sRxLeftBuf3[0];
        if (left_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxLeft3 += I2S_RXBUFF_SZ;
        }

        /* Get pointer to right ping/pong buffer */
        ptrRxRight3 = &ping_pong_i2sRxRightBuf3[0];
        if (right_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxRight3+= I2S_RXBUFF_SZ;
        }

        /* Get pointer to ping/pong buffer */
        ptrRxLeft4 = &ping_pong_i2sRxLeftBuf4[0];
        if (left_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxLeft4 += I2S_RXBUFF_SZ;
        }

        /* Get pointer to right ping/pong buffer */
        ptrRxRight4 = &ping_pong_i2sRxRightBuf4[0];
        if (right_rx_buf_sel == 0x1) /* check ping or pong buffer */
        {
            /* this buffer has data to be processed */
            ptrRxRight4+= I2S_RXBUFF_SZ;
        }
#endif //USE_FOUR_CODEC
        left_rx_buf_sel ^= 0x1; /* update ping/pong */
        right_rx_buf_sel ^= 0x1; /* update ping/pong */

        /* if the record is started, store data in circular buffer */
        if ((usb_rec_mode)||(usb_rec_start))
        {
            for (i = 0; i < RXBUFF_SZ_ADCSAMPS; i++)
            {
#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft;
				codec_input_buffer_input_index++;
				ptrRxLeft++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft;
				codec_input_buffer_input_index++;
				ptrRxLeft++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft;
            	codec_input_buffer_input_index++;
            	ptrRxLeft += 2;
#endif //SAMP_24BIT

#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight;
				codec_input_buffer_input_index++;
				ptrRxRight++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight;
				codec_input_buffer_input_index++;
				ptrRxRight++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight;
				codec_input_buffer_input_index++;
            	ptrRxRight += 2;
#endif //SAMP_24BIT


#ifdef USE_TWO_CODEC
#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft2;
				codec_input_buffer_input_index++;
				ptrRxLeft2++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft2;
				codec_input_buffer_input_index++;
				ptrRxLeft2++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft2;
				codec_input_buffer_input_index++;
            	ptrRxLeft2 += 2;
#endif //SAMP_24BIT

#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight2;
				codec_input_buffer_input_index++;
				ptrRxRight2++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight2;
				ptrRxRight2++;
				codec_input_buffer_input_index++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight2;
				codec_input_buffer_input_index++;
            	ptrRxRight2 += 2;
#endif //SAMP_24BIT
#endif //USE_TWO_CODEC

#ifdef USE_THREE_CODEC
#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft2;
				codec_input_buffer_input_index++;
				ptrRxLeft2++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft2;
				codec_input_buffer_input_index++;
				ptrRxLeft2++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft2;
				codec_input_buffer_input_index++;
            	ptrRxLeft2 += 2;
#endif //SAMP_24BIT

#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight2;
				codec_input_buffer_input_index++;
				ptrRxRight2++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight2;
				codec_input_buffer_input_index++;
				ptrRxRight2++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight2;
				codec_input_buffer_input_index++;
            	ptrRxRight2 += 2;
#endif //SAMP_24BIT

#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft3;
				codec_input_buffer_input_index++;
				ptrRxLeft3++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft3;
				codec_input_buffer_input_index++;
				ptrRxLeft3++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft3;
				codec_input_buffer_input_index++;
            	ptrRxLeft3 += 2;
#endif //SAMP_24BIT

#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight3;
				codec_input_buffer_input_index++;
				ptrRxRight3++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight3;
				codec_input_buffer_input_index++;
				ptrRxRight3++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight3;
				codec_input_buffer_input_index++;
            	ptrRxRight3 += 2;
#endif //SAMP_24BIT
#endif //USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft2;
				codec_input_buffer_input_index++;
				ptrRxLeft2++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft2;
				codec_input_buffer_input_index++;
				ptrRxLeft2++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft2;
				codec_input_buffer_input_index++;
            	ptrRxLeft2 += 2;
#endif //SAMP_24BIT

#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight2;
				codec_input_buffer_input_index++;
				ptrRxRight2++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight2;
				codec_input_buffer_input_index++;
				ptrRxRight2++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight2;
				codec_input_buffer_input_index++;
            	ptrRxRight2 += 2;
#endif //SAMP_24BIT

#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft3;
				codec_input_buffer_input_index++;
				ptrRxLeft3++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft3;
				codec_input_buffer_input_index++;
				ptrRxLeft3++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft3;
				codec_input_buffer_input_index++;
            	ptrRxLeft3 += 2;
#endif //SAMP_24BIT

#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight3;
				codec_input_buffer_input_index++;
				ptrRxRight3++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight3;
				codec_input_buffer_input_index++;
				ptrRxRight3++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight3;
				codec_input_buffer_input_index++;
            	ptrRxRight3 += 2;
#endif //SAMP_24BIT

#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft4;
				codec_input_buffer_input_index++;
				ptrRxLeft4++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft4;
				codec_input_buffer_input_index++;
				ptrRxLeft4++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxLeft4;
				codec_input_buffer_input_index++;
            	ptrRxLeft4 += 2;
#endif //SAMP_24BIT

#ifdef SAMP_24BIT
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight4;
				codec_input_buffer_input_index++;
				ptrRxRight4++;
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight4;
				codec_input_buffer_input_index++;
				ptrRxRight4++;
#else
				codec_input_buffer[codec_input_buffer_input_index] = *ptrRxRight4;
				codec_input_buffer_input_index++;
            	ptrRxRight4 += 2;
#endif //SAMP_24BIT
#endif //USE_FOUR_CODEC

                if (codec_input_buffer_input_index >= CODEC_INPUT_BUFFER_SIZE)
                {
                    codec_input_buffer_input_index = 0;
                }
            }

			// increase codec input buffer sample counter (mono)
			codec_input_buffer_sample += RXBUFF_SZ_ADCSAMPS;
			recAudioCopyCount++;
			// if the codec_input_buffer is half full, start recording
			if ((usb_rec_start)&&(!usb_rec_mode)&&(codec_input_buffer_sample>=(RXBUFF_SZ_ADCSAMPS*CODEC_INPUT_SZ_MSEC/2)))
			{
				usb_rec_start = FALSE;
				usb_rec_mode = TRUE;
			}
        }
    }
}

/* Perform Playback (Tx) audio algorithm processing */
Uint32 playAudioTaskCount = 0;
Uint32 playAudioCopyCount = 0;
Uint32 playMemsetCount = 0;
void PbAudioAlgTsk(void)
{
    Int16 *pbOutBufLeft;
    Int16 *pbOutBufRight;
#ifdef USE_TWO_CODEC
    Int16 *pbOutBufLeft2;
    Int16 *pbOutBufRight2;
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
    Int16 *pbOutBufLeft2;
    Int16 *pbOutBufRight2;
    Int16 *pbOutBufLeft3;
    Int16 *pbOutBufRight3;
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
    Int16 *pbOutBufLeft2;
    Int16 *pbOutBufRight2;
    Int16 *pbOutBufLeft3;
    Int16 *pbOutBufRight3;
    Int16 *pbOutBufLeft4;
    Int16 *pbOutBufRight4;
#endif //USE_FOUR_CODEC
    volatile Int16 i, loopCount;
    CodecCfgMsgObj codecCfgMsg;
    Bool mute_play_status = TRUE;
    Int16 temp1;
	extern int bufferIn[256];
	extern int bufferInIdx;

    while (1)
    {
        SEM_pend(&SEM_PingPongTxLeftComplete, SYS_FOREVER);
#ifdef USE_TWO_CODEC
        SEM_pend(&SEM_PingPongTxLeftComplete2, SYS_FOREVER);
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
        SEM_pend(&SEM_PingPongTxLeftComplete2, SYS_FOREVER);
        SEM_pend(&SEM_PingPongTxLeftComplete3, SYS_FOREVER);
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
        SEM_pend(&SEM_PingPongTxLeftComplete2, SYS_FOREVER);
        SEM_pend(&SEM_PingPongTxLeftComplete3, SYS_FOREVER);
        SEM_pend(&SEM_PingPongTxLeftComplete4, SYS_FOREVER);
#endif //USE_FOUR_CODEC
        SEM_pend(&SEM_PingPongTxRightComplete, SYS_FOREVER);
#ifdef USE_TWO_CODEC
        SEM_pend(&SEM_PingPongTxRightComplete2, SYS_FOREVER);
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
        //SEM_pend(&SEM_PingPongTxRightComplete2, SYS_FOREVER);
        //SEM_pend(&SEM_PingPongTxRightComplete3, SYS_FOREVER);
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
        SEM_pend(&SEM_PingPongTxRightComplete2, SYS_FOREVER);
        SEM_pend(&SEM_PingPongTxRightComplete3, SYS_FOREVER);
        SEM_pend(&SEM_PingPongTxRightComplete4, SYS_FOREVER);
#endif //USE_FOUR_CODEC

        if ((playAudioTaskCount % 100) == 0){
        	EZDSP5535_LED_toggle(0);
        }

		playAudioTaskCount++;
        /* Select output buffer */
        pbOutBufLeft = ping_pong_i2sTxLeftBuf + left_tx_buf_sel*i2sTxBuffSz;
#ifdef USE_TWO_CODEC
        pbOutBufLeft2 = ping_pong_i2sTxLeftBuf2 + left_tx_buf_sel*i2sTxBuffSz;
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
        pbOutBufLeft2 = ping_pong_i2sTxLeftBuf2 + left_tx_buf_sel*i2sTxBuffSz;
        pbOutBufLeft3 = ping_pong_i2sTxLeftBuf3 + left_tx_buf_sel*i2sTxBuffSz;
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
        pbOutBufLeft2 = ping_pong_i2sTxLeftBuf2 + left_tx_buf_sel*i2sTxBuffSz;
        pbOutBufLeft3 = ping_pong_i2sTxLeftBuf3 + left_tx_buf_sel*i2sTxBuffSz;
        pbOutBufLeft4 = ping_pong_i2sTxLeftBuf4 + left_tx_buf_sel*i2sTxBuffSz;
#endif //USE_FOUR_CODEC
        left_tx_buf_sel ^= 0x1;
        pbOutBufRight = ping_pong_i2sTxRightBuf + right_tx_buf_sel*i2sTxBuffSz;
#ifdef USE_TWO_CODEC
        pbOutBufRight2 = ping_pong_i2sTxRightBuf2 + right_tx_buf_sel*i2sTxBuffSz;
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
        pbOutBufRight2 = ping_pong_i2sTxRightBuf2 + right_tx_buf_sel*i2sTxBuffSz;
        pbOutBufRight3 = ping_pong_i2sTxRightBuf3 + right_tx_buf_sel*i2sTxBuffSz;
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
        pbOutBufRight2 = ping_pong_i2sTxRightBuf2 + right_tx_buf_sel*i2sTxBuffSz;
        pbOutBufRight3 = ping_pong_i2sTxRightBuf3 + right_tx_buf_sel*i2sTxBuffSz;
        pbOutBufRight4 = ping_pong_i2sTxRightBuf4 + right_tx_buf_sel*i2sTxBuffSz;
#endif //USE_FOUR_CODEC
        right_tx_buf_sel ^= 0x1;

		if (usb_play_mode)
        {
        	// do we have enough samples for the DMA buffer? 
			if (codec_output_buffer_sample>=MAX_TXBUFF_SZ_DACSAMPS)
			{
				if (mute_play_status){
					codecCfgMsg.wMsg = CODEC_CFG_MSG_UNMUTE;
					MBX_post(&MBX_codecConfig, &codecCfgMsg, SYS_FOREVER);
					mute_play_status = FALSE;
				}
				playAudioCopyCount++;
	            /* copy one-millisecond audio data from circular buffer codec_output_buffer to DMA ping pong buffer */
				for (i=0; i<MAX_TXBUFF_SZ_DACSAMPS; i++)
				{
#ifdef SAMP_24BIT
					// copy left channel sample
					temp1 = *pbOutBufLeft = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft++;
					*pbOutBufLeft = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft++;
					if (bufferInIdx<256){
						bufferIn[bufferInIdx] = temp1; // Is it the MSB 16 bits?
						bufferInIdx++;
						// if the bufferIn is filled, then send a semaphore to the SpectrumDisplayTask
						if (bufferInIdx==256)
						{
							// send semaphore
							SEM_post(&SEM_PingPongRxLeftComplete4);
						}
					}
					// copy right channel sample
					*pbOutBufRight = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight++;
					*pbOutBufRight = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight++;
#ifdef USE_TWO_CODEC
					// copy left channel sample
					*pbOutBufLeft2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft2++;
					*pbOutBufLeft2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft2++;
					// copy right channel sample
					*pbOutBufRight2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight2++;
					*pbOutBufRight2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight2++;
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
					// copy left channel sample
					*pbOutBufLeft2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft2++;
					*pbOutBufLeft2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft2++;
					// copy right channel sample
					*pbOutBufRight2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight2++;
					*pbOutBufRight2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight2++;

					// copy left channel sample
					*pbOutBufLeft3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft3++;
					*pbOutBufLeft3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft3++;
					// copy right channel sample
					*pbOutBufRight3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight3++;
					*pbOutBufRight3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight3++;
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
					// copy left channel sample
					*pbOutBufLeft2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft2++;
					*pbOutBufLeft2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft2++;
					// copy right channel sample
					*pbOutBufRight2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight2++;
					*pbOutBufRight2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight2++;

					// copy left channel sample
					*pbOutBufLeft3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft3++;
					*pbOutBufLeft3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft3++;
					// copy right channel sample
					*pbOutBufRight3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight3++;
					*pbOutBufRight3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight3++;

					// copy left channel sample
					*pbOutBufLeft4 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft4++;
					*pbOutBufLeft4 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft4++;
					// copy right channel sample
					*pbOutBufRight4 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight4++;
					*pbOutBufRight4 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight4++;
#endif //USE_FOUR_CODEC
#else // SAMP_24BIT
					// copy left channel sample
					*pbOutBufLeft = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft +=2;
					// copy right channel sample
					*pbOutBufRight = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight +=2;
#ifdef USE_TWO_CODEC
					// copy left channel sample
					*pbOutBufLeft2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft2 +=2;
					// copy right channel sample
					*pbOutBufRight2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight2 +=2;
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
					// copy left channel sample
					*pbOutBufLeft2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft2 +=2;
					// copy right channel sample
					*pbOutBufRight2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight2 +=2;

					// copy left channel sample
					*pbOutBufLeft3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft3 +=2;
					// copy right channel sample
					*pbOutBufRight3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight3 +=2;
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
					// copy left channel sample
					*pbOutBufLeft2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft2 +=2;
					// copy right channel sample
					*pbOutBufRight2 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight2 +=2;

					// copy left channel sample
					*pbOutBufLeft3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft3 +=2;
					// copy right channel sample
					*pbOutBufRight3 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight3 +=2;

					// copy left channel sample
					*pbOutBufLeft4 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufLeft4 +=2;
					// copy right channel sample
					*pbOutBufRight4 = codec_output_buffer[codec_output_buffer_output_index];
					codec_output_buffer_output_index++;
					pbOutBufRight4 +=2;
#endif //USE_FOUR_CODEC
#endif //SAMP_24BIT
					// loop around
					if (codec_output_buffer_output_index>=CODEC_OUTPUT_BUFFER_SIZE)
						codec_output_buffer_output_index = 0;
				}
				
				// reduce the microframe counter by TXBUFF_SZ_MSEC*8 (there are 8 microframes per millisecond)
				codec_output_buffer_sample -= MAX_TXBUFF_SZ_DACSAMPS;
			} else
			{
				codec_output_buffer_underflow++;
				// codec_output_buffer underflow, send zero instead
	            ///memset(pbOutBufLeft, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);
	            ///memset(pbOutBufRight, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);
				for (i=0; i<MAX_TXBUFF_SZ_DACSAMPS; i++)
				{
					pbOutBufLeft[i] = 0;
					pbOutBufRight[i] = 0;
					if (bufferInIdx<256){
						bufferIn[bufferInIdx] = 0;
						bufferInIdx++;
						// if the bufferIn is filled, then send a semaphore to the SpectrumDisplayTask
						if (bufferInIdx==256)
						{
							// send semaphore
							SEM_post(&SEM_PingPongRxLeftComplete4);
						}
					}
				}
#ifdef USE_TWO_CODEC
				// codec_output_buffer underflow, send zero instead
	            //memset(pbOutBufLeft2, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);
	            //memset(pbOutBufRight2, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);
				for (i=0; i<MAX_TXBUFF_SZ_DACSAMPS; i++)
				{
					pbOutBufLeft2[i] = 0;
					pbOutBufRight2[i] = 0;
				}
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
				// codec_output_buffer underflow, send zero instead
	            ///memset(pbOutBufLeft2, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);
	            ///memset(pbOutBufRight2, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);

				// codec_output_buffer underflow, send zero instead
	            ///memset(pbOutBufLeft3, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);
	            ///memset(pbOutBufRight3, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);
				for (i=0; i<MAX_TXBUFF_SZ_DACSAMPS; i++)
				{
					pbOutBufLeft2[i] = 0;
					pbOutBufRight2[i] = 0;
					pbOutBufLeft3[i] = 0;
					pbOutBufRight3[i] = 0;
				}
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
				// codec_output_buffer underflow, send zero instead
	            ///memset(pbOutBufLeft2, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);
	            ///memset(pbOutBufRight2, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);

				// codec_output_buffer underflow, send zero instead
	            ///memset(pbOutBufLeft3, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);
	            ///memset(pbOutBufRight3, 0x7F, 2*MAX_TXBUFF_SZ_DACSAMPS);
				for (i=0; i<MAX_TXBUFF_SZ_DACSAMPS; i++)
				{
					pbOutBufLeft2[i] = 0;
					pbOutBufRight2[i] = 0;
					pbOutBufLeft3[i] = 0;
					pbOutBufRight3[i] = 0;
					pbOutBufLeft4[i] = 0;
					pbOutBufRight4[i] = 0;
				}
#endif //USE_FOUR_CODEC
			}
        } // end if usb_play_mode
        else
        {
        	if (!mute_play_status){
        		codecCfgMsg.wMsg = CODEC_CFG_MSG_MUTE;
				MBX_post(&MBX_codecConfig, &codecCfgMsg, SYS_FOREVER);
				mute_play_status = TRUE;
        	}
			playMemsetCount++;
            /* Output zeros */
            ///memset(pbOutBufLeft, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);
            ///memset(pbOutBufRight, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);
			for (i=0; i<MAX_TXBUFF_SZ_DACSAMPS; i++)
			{
				pbOutBufLeft[i] = 0;
				pbOutBufRight[i] = 0;
				if (bufferInIdx<256){
						bufferIn[bufferInIdx] = 0;
						bufferInIdx++;
						// if the bufferIn is filled, then send a semaphore to the SpectrumDisplayTask
						if (bufferInIdx==256)
						{
							// send semaphore
							SEM_post(&SEM_PingPongRxLeftComplete4);
						}
					}
			}
#ifdef USE_TWO_CODEC
			// codec_output_buffer underflow, send zero instead
            ///memset(pbOutBufLeft2, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);
            ///memset(pbOutBufRight2, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);
			for (i=0; i<MAX_TXBUFF_SZ_DACSAMPS; i++)
			{
				pbOutBufLeft2[i] = 0;
				pbOutBufRight2[i] = 0;
			}
#endif //USE_TWO_CODEC
#ifdef USE_THREE_CODEC
			// codec_output_buffer underflow, send zero instead
            ///memset(pbOutBufLeft2, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);
            ///memset(pbOutBufRight2, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);

			// codec_output_buffer underflow, send zero instead
            ///memset(pbOutBufLeft3, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);
            ///memset(pbOutBufRight3, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);
			for (i=0; i<MAX_TXBUFF_SZ_DACSAMPS; i++)
			{
				pbOutBufLeft2[i] = 0;
				pbOutBufRight2[i] = 0;
				pbOutBufLeft3[i] = 0;
				pbOutBufRight3[i] = 0;
			}
#endif //USE_THREE_CODEC
#ifdef USE_FOUR_CODEC
			// codec_output_buffer underflow, send zero instead
            ///memset(pbOutBufLeft2, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);
            ///memset(pbOutBufRight2, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);

			// codec_output_buffer underflow, send zero instead
            ///memset(pbOutBufLeft3, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);
            ///memset(pbOutBufRight3, 0, 2*MAX_TXBUFF_SZ_DACSAMPS);
			for (i=0; i<MAX_TXBUFF_SZ_DACSAMPS; i++)
			{
				pbOutBufLeft2[i] = 0;
				pbOutBufRight2[i] = 0;
				pbOutBufLeft3[i] = 0;
				pbOutBufRight3[i] = 0;
				pbOutBufLeft4[i] = 0;
				pbOutBufRight4[i] = 0;
			}
#endif //USE_FOUR_CODEC
        }
        /*                                      */
        /* Insert Playback audio algorithm here */
        /*                                      */
    }
}
