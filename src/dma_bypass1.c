//////////////////////////////////////////////////////////////////////////////
// * File name: dma_bypass1.c
// *                                                                          
// * Description: This file includes DMA configuration functions and DMA ISR function.
// *                                                                          
// * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
// *                                                                          
// *                                                                          
// *  Redistribution and use in source and binary forms, with or without      
// *  modification, are permitted provided that the following conditions      
// *  are met:                                                                
// *                                                                          
// *    Redistributions of source code must retain the above copyright        
// *    notice, this list of conditions and the following disclaimer.         
// *                                                                          
// *    Redistributions in binary form must reproduce the above copyright     
// *    notice, this list of conditions and the following disclaimer in the   
// *    documentation and/or other materials provided with the                
// *    distribution.                                                         
// *                                                                          
// *    Neither the name of Texas Instruments Incorporated nor the names of   
// *    its contributors may be used to endorse or promote products derived   
// *    from this software without specific prior written permission.         
// *                                                                          
// *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     
// *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       
// *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   
// *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT    
// *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   
// *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        
// *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
// *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   
// *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     
// *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   
// *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
// *                                                                          
//////////////////////////////////////////////////////////////////////////////

#include "data_types.h"
#include "register_dma.h"
#include "register_cpu.h"
#include "dma_bypass1.h"
#include "ref_data_bypass.h"


#define AUTO_RELOAD     1
#define SUCCESS			0

extern Uint16 fSineWave;

Int16 RcvL1[1024], RcvL2[1024], RcvR1[1024], RcvR2[1024];

Uint32 RxL1_DMA_address;
Uint32 RxL2_DMA_address;
Uint32 RxR1_DMA_address;
Uint32 RxR2_DMA_address;
Uint32 TxL1_DMA_address;
Uint32 TxL2_DMA_address;
Uint32 TxR1_DMA_address;
Uint32 TxR2_DMA_address;

Uint16 CurrentRxL_DMAChannel =1;
Uint16 CurrentRxR_DMAChannel =1;
// 1: L1, R1 channel
// 2: L2, R2 channel 

Uint16 CurrentTxL_DMAChannel =1;
Uint16 CurrentTxR_DMAChannel =1;
// 1: L1, R1 channel
// 2: L2, R2 channel 
Uint16 RunFilterForL =0;
Uint16 RunFilterForR =0;

extern Int16 XmitL1[];
extern Int16 XmitR1[];

