//////////////////////////////////////////////////////////////////////////////
// * File name: ref_data_bypass.h
// *                                                                          
// * Description:  external buffer definitions.
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

#ifndef _REF_DATA_H_
#define _REF_DATA_H_


#include "data_types.h"

#define COEF_48 	            48
#define XMIT_BUFF_SIZE          48*2


extern Int16 RcvL1[];
extern Int16 RcvR1[];
extern Int16 RcvL2[];
extern Int16 RcvR2[];

extern Int16 coeff_fir_48[];

//extern Int16 XmitL1[];
//extern Int16 XmitR1[];
//extern Int16 XmitL2[];
//extern Int16 XmitR2[];

extern Int16 FilterOutL1[];
extern Int16 FilterOutR1[];
extern Int16 FilterOutL2[];
extern Int16 FilterOutR2[];
extern Int16 DelayFilterOutL[];
extern Int16 DelayFilterOutR[];
extern Int16 fir(Int16 *input,Int16 *Coeff,Int16 *output, Int16 *dbuffer, Int16 NumOfInput,Int16 NumOfCoeff);

#endif //_REF_DATA_H_
