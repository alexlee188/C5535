/*
 * $$$MODULE_NAME codec_config.c
 *
 * $$$MODULE_DESC codec_config.c
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#include <std.h>
#include "VC5505_CSL_BIOS_cfg.h"
#include "codec_aic3254.h"
#include "codec_config.h"
#include "psp_common.h"
#include "psp_dma.h"
#include "i2s_sample.h"
#include "app_asrc.h"
#include "Sample_rate.h"

Uint16 gSetRecSampRateFlag = FALSE;
long gSetRecSampRateTemp = SAMP_RATE_96KHZ;
long gSetRecSampRate = SAMP_RATE_96KHZ;
long gSetRecSampRatePrev = SAMP_RATE_96KHZ;
// the number of sample per uFrame
Uint16 gSetRecSampNum = SAMP_RATE_96KHZ/8000;
// the revise amount to the number of sample per uFrame
Uint16 gSetRecSampNumDelta = 0;

extern Uint16 uFrameCount;
extern Uint32 sof_int_count;
extern Int32 dmaSampCntPerSec;
extern Int32 usbIsoInSampCntPerSec;

void RateChange(void)
{
	Uint16 status;
	volatile Uint32 looper;

	// if the sample rate has been changed, then call the codec configuration
	if (gSetRecSampRateFlag)
	{
		// if the record sample rate is indeed changed
		if (gSetRecSampRateTemp!=gSetRecSampRate)
		{
			if ((gSetRecSampRateTemp==SAMP_RATE_96KHZ)||		// 96kHz
					(gSetRecSampRateTemp==SAMP_RATE_88_2KHZ)||	// 88.2kHz
					(gSetRecSampRateTemp==SAMP_RATE_44_1KHZ)||	// 44.1kHz
					(gSetRecSampRateTemp==SAMP_RATE_48KHZ))		// 48kHz
			{
				// stop the I2S data receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx, FALSE); 
				// stop the I2S data transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx, FALSE); 
			    /* stop left Rx DMA */
			    status = DMA_StopTransfer(hDmaRxLeft);
			    /* stop right Rx DMA */
			    status |= DMA_StopTransfer(hDmaRxRight);
			    /* stop left Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxLeft);
			    /* stop right Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxRight);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Stop CH1/2 DMA ERROR");
#endif
				}

#ifdef USE_TWO_CODEC
				// stop the I2S data receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx2, FALSE); 
				// stop the I2S data transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx2, FALSE); 
			    /* stop left Rx DMA */
			    status = DMA_StopTransfer(hDmaRxLeft2);
			    /* stop right Rx DMA */
			    status |= DMA_StopTransfer(hDmaRxRight2);
			    /* stop left Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxLeft2);
			    /* stop right Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxRight2);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Stop CH3/4 DMA ERROR");
#endif
				}
#endif // USE_TWO_CODEC

#ifdef USE_THREE_CODEC
				// stop the I2S data receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx2, FALSE); 
				// stop the I2S data transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx2, FALSE); 
			    /* stop left Rx DMA */
			    status = DMA_StopTransfer(hDmaRxLeft2);
			    /* stop right Rx DMA */
			    status |= DMA_StopTransfer(hDmaRxRight2);
			    /* stop left Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxLeft2);
			    /* stop right Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxRight2);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Stop CH3/4 DMA ERROR");
#endif
				}

				// stop the I2S data receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx3, FALSE); 
				// stop the I2S data transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx3, FALSE); 
			    /* stop left Rx DMA */
			    status = DMA_StopTransfer(hDmaRxLeft3);
			    /* stop right Rx DMA */
			    status |= DMA_StopTransfer(hDmaRxRight3);
			    /* stop left Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxLeft3);
			    /* stop right Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxRight3);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Stop CH5/6 DMA ERROR");
#endif
				}
#endif // USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
				// stop the I2S data receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx2, FALSE); 
				// stop the I2S data transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx2, FALSE); 
			    /* stop left Rx DMA */
			    status = DMA_StopTransfer(hDmaRxLeft2);
			    /* stop right Rx DMA */
			    status |= DMA_StopTransfer(hDmaRxRight2);
			    /* stop left Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxLeft2);
			    /* stop right Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxRight2);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Stop CH3/4 DMA ERROR");
#endif
				}

				// stop the I2S data receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx3, FALSE); 
				// stop the I2S data transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx3, FALSE); 
			    /* stop left Rx DMA */
			    status = DMA_StopTransfer(hDmaRxLeft3);
			    /* stop right Rx DMA */
			    status |= DMA_StopTransfer(hDmaRxRight3);
			    /* stop left Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxLeft3);
			    /* stop right Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxRight3);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Stop CH5/6 DMA ERROR");
#endif
				}

				// stop the I2S data receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx4, FALSE); 
				// stop the I2S data transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx4, FALSE); 
			    /* stop left Rx DMA */
			    status = DMA_StopTransfer(hDmaRxLeft4);
			    /* stop right Rx DMA */
			    status |= DMA_StopTransfer(hDmaRxRight4);
			    /* stop left Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxLeft4);
			    /* stop right Tx DMA */
			    status |= DMA_StopTransfer(hDmaTxRight4);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Stop CH7/8 DMA ERROR");