void setDMA_address(void)
{

#if 0 // double buffer with filtering
    RxL1_DMA_address = (Uint32)RcvL1;   
    RxL1_DMA_address = (RxL1_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA

    RxL2_DMA_address = (Uint32)RcvL2;   
    RxL2_DMA_address = (RxL2_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA

    RxR1_DMA_address = (Uint32)RcvR1;   
    RxR1_DMA_address = (RxR1_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA

    RxR2_DMA_address = (Uint32)RcvR2;   
    RxR2_DMA_address = (RxR2_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA

    TxL1_DMA_address = (Uint32)FilterOutL1;   
    TxL1_DMA_address = (TxL1_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA

    TxL2_DMA_address = (Uint32)FilterOutL2;   
    TxL2_DMA_address = (TxL2_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA
    
    TxR1_DMA_address = (Uint32)FilterOutR1;   
    TxR1_DMA_address = (TxR1_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA
    
    TxR2_DMA_address = (Uint32)FilterOutR2;   
    TxR2_DMA_address = (TxR2_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA      

#else
    // just bypass with double buffering, for checking ping-pong buffer 
    RxL1_DMA_address = (Uint32)RcvL1;   
    RxL1_DMA_address = (RxL1_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA

    RxL2_DMA_address = (Uint32)RcvL2;   
    RxL2_DMA_address = (RxL2_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA

    RxR1_DMA_address = (Uint32)RcvR1;   
    RxR1_DMA_address = (RxR1_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA

    RxR2_DMA_address = (Uint32)RcvR2;   
    RxR2_DMA_address = (RxR2_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA

    TxL1_DMA_address = (Uint32)RcvL2;   
    TxL1_DMA_address = (TxL1_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA

    TxL2_DMA_address = (Uint32)RcvL1;   
    TxL2_DMA_address = (TxL2_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA
    
    TxR1_DMA_address = (Uint32)RcvR2;   
    TxR1_DMA_address = (TxR1_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA
    
    TxR2_DMA_address = (Uint32)RcvR1;   
    TxR2_DMA_address = (TxR2_DMA_address<<1) + 0x10000;		// change word address to byte address and add DARAM offset for DMA      

#endif
    
}

void set_dma0_ch0_stop(void)
{
	Uint16 temp;
//  DMA stop
    temp = DMA0_CH0_TC_MSW;
	temp &= 0x7FFF;
	DMA0_CH0_TC_MSW = temp;    
}

void set_dma0_ch1_stop(void)
{
	Uint16 temp;
//  DMA stop
    temp = DMA0_CH1_TC_MSW;
	temp &= 0x7FFF;
	DMA0_CH1_TC_MSW = temp;    
}

void set_dma0_ch2_stop(void)
{
	Uint16 temp;
//  DMA stop
    temp = DMA0_CH2_TC_MSW;
	temp &= 0x7FFF;
	DMA0_CH2_TC_MSW = temp;    
}

void set_dma0_ch3_stop(void)
{
	Uint16 temp;
//  DMA stop
    temp = DMA0_CH3_TC_MSW;
	temp &= 0x7FFF;
	DMA0_CH3_TC_MSW = temp;    
}

Uint16 set_dma0_ch0_i2s0_Lout(void)
{
	Uint16 temp; 

    DMA0_CH0_TC_LSW = XMIT_BUFF_SIZE*2;

#if(AUTO_RELOAD ==1)        
    DMA0_CH0_TC_MSW = 0x3204;     //src incre, destination fix, sync, auto, int 
#else
    DMA0_CH0_TC_MSW = 0x2204;     //src incre, destination fix, sync, No auto, int
#endif
                
	temp = DMA0_CH10_EVENT_SRC;
    DMA0_CH10_EVENT_SRC = temp | 0x0001;   // I2S2 transmit event
    
    DMA0_CH0_SRC_LSW = (Uint16)TxL1_DMA_address;
    DMA0_CH0_SRC_MSW = 0xFFFF & (TxL1_DMA_address >> 16);
    
    DMA0_CH0_DST_LSW = 0x2808;      // is20 transmit left data register lsw
    DMA0_CH0_DST_MSW = 0;

    CurrentTxL_DMAChannel =1;    
//  DMA starts   
    temp = DMA0_CH0_TC_MSW;
	temp |= 0x8000;
	DMA0_CH0_TC_MSW = temp;

    return SUCCESS;
}

Uint16 set_dma0_ch1_i2s0_Rout(void)
{	
	Uint16 temp;

    DMA0_CH1_TC_LSW = XMIT_BUFF_SIZE*2;
//    DMA0_CH1_TC_LSW = 8;

#if(AUTO_RELOAD ==1)            
    DMA0_CH1_TC_MSW = 0x3204;     //src incre, destination fix, sync, auto
#else    
    DMA0_CH1_TC_MSW = 0x2204;     //src incre, destination fix, sync, No auto
#endif
            
	temp = DMA0_CH10_EVENT_SRC;
    DMA0_CH10_EVENT_SRC = temp | 0x0100;   // I2S2 transmit event
    
    DMA0_CH1_SRC_LSW = (Uint16)TxR1_DMA_address;
    DMA0_CH1_SRC_MSW = 0xFFFF & (TxR1_DMA_address >> 16);
    
    DMA0_CH1_DST_LSW = 0x280C;      // is20 transmit right data register lsw
    DMA0_CH1_DST_MSW =0;
    
    CurrentTxR_DMAChannel =1;
// DMA starts    
    temp = DMA0_CH1_TC_MSW;
	temp |= 0x8000;
	DMA0_CH1_TC_MSW = temp;


    return SUCCESS;
}

Uint16 set_dma0_ch2_i2s0_Lin(void)
{
	Uint16 temp; 

    DMA0_CH2_TC_LSW = XMIT_BUFF_SIZE*2;

#if(AUTO_RELOAD ==1)        
    DMA0_CH2_TC_MSW = 0x3084;     //src fix, destination increase, sync, auto, int 
#else
    DMA0_CH2_TC_MSW = 0x2084;     //src fix, destination increase,, No auto, int
#endif
                
	temp = DMA0_CH32_EVENT_SRC;
    DMA0_CH32_EVENT_SRC = temp | 0x0002;   // I2S2 receive event

    DMA0_CH2_DST_LSW = (Uint16)RxL1_DMA_address;
    DMA0_CH2_DST_MSW = 0xFFFF & (RxL1_DMA_address >> 16);
    
    DMA0_CH2_SRC_LSW = 0x2828;      // is22 receive left data register lsw
    DMA0_CH2_SRC_MSW = 0;

    CurrentRxL_DMAChannel =1;
//  DMA starts   
    temp = DMA0_CH2_TC_MSW;
	temp |= 0x8000;
	DMA0_CH2_TC_MSW = temp;

    return SUCCESS;
}

Uint16 set_dma0_ch3_i2s0_Rin(void)
{
	Uint16 temp; 

    DMA0_CH3_TC_LSW = XMIT_BUFF_SIZE*2;

#if(AUTO_RELOAD ==1)        
    DMA0_CH3_TC_MSW = 0x3084;     //src fix, destination increase, sync, auto , int
#else
    DMA0_CH3_TC_MSW = 0x2084;     //src fix, destination increase,, No auto, int  
#endif
                
	temp = DMA0_CH32_EVENT_SRC;
    DMA0_CH32_EVENT_SRC = temp | 0x0200;   // I2S2 receive event
    
    DMA0_CH3_DST_LSW = (Uint16)RxR1_DMA_address;
    DMA0_CH3_DST_MSW = 0xFFFF & (RxR1_DMA_address >> 16);
    
    DMA0_CH3_SRC_LSW = 0x282C;      // is22 receive right data register lsw
    DMA0_CH3_SRC_MSW = 0;

    CurrentRxR_DMAChannel =1;
//  DMA starts   
    temp = DMA0_CH3_TC_MSW;
	temp |= 0x8000;
	DMA0_CH3_TC_MSW = temp;

    return SUCCESS;
}






void enable_dma_int(void)
{
    // interrupt order: TxL -> TxR -> RxL -> RxR
    // enable only TxR (channel 1) and RxR (channel 3).
    //DMA_MSK = 0x00F0;     // enable DMA1, channel 3 and channel 1 interrupts
    DMA_MSK = 0x000F;     // enable DMA interrupts
    DMA_IFR = 0xFFFF;     // clear interrupt flags
}


interrupt void DMA_Isr_1(void)
{   
    Uint16 temp;//, dma_start;
    
    temp = IFR0;
    IFR0 = temp;  

	temp = DMA_IFR;
//    DMA_IFR = temp;     // clear interrupt flags

    if(temp&0x0001)
    {
        // DMA TxL interrupt (channel 0)
        if(CurrentTxL_DMAChannel ==1)
        {
            CurrentTxL_DMAChannel =2;
            DMA0_CH0_SRC_LSW = (Uint16)TxL2_DMA_address;
            DMA0_CH0_SRC_MSW = 0xFFFF & (TxL2_DMA_address >> 16);
        
                
        }
        else
        {
            CurrentTxL_DMAChannel =1;
            DMA0_CH0_SRC_LSW = (Uint16)TxL1_DMA_address;
            DMA0_CH0_SRC_MSW = 0xFFFF & (TxL1_DMA_address >> 16);
        }
        DMA_IFR = 0x0001;     // clear interrupt flags
        //dma_start = DMA1_CH0_TC_MSW;
	    //dma_start |= 0x8000;
	    //DMA1_CH0_TC_MSW = dma_start;
    } 
    else if(temp&0x0002)
    {
        // DMA TxR interrupt (channel 1)
        if(CurrentTxR_DMAChannel ==1)
        {
            CurrentTxR_DMAChannel =2;
            DMA0_CH1_SRC_LSW = (Uint16)TxR2_DMA_address;
            DMA0_CH1_SRC_MSW = 0xFFFF & (TxR2_DMA_address >> 16);
        }
        else
        {
            CurrentTxR_DMAChannel =1;
            DMA0_CH1_SRC_LSW = (Uint16)TxR1_DMA_address;
            DMA0_CH1_SRC_MSW = 0xFFFF & (TxR1_DMA_address >> 16);
        }
        DMA_IFR = 0x0002;     // clear interrupt flags        
        //dma_start = DMA1_CH1_TC_MSW;
	    //dma_start |= 0x8000;
	    //DMA1_CH1_TC_MSW = dma_start;        
    }
    else if(temp&0x0004)
    {
        RunFilterForL =1;        
        // DMA RxL interrupt (channel 2)
        if(CurrentRxL_DMAChannel ==1)
        {
            // change DMA Tx SRC address
            CurrentRxL_DMAChannel =2;

            DMA0_CH2_DST_LSW = (Uint16)RxL2_DMA_address;
            DMA0_CH2_DST_MSW = 0xFFFF & (RxL2_DMA_address >> 16);
        }
        else
        {
            CurrentRxL_DMAChannel =1;

            DMA0_CH2_DST_LSW = (Uint16)RxL1_DMA_address;
            DMA0_CH2_DST_MSW = 0xFFFF & (RxL1_DMA_address >> 16);
        }
        DMA_IFR = 0x0004;     // clear interrupt flags        
        //dma_start = DMA1_CH2_TC_MSW;
	    //dma_start |= 0x8000;
	    //DMA1_CH2_TC_MSW = dma_start;        
    }
    else if(temp&0x0008)
    {
        RunFilterForR =1;                
        // DMA RxR interrupt (channel 3)
        if(CurrentRxR_DMAChannel ==1)
        {
            // change DMA Tx SRC address
            CurrentRxR_DMAChannel =2;

            DMA0_CH3_DST_LSW = (Uint16)RxR2_DMA_address;
            DMA0_CH3_DST_MSW = 0xFFFF & (RxR2_DMA_address >> 16);
        }
        else
        {
            CurrentRxR_DMAChannel =1;

            DMA0_CH3_DST_LSW = (Uint16)RxR1_DMA_address;
            DMA0_CH3_DST_MSW = 0xFFFF & (RxR1_DMA_address >> 16);
        }
        DMA_IFR = 0x0008;     // clear interrupt flags        
        //dma_start = DMA1_CH3_TC_MSW;
	    //dma_start |= 0x8000;
	    //DMA1_CH3_TC_MSW = dma_start;        
    }
}

