
;//////////////////////////////////////////////////////////////////////////////
;// * File name: i2s_register.asm
;// *                                                                          
;// * Description:  definition of i2s registers and functions to read/write i2s reigsters.
;// *                                                                          
;// * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
;// *                                                                          
;// *                                                                          
;// *  Redistribution and use in source and binary forms, with or without      
;// *  modification, are permitted provided that the following conditions      
;// *  are met:                                                                
;// *                                                                          
;// *    Redistributions of source code must retain the above copyright        
;// *    notice, this list of conditions and the following disclaimer.         
;// *                                                                          
;// *    Redistributions in binary form must reproduce the above copyright     
;// *    notice, this list of conditions and the following disclaimer in the   
;// *    documentation and/or other materials provided with the                
;// *    distribution.                                                         
;// *                                                                          
;// *    Neither the name of Texas Instruments Incorporated nor the names of   
;// *    its contributors may be used to endorse or promote products derived   
;// *    from this software without specific prior written permission.         
;// *                                                                          
;// *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     
;// *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       
;// *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   
;// *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT    
;// *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   
;// *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        
;// *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
;// *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   
;// *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     
;// *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   
;// *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
;// *                                                                          
;//////////////////////////////////////////////////////////////////////////////
         .mmregs
         .include "lpva200.inc"
         
         .C54CM_off
         .CPL_off
         .ARMS_off

         .text   

        .global _i2s0_read_CR
        .global _i2s0_write_CR
        .global _i2s0_read_SRGR
        .global _i2s0_write_SRGR
        .global _i2s0_read_TXL_LSW
        .global _i2s0_write_TXL_LSW
        .global _i2s0_read_TXL_MSW
        .global _i2s0_write_TXL_MSW
        .global _i2s0_read_TXR_LSW
        .global _i2s0_write_TXR_LSW
        .global _i2s0_read_TXR_MSW
        .global _i2s0_write_TXR_MSW
        .global _i2s0_read_INTR
        .global _i2s0_read_INT_MASK
        .global _i2s0_write_INT_MASK
        .global _i2s0_read_RXL_LSW
        .global _i2s0_write_RXL_LSW
        .global _i2s0_read_RXL_MSW
        .global _i2s0_write_RXL_MSW
        .global _i2s0_read_RXR_LSW
        .global _i2s0_write_RXR_LSW
        .global _i2s0_read_RXR_MSW
        .global _i2s0_write_RXR_MSW
        .global _i2s1_read_CR
        .global _i2s1_write_CR
        .global _i2s1_read_SRGR
        .global _i2s1_write_SRGR
        .global _i2s1_read_TXL_LSW
        .global _i2s1_write_TXL_LSW
        .global _i2s1_read_TXL_MSW
        .global _i2s1_write_TXL_MSW
        .global _i2s1_read_TXR_LSW
        .global _i2s1_write_TXR_LSW
        .global _i2s1_read_TXR_MSW
        .global _i2s1_write_TXR_MSW
        .global _i2s1_read_INTR
        .global _i2s1_read_INT_MASK
        .global _i2s1_write_INT_MASK
        .global _i2s1_read_RXL_LSW
        .global _i2s1_write_RXL_LSW
        .global _i2s1_read_RXL_MSW
        .global _i2s1_write_RXL_MSW
        .global _i2s1_read_RXR_LSW
        .global _i2s1_write_RXR_LSW
        .global _i2s1_read_RXR_MSW
        .global _i2s1_write_RXR_MSW
        .global _i2s2_read_CR
        .global _i2s2_write_CR
        .global _i2s2_read_SRGR
        .global _i2s2_write_SRGR
        .global _i2s2_read_TXL_LSW
        .global _i2s2_write_TXL_LSW
        .global _i2s2_read_TXL_MSW
        .global _i2s2_write_TXL_MSW
        .global _i2s2_read_TXR_LSW
        .global _i2s2_write_TXR_LSW
        .global _i2s2_read_TXR_MSW
        .global _i2s2_write_TXR_MSW
        .global _i2s2_read_INTR
        .global _i2s2_read_INT_MASK
        .global _i2s2_write_INT_MASK
        .global _i2s2_read_RXL_LSW
        .global _i2s2_write_RXL_LSW
        .global _i2s2_read_RXL_MSW
        .global _i2s2_write_RXL_MSW
        .global _i2s2_read_RXR_LSW
        .global _i2s2_write_RXR_LSW
        .global _i2s2_read_RXR_MSW
        .global _i2s2_write_RXR_MSW
        .global _i2s3_read_CR
        .global _i2s3_write_CR
        .global _i2s3_read_SRGR
        .global _i2s3_write_SRGR
        .global _i2s3_read_TXL_LSW
        .global _i2s3_write_TXL_LSW
        .global _i2s3_read_TXL_MSW
        .global _i2s3_write_TXL_MSW
        .global _i2s3_read_TXR_LSW
        .global _i2s3_write_TXR_LSW
        .global _i2s3_read_TXR_MSW
        .global _i2s3_write_TXR_MSW
        .global _i2s3_read_INTR
        .global _i2s3_read_INT_MASK
        .global _i2s3_write_INT_MASK
        .global _i2s3_read_RXL_LSW
        .global _i2s3_write_RXL_LSW
        .global _i2s3_read_RXL_MSW
        .global _i2s3_write_RXL_MSW
        .global _i2s3_read_RXR_LSW
        .global _i2s3_write_RXR_LSW
        .global _i2s3_read_RXR_MSW
        .global _i2s3_write_RXR_MSW
        .global _enable_i2s0
        .global _enable_i2s1
        .global _enable_i2s2
        .global _enable_i2s3
        .global _disable_i2s0
        .global _disable_i2s1
        .global _disable_i2s2
        .global _disable_i2s3





 
        
