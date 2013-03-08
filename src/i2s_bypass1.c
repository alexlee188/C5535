//////////////////////////////////////////////////////////////////////////////
// * File name: i2s_bypass1.c
// *                                                                          
// * Description: This file includes I2S configuration functions.
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
#include "register_cpu.h"
#include "i2s_bypass1.h"

#define SUCCESS		0

#define WORD_8		0
#define WORD_16		0x04
#define WORD_24		0x07
#define WORD_32		0x08

#define WORD_SIZE	WORD_16	 

#define FS_8		0
#define FS_16		0x1
#define FS_32		0x2
#define FS_64		0x3
#define FS_128		0x4
#define FS_256		0x5

#define FS_DIV		FS_32

#define BUF_SIZE 	48
//Uint16 I2S_RCV_L[BUF_SIZE];
//Uint16 I2S_RCV_R[BUF_SIZE];
Uint16 Buf_Address =0;


Uint16 set_i2s0_master(void)
{
    Uint16 temp=0;

	temp = WORD_SIZE <<2;
	temp |= 0x0082;				// Master, Pack
    i2s0_write_CR(temp);        // Master, I2S, 16 bit
    temp = i2s0_read_CR();
    
	temp = FS_DIV <<3;
	temp |= 0x0005; 				// clock = CPU / 4 
    i2s0_write_SRGR(temp);       
    
#if(USE_DMA ==1)    
    i2s0_write_INT_MASK(0);         // Disable I2S interrupt
#else
    i2s0_write_INT_MASK(0x20);         // Tx stereo interrupt

    i2s0_write_TXL_LSW(0x5678);
    i2s0_write_TXL_MSW(0x1234);
    i2s0_write_TXR_LSW(0x5678);
    i2s0_write_TXR_MSW(0x1234);
#endif

        
    return SUCCESS;
}

void set_i2s0_slave(void)
{
    Uint16 temp=0;

	temp = WORD_SIZE <<2;
	temp |= 0x0080;				// Slave, Pack
    i2s0_write_CR(temp);        // Master, I2S, 16 bit
    temp = i2s0_read_CR();

#if(USE_DMA ==1)    
    i2s0_write_INT_MASK(0);         // Disable I2S interrupt
#else
    i2s0_write_INT_MASK(0x20);         // Tx stereo interrupt

    i2s0_write_TXL_LSW(0x5678);
    i2s0_write_TXL_MSW(0x1234);
    i2s0_write_TXR_LSW(0x5678);
    i2s0_write_TXR_MSW(0x1234);
#endif
    
}