#endif
				}
#endif // USE_FOUR_CODEC

				/* Initialize audio module */
			    status = AIC3254_init(gSetRecSampRateTemp, gSetRecSampRateTemp);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Change Codec Sample Rate ERROR: 0x%x", gSetRecSampRate);
#endif
				}
				
				// save the previous sample rate
				gSetRecSampRatePrev = gSetRecSampRate;
				// update the sample rate
				gSetRecSampRate = gSetRecSampRateTemp;

				// compute number of samples per uFrame
				// update the gSetRecSampNum
				gSetRecSampNum = gSetRecSampRate/8000;

			    // reset codec input circular buffer
			    ///memset(codec_input_buffer, 0, CODEC_INPUT_BUFFER_SIZE*2);
				///for (looper=0; looper<CODEC_INPUT_BUFFER_SIZE; looper++)
				///	codec_input_buffer[looper] = 0;
			    codec_input_buffer_input_index = 0;
			    codec_input_buffer_output_index = 0;
			    codec_input_buffer_underflow = 0;
			    codec_input_buffer_overflow = 0;
				codec_input_buffer_sample = 0;

				// reset the ping pong buffer
				///memset(ping_pong_i2sRxLeftBuf, 0, gSetRecSampNum*8); 
				left_rx_buf_sel = 0x0;
			    /* Start left Rx DMA */
			    status = DMA_StartTransfer(hDmaRxLeft);
				// reset the ping pong buffer
				///memset(ping_pong_i2sRxRightBuf, 0, gSetRecSampNum*8); 
				right_rx_buf_sel = 0x0;
			    /* Start right Rx DMA */
			    status |= DMA_StartTransfer(hDmaRxRight);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start RX CH1/2 DMA ERROR");
#endif
				}				

			    // reset codec output circular buffer
			    ///memset(codec_output_buffer, 0, CODEC_OUTPUT_BUFFER_SIZE*2);
				///for (looper=0; looper<CODEC_OUTPUT_BUFFER_SIZE; looper++)
				///	codec_output_buffer[looper] = 0;
			    codec_output_buffer_input_index = 0;
			    codec_output_buffer_output_index = 0;
			    codec_output_buffer_underflow = 0;
			    codec_output_buffer_overflow = 0;
				codec_output_buffer_sample = 0;

				// reset the ping pong buffer
				///memset(ping_pong_i2sTxLeftBuf, 0, gSetRecSampNum*8); 
				left_tx_buf_sel = 0x0;
			    /* Start left Tx DMA */
			    status = DMA_StartTransfer(hDmaTxLeft);
				// reset the ping pong buffer
				///memset(ping_pong_i2sTxRightBuf, 0, gSetRecSampNum*8); 
				right_tx_buf_sel = 0x0;
			    /* Start right Tx DMA */
			    status |= DMA_StartTransfer(hDmaTxRight);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start TX CH1/2 DMA ERROR");
#endif
				}				
				// start the I2S dats receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx, TRUE);
				// start the I2S dats transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx, TRUE);	

#ifdef USE_TWO_CODEC
				// reset the ping pong buffer
				///memset(ping_pong_i2sRxLeftBuf2, 0, gSetRecSampNum*8); 
			    /* Start left Rx DMA */
			    status = DMA_StartTransfer(hDmaRxLeft2);
				// reset the ping pong buffer
				///memset(ping_pong_i2sRxRightBuf2, 0, gSetRecSampNum*8); 
			    /* Start right Rx DMA */
			    status |= DMA_StartTransfer(hDmaRxRight2);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start RX CH3/4 DMA ERROR");
#endif
				}				

				// reset the ping pong buffer
				///memset(ping_pong_i2sTxLeftBuf2, 0, gSetRecSampNum*8); 
			    /* Start left Tx DMA */
			    status = DMA_StartTransfer(hDmaTxLeft2);
				// reset the ping pong buffer
				///memset(ping_pong_i2sTxRightBuf2, 0, gSetRecSampNum*8); 
			    /* Start right Tx DMA */
			    status |= DMA_StartTransfer(hDmaTxRight2);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start TX CH3/4 DMA ERROR");