;//==============================================================================
;// I2S0 registers
;//==============================================================================
_i2s0_read_CR:
    T0 = *port(I2S0_CR_LSW)
    return

_i2s0_write_CR:
    *port(I2S0_CR_LSW) = T0
    return    

_i2s0_read_SRGR:
    T0 = *port(I2S0_SRGR)
    return

_i2s0_write_SRGR:
    *port(I2S0_SRGR) = T0
    return    

_i2s0_read_TXL_LSW:
    T0 = *port(I2S0_TXW0_LSW)
    return

_i2s0_write_TXL_LSW:
    *port(I2S0_TXW0_LSW) = T0
    return    

_i2s0_read_TXL_MSW:
    T0 = *port(I2S0_TXW0_MSW)
    return

_i2s0_write_TXL_MSW:
    *port(I2S0_TXW0_MSW) = T0
    return    

_i2s0_read_TXR_LSW:
    T0 = *port(I2S0_TXW1_LSW)
    return

_i2s0_write_TXR_LSW:
    *port(I2S0_TXW1_LSW) = T0
    return    

_i2s0_read_TXR_MSW:
    T0 = *port(I2S0_TXW1_MSW)
    return

_i2s0_write_TXR_MSW:
    *port(I2S0_TXW1_MSW) = T0
    return      


_i2s0_read_INTR:
    T0 = *port(I2S0_IR)
    return

_i2s0_read_INT_MASK:
    T0 = *port(I2S0_IMCR)
    return

_i2s0_write_INT_MASK:
    *port(I2S0_IMCR) = T0
    return 

_i2s0_read_RXL_LSW:
    T0 = *port(I2S0_RXW0_LSW)
    return

_i2s0_write_RXL_LSW:
    *port(I2S0_RXW0_LSW) = T0
    return    

_i2s0_read_RXL_MSW:
    T0 = *port(I2S0_RXW0_MSW)
    return

_i2s0_write_RXL_MSW:
    *port(I2S0_RXW0_MSW) = T0
    return    

_i2s0_read_RXR_LSW:
    T0 = *port(I2S0_RXW1_LSW)
    return

_i2s0_write_RXR_LSW:
    *port(I2S0_RXW1_LSW) = T0
    return    

_i2s0_read_RXR_MSW:
    T0 = *port(I2S0_RXW1_MSW)
    return

_i2s0_write_RXR_MSW:
    *port(I2S0_RXW1_MSW) = T0
    return 


;//==============================================================================
;// I2S1 registers
;//==============================================================================
_i2s1_read_CR:
    T0 = *port(I2S1_CR_LSW)
    return

_i2s1_write_CR:
    *port(I2S1_CR_LSW) = T0
    return    

_i2s1_read_SRGR:
    T0 = *port(I2S1_SRGR)
    return

_i2s1_write_SRGR:
    *port(I2S1_SRGR) = T0
    return    

_i2s1_read_TXL_LSW:
    T0 = *port(I2S1_TXW0_LSW)
    return

_i2s1_write_TXL_LSW:
    *port(I2S1_TXW0_LSW) = T0
    return    

_i2s1_read_TXL_MSW:
    T0 = *port(I2S1_TXW0_MSW)
    return

_i2s1_write_TXL_MSW:
    *port(I2S1_TXW0_MSW) = T0
    return    

_i2s1_read_TXR_LSW:
    T0 = *port(I2S1_TXW1_LSW)
    return

_i2s1_write_TXR_LSW:
    *port(I2S1_TXW1_LSW) = T0
    return    

