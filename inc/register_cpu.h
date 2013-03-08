//////////////////////////////////////////////////////////////////////////////
// * File name: register_cpu.h
// *                                                                          
// * Description:  definition of CPU registers.
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

#ifndef _REG_CPU_H_
#define _REG_CPU_H_

#define IER0        *(volatile unsigned *)0x0000
#define IFR0        *(volatile unsigned *)0x0001
#define ST0_55      *(volatile unsigned *)0x0002
#define ST1_55      *(volatile unsigned *)0x0003
#define ST3_55      *(volatile unsigned *)0x0004
#define IER1        *(volatile unsigned *)0x0045
#define IFR1        *(volatile unsigned *)0x0046

//**************************************************************************
//        The IFR0 (Interrupt Flag Register 0)
//**************************************************************************
//15    14   13 12    11    10     9    8
//RCV2 XMT2 SAR LCD PROG3 CoProc PROG2 DMA                                                          
//7      6     5    4     3    2    1    0                                                                                   
//PROG1 UART PROG0 TINT INT1 INT0 Reserved                                                          

//**************************************************************************
//        The IFR1 (Interrupt Flag Register 1)
//**************************************************************************
//15   11   10    9   8  
//Reserved RTOS DLOG BERR                                             
//7     6          5   4    3    2             1     0                                                                                   
//I2C EMIF_Error GPIO USB  SPI Wakeup or RTC RCV3   XMT3                                                                                         


//RESET     Reset (hardware & software)
//NMI       Nonmaskable interrupt
//INT0      External user interrupt #0
//INT1      External user interrupt #1
//TINT      Timer aggregated interrupts
//PROG0     Programmable Transmit Interrupt 0 (I2S0Transmit or MMC/SD0 interrupt)
//UART      UART interrupt
//PROG1     Programmable Receive Interrupt 0 (I2S0Receive or MMC/SD0 SDIO interrupt)
//DMA       DMA aggregated interrupts
//PROG2     Programmable Transmit Interrupt 1 (I2S1Transmit or MMC/SD1 interrupt)
//CoProc    Coprocessor Engine Interrupt
//PROG3     Programmable Receive Interrupt 1 (I2S1Receive or MMC/SD1 SDIO interrupt)
//LCD       LCD Interrupt
//SAR       10-bit SAR A/D Conversion or pen Interrupt
//XMT2      I2S2 Transmit Interrupt
//RCV2      I2S2 Receive Interrupt
//XMT3      I2S3 Transmit Interrupt
//RCV3      I2S3 Receive Interrupt
//RTC       Wakeup or Real-time clock interrupt
//SPI       SPI interrupt
//USB       USB Interrupt
//GPIO      GPIO aggregated interrupts
//EMIF      EMIF Error interrupt
//I2C       IIC interrupt
//BERR      Bus Error interrupt
//DLOG      Emulation interrupt - DLOG
//RTOS      Emulation interrupt - RTOS
//RTDXRCV   Emulation interrupt – RTDX receive
//RTDXXMT   Emulation interrupt – RTDX transmit
//EMUINT    Emulation monitor mode interrupt
//SINT30    Software interrupt #30
//SINT31    Software interrupt #31

#endif