#endif
				}				
				// start the I2S dats receive
				//DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx2, TRUE);	
				// start the I2S dats transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx2, TRUE);	
#endif // USE_TWO_CODEC

#ifdef USE_THREE_CODEC
				// reset the ping pong buffer
				///memset(ping_pong_i2sRxLeftBuf2, 0, gSetRecSampNum*8); 
			    /* Start left Rx DMA */
			    status = DMA_StartTransfer(hDmaRxLeft2);
				// reset the ping pong buffer
				///memset(ping_pong_i2sRxRightBuf2, 0, gSetRecSampNum*8); 
			    /* Start right Rx DMA */
			    status |= DMA_StartTransfer(hDmaRxRight2);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start RX CH3/4 DMA ERROR");
#endif
				}

				// reset the ping pong buffer
				///memset(ping_pong_i2sTxLeftBuf2, 0, gSetRecSampNum*8); 
			    /* Start left Tx DMA */
			    status = DMA_StartTransfer(hDmaTxLeft2);
				// reset the ping pong buffer
				///memset(ping_pong_i2sTxRightBuf2, 0, gSetRecSampNum*8); 
			    /* Start right Tx DMA */
			    status |= DMA_StartTransfer(hDmaTxRight2);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start TX CH3/4 DMA ERROR");
#endif
				}
				// start the I2S dats receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx2, TRUE);	
				// start the I2S dats transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx2, TRUE);	

				// reset the ping pong buffer
				///memset(ping_pong_i2sRxLeftBuf3, 0, gSetRecSampNum*8); 
			    /* Start left Rx DMA */
			    status = DMA_StartTransfer(hDmaRxLeft3);
				// reset the ping pong buffer
				///memset(ping_pong_i2sRxRightBuf3, 0, gSetRecSampNum*8); 
			    /* Start right Rx DMA */
			    status |= DMA_StartTransfer(hDmaRxRight3);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start RX CH5/6 DMA ERROR");
#endif
				}

				// reset the ping pong buffer
				///memset(ping_pong_i2sTxLeftBuf3, 0, gSetRecSampNum*8); 
			    /* Start left Tx DMA */
			    status = DMA_StartTransfer(hDmaTxLeft3);
				// reset the ping pong buffer
				///memset(ping_pong_i2sTxRightBuf3, 0, gSetRecSampNum*8); 
			    /* Start right Tx DMA */
			    status |= DMA_StartTransfer(hDmaTxRight3);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start TX CH5/6 DMA ERROR");
#endif
				}
				// start the I2S dats receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx3, TRUE);	
				// start the I2S dats transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx3, TRUE);	
#endif // USE_THREE_CODEC

#ifdef USE_FOUR_CODEC
				// reset the ping pong buffer
				///memset(ping_pong_i2sRxLeftBuf2, 0, gSetRecSampNum*8); 
			    /* Start left Rx DMA */
			    status = DMA_StartTransfer(hDmaRxLeft2);
				// reset the ping pong buffer
				///memset(ping_pong_i2sRxRightBuf2, 0, gSetRecSampNum*8); 
			    /* Start right Rx DMA */
			    status |= DMA_StartTransfer(hDmaRxRight2);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start RX CH3/4 DMA ERROR");
#endif
				}

				// reset the ping pong buffer
				///memset(ping_pong_i2sTxLeftBuf2, 0, gSetRecSampNum*8); 
			    /* Start left Tx DMA */
			    status = DMA_StartTransfer(hDmaTxLeft2);
				// reset the ping pong buffer
				///memset(ping_pong_i2sTxRightBuf2, 0, gSetRecSampNum*8); 
			    /* Start right Tx DMA */
			    status |= DMA_StartTransfer(hDmaTxRight2);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start TX CH3/4 DMA ERROR");
#endif
				}
				// start the I2S dats receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx2, TRUE);	
				// start the I2S dats transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx2, TRUE);	

				// reset the ping pong buffer
				///memset(ping_pong_i2sRxLeftBuf3, 0, gSetRecSampNum*8); 
			    /* Start left Rx DMA */
			    status = DMA_StartTransfer(hDmaRxLeft3);
				// reset the ping pong buffer
				///memset(ping_pong_i2sRxRightBuf3, 0, gSetRecSampNum*8); 
			    /* Start right Rx DMA */
			    status |= DMA_StartTransfer(hDmaRxRight3);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start TX CH5/6 DMA ERROR");