_i2s1_read_TXR_MSW:
    T0 = *port(I2S1_TXW1_MSW)
    return

_i2s1_write_TXR_MSW:
    *port(I2S1_TXW1_MSW) = T0
    return      


_i2s1_read_INTR:
    T0 = *port(I2S1_IR)
    return

_i2s1_read_INT_MASK:
    T0 = *port(I2S1_IMCR)
    return

_i2s1_write_INT_MASK:
    *port(I2S1_IMCR) = T0
    return 

_i2s1_read_RXL_LSW:
    T0 = *port(I2S1_RXW0_LSW)
    return

_i2s1_write_RXL_LSW:
    *port(I2S1_RXW0_LSW) = T0
    return    

_i2s1_read_RXL_MSW:
    T0 = *port(I2S1_RXW0_MSW)
    return

_i2s1_write_RXL_MSW:
    *port(I2S1_RXW0_MSW) = T0
    return    

_i2s1_read_RXR_LSW:
    T0 = *port(I2S1_RXW1_LSW)
    return

_i2s1_write_RXR_LSW:
    *port(I2S1_RXW1_LSW) = T0
    return    

_i2s1_read_RXR_MSW:
    T0 = *port(I2S1_RXW1_MSW)
    return

_i2s1_write_RXR_MSW:
    *port(I2S1_RXW1_MSW) = T0
    return 

;//==============================================================================
;// I2S2 registers
;//==============================================================================
_i2s2_read_CR:
    T0 = *port(I2S2_CR_LSW)
    return

_i2s2_write_CR:
    *port(I2S2_CR_LSW) = T0
    return    

_i2s2_read_SRGR:
    T0 = *port(I2S2_SRGR)
    return

_i2s2_write_SRGR:
    *port(I2S2_SRGR) = T0
    return    

_i2s2_read_TXL_LSW:
    T0 = *port(I2S2_TXW0_LSW)
    return

_i2s2_write_TXL_LSW:
    *port(I2S2_TXW0_LSW) = T0
    return    

_i2s2_read_TXL_MSW:
    T0 = *port(I2S2_TXW0_MSW)
    return

_i2s2_write_TXL_MSW:
    *port(I2S2_TXW0_MSW) = T0
    return    

_i2s2_read_TXR_LSW:
    T0 = *port(I2S2_TXW1_LSW)
    return

_i2s2_write_TXR_LSW:
    *port(I2S2_TXW1_LSW) = T0
    return    

_i2s2_read_TXR_MSW:
    T0 = *port(I2S2_TXW1_MSW)
    return

_i2s2_write_TXR_MSW:
    *port(I2S2_TXW1_MSW) = T0
    return      


_i2s2_read_INTR:
    T0 = *port(I2S2_IR)
    return

_i2s2_read_INT_MASK:
    T0 = *port(I2S2_IMCR)
    return

_i2s2_write_INT_MASK:
    *port(I2S2_IMCR) = T0
    return 

_i2s2_read_RXL_LSW:
    T0 = *port(I2S2_RXW0_LSW)
    return

_i2s2_write_RXL_LSW:
    *port(I2S2_RXW0_LSW) = T0
    return    

_i2s2_read_RXL_MSW:
    T0 = *port(I2S2_RXW0_MSW)
    return

_i2s2_write_RXL_MSW:
    *port(I2S2_RXW0_MSW) = T0
    return    

_i2s2_read_RXR_LSW:
    T0 = *port(I2S2_RXW1_LSW)
    return

_i2s2_write_RXR_LSW:
    *port(I2S2_RXW1_LSW) = T0
    return    

_i2s2_read_RXR_MSW:
    T0 = *port(I2S2_RXW1_MSW)
    return

_i2s2_write_RXR_MSW:
    *port(I2S2_RXW1_MSW) = T0
    return     


;//==============================================================================
;// I2S3 registers
;//==============================================================================
_i2s3_read_CR:
    T0 = *port(I2S3_CR_LSW)
    return

_i2s3_write_CR:
    *port(I2S3_CR_LSW) = T0
    return    

_i2s3_read_SRGR:
    T0 = *port(I2S3_SRGR)
    return

_i2s3_write_SRGR:
    *port(I2S3_SRGR) = T0
    return    

_i2s3_read_TXL_LSW:
    T0 = *port(I2S3_TXW0_LSW)
    return

_i2s3_write_TXL_LSW:
    *port(I2S3_TXW0_LSW) = T0
    return    

_i2s3_read_TXL_MSW:
    T0 = *port(I2S3_TXW0_MSW)
    return