#endif
				}

				// reset the ping pong buffer
				///memset(ping_pong_i2sTxLeftBuf3, 0, gSetRecSampNum*8); 
			    /* Start left Tx DMA */
			    status = DMA_StartTransfer(hDmaTxLeft3);
				// reset the ping pong buffer
				///memset(ping_pong_i2sTxRightBuf3, 0, gSetRecSampNum*8); 
			    /* Start right Tx DMA */
			    status |= DMA_StartTransfer(hDmaTxRight3);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start TX CH5/6 DMA ERROR");
#endif
				}
				// start the I2S dats receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx3, TRUE);	
				// start the I2S dats transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx3, TRUE);	

				// reset the ping pong buffer
				///memset(ping_pong_i2sRxLeftBuf4, 0, gSetRecSampNum*8); 
			    /* Start left Rx DMA */
			    status = DMA_StartTransfer(hDmaRxLeft4);
				// reset the ping pong buffer
				///memset(ping_pong_i2sRxRightBuf4, 0, gSetRecSampNum*8); 
			    /* Start right Rx DMA */
			    status |= DMA_StartTransfer(hDmaRxRight4);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start RX CH7/8 DMA ERROR");
#endif
				}
				// reset the ping pong buffer
				///memset(ping_pong_i2sTxLeftBuf4, 0, gSetRecSampNum*8); 
			    /* Start left Tx DMA */
			    status = DMA_StartTransfer(hDmaTxLeft4);
				// reset the ping pong buffer
				///memset(ping_pong_i2sTxRightBuf4, 0, gSetRecSampNum*8); 
			    /* Start right Tx DMA */
			    status |= DMA_StartTransfer(hDmaTxRight4);
				if (status!=PSP_SOK)
				{
#ifdef DEBUG_LOG_PRINT
					LOG_printf(&trace, "Start TX CH7/8 DMA ERROR");
#endif
				}
				// start the I2S dats receive
				///DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleRx4, TRUE);	
				// start the I2S dats transfer
				DDC_I2S_transEnable((DDC_I2SHandle)i2sHandleTx4, TRUE);	
#endif // USE_FOUR_CODEC
			} else
			{
				///gSetRecSampRate = gSetRecSampRatePrev;
#ifdef DEBUG_LOG_PRINT
				LOG_printf(&trace, "Unsupported Codec Sample Rate: 0x%x", gSetRecSampRate);
#endif
			}

	        /* Initialize pitch calculation */
	        initPitchCalc(hDmaTxLeft);
			// reset the accumulators
			sof_int_count = 0;
			uFrameCount = 0;
			usbIsoInSampCntPerSec = 0;
			dmaSampCntPerSec = 0;
		}
		
		gSetRecSampRateFlag = FALSE;
	}
}

/* Reconfigures codec in response to USB message */
void CodecConfigTask(void)
{
    CodecCfgMsgObj codecCfgMsg;
    Int16 *pData;

    while (1)
    {
        //TSK_settime(TSK_self()); // statistic collection

        if (MBX_pend(&MBX_codecConfig, &codecCfgMsg, SYS_FOREVER))
        {
            switch (codecCfgMsg.wMsg)
            {
                case CODEC_CFG_MSG_ADJ_VOL_L:
                    pData =  (Int16 *)codecCfgMsg.wData;
                    //Adjust_Volume(*pData, 0);
                    break;

                case CODEC_CFG_MSG_ADJ_VOL_R:
                    pData =  (Int16 *)codecCfgMsg.wData;
                    //Adjust_Volume(*pData, 1);
                    break;

                case CODEC_CFG_MSG_ADJ_MUTE:
                    pData =  (Int16 *)codecCfgMsg.wData;
                    if ((*pData & 0xff) == 0)
                    {
                        // un-mute
                        //STS_set(&mySts1, CLK_gethtime());
                        //if(Set_Mute_State(FALSE) == FALSE)
                        //{
                        //    LOG_printf(&trace, "FAILED MUTE CLEAR\n");
                        //}
                        //STS_delta(&mySts1, CLK_gethtime());
                        //TSK_deltatime(TSK_self()); // statistic collection
                        break;
                    }
                    else if ((*pData & 0xff) == 1)
                    {
                        // mute
                        //STS_set(&mySts1, CLK_gethtime());
                        //if(Set_Mute_State(TRUE) == FALSE)
                        //{
                        //    LOG_printf(&trace, "FAILED MUTE SET\n");
                        //}
                        //STS_delta(&mySts1, CLK_gethtime());
                        //TSK_deltatime(TSK_self()); // statistic collection
                    }

                    break;

                case CODEC_CFG_MSG_ADJ_RATE:
					// change to new sample rate
                    RateChange();
					
                    break;

                default:
                    break;
            }
        }
    }
}