_i2s3_write_TXL_MSW:
    *port(I2S3_TXW0_MSW) = T0
    return    

_i2s3_read_TXR_LSW:
    T0 = *port(I2S3_TXW1_LSW)
    return

_i2s3_write_TXR_LSW:
    *port(I2S3_TXW1_LSW) = T0
    return    

_i2s3_read_TXR_MSW:
    T0 = *port(I2S3_TXW1_MSW)
    return

_i2s3_write_TXR_MSW:
    *port(I2S3_TXW1_MSW) = T0
    return      


_i2s3_read_INTR:
    T0 = *port(I2S3_IR)
    return

_i2s3_read_INT_MASK:
    T0 = *port(I2S3_IMCR)
    return

_i2s3_write_INT_MASK:
    *port(I2S3_IMCR) = T0
    return 

_i2s3_read_RXL_LSW:
    T0 = *port(I2S3_RXW0_LSW)
    return

_i2s3_write_RXL_LSW:
    *port(I2S3_RXW0_LSW) = T0
    return    

_i2s3_read_RXL_MSW:
    T0 = *port(I2S3_RXW0_MSW)
    return

_i2s3_write_RXL_MSW:
    *port(I2S3_RXW0_MSW) = T0
    return    

_i2s3_read_RXR_LSW:
    T0 = *port(I2S3_RXW1_LSW)
    return

_i2s3_write_RXR_LSW:
    *port(I2S3_RXW1_LSW) = T0
    return    

_i2s3_read_RXR_MSW:
    T0 = *port(I2S3_RXW1_MSW)
    return

_i2s3_write_RXR_MSW:
    *port(I2S3_RXW1_MSW) = T0
    return 
                        
;//==============================================================================
;// Function: _enable_i2s0
;//------------------------------------------------------------------------------
;// Description: 
;// Prototype: void enable_i2s0(void);  
;//============================================================================== 

_enable_i2s0:
    AC0 = *port(#I2S0_CR_LSW)
    AC0 |= 0x8000
    *port(#I2S0_CR_LSW)=AC0
    return

;//==============================================================================
;// Function: _enable_i2s1
;//------------------------------------------------------------------------------
;// Description: 
;// Prototype: void enable_i2s1(void);  
;//============================================================================== 

_enable_i2s1:
    AC0 = *port(#I2S1_CR_LSW)
    AC0 |= 0x8000
    *port(#I2S1_CR_LSW)=AC0
    return

;//==============================================================================
;// Function: _enable_i2s2
;//------------------------------------------------------------------------------
;// Description: 
;// Prototype: void enable_i2s2(void);  
;//============================================================================== 

_enable_i2s2:
    AC0 = *port(#I2S2_CR_LSW)
    AC0 |= 0x8000
    *port(#I2S2_CR_LSW)=AC0
    return

;//==============================================================================
;// Function: _enable_i2s3
;//------------------------------------------------------------------------------
;// Description: 
;// Prototype: void enable_i2s3(void);  
;//============================================================================== 

_enable_i2s3:
    AC0 = *port(#I2S3_CR_LSW)
    AC0 |= 0x8000
    *port(#I2S3_CR_LSW)=AC0
    return

;//==============================================================================
;// Function: _disable_i2s0
;//------------------------------------------------------------------------------
;// Description: 
;// Prototype: void disable_i2s0(void);  
;//============================================================================== 

_disable_i2s0:
    AC0 = *port(#I2S0_CR_LSW)
    AC0 &= 0x7FFF
    *port(#I2S0_CR_LSW)=AC0
    return

;//==============================================================================
;// Function: _disable_i2s1
;//------------------------------------------------------------------------------
;// Description: 
;// Prototype: void disable_i2s1(void);  
;//============================================================================== 

_disable_i2s1:
    AC0 = *port(#I2S1_CR_LSW)
    AC0 &= 0x7FFF
    *port(#I2S1_CR_LSW)=AC0
    return

;//==============================================================================
;// Function: _disable_i2s2
;//------------------------------------------------------------------------------
;// Description: 
;// Prototype: void disable_i2s2(void);  
;//============================================================================== 

_disable_i2s2:
    AC0 = *port(#I2S2_CR_LSW)
    AC0 &= 0x7FFF
    *port(#I2S2_CR_LSW)=AC0
    return

;//==============================================================================
;// Function: _disable_i2s3
;//------------------------------------------------------------------------------
;// Description: 
;// Prototype: void disable_i2s3(void);  
;//============================================================================== 

_disable_i2s3:
    AC0 = *port(#I2S3_CR_LSW)
    AC0 &= 0x7FFF
    *port(#I2S3_CR_LSW)=AC0
    return

    .end