/*
 * $$$MODULE_NAME aic32.h
 *
 * $$$MODULE_DESC aic32.h
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

/****************************************************************************************
;
;   File Name   : AIC32.h
;   Author      : Pedro Gelabert 
;
;   Version     : 0.1
;   Date        : 09 Jan 2006
;   Update      : 
;
;   Description : AIC32 register set
;****************************************************************************************/

/*===== AIC32 IIC Address =====*/
#define AIC32_IIC_ADDRESS               0x0018

/*===== AIC32 Regsiter Map =====*/
#define AIC32_PAGE      0x00

/*******
// Page0: Control Registers 
*******/

//
// Digital Interface
#define AIC32_RESET     0x01
#define AIC32_RATE      0x02
#define AIC32_PLLa      0x03
#define AIC32_PLLb      0x04
#define AIC32_PLLc      0x05
#define AIC32_PLLd      0x06
#define AIC32_DATAPATH  0x07
#define AIC32_INTERFa   0x08
#define AIC32_INTERFb   0x09
#define AIC32_INTERFc   0x0A
#define AIC32_OVERFLOW  0x0B
#define AIC32_DIGFILT   0x0C

// #define AIC33_HEDETa 0x0D   --- Reserved
#define AIC32_HEDETb    0x0E

//Analog Input  
#define AIC32_ADCPGAL   0x0F
#define AIC32_ADCPGAR   0x10
#define AIC32_MIC3_ADCL 0x11
#define AIC32_MIC3_ADCR 0x12
#define AIC32_L1L_ADCL  0x13
#define AIC32_L2L_ADCL  0x14
#define AIC32_L1R_ADCL  0x15
#define AIC32_L1R_ADCR  0x16
#define AIC32_L2R_ADCR  0x17
#define AIC32_L1L_ADCR  0x18
#define AIC32_MICBIAS   0x19

#define AIC32_AGCLa     0x1A
#define AIC32_AGCLb     0x1B
#define AIC32_AGCLc     0x1C
#define AIC32_AGCRa     0x1D
#define AIC32_AGCRb     0x1E
#define AIC32_AGCRc     0x1F
#define AIC32_AGCLGAIN  0x20
#define AIC32_AGCRGAIN  0x21
#define AIC32_AGCLNOISE 0x22
#define AIC32_AGCRNOISE 0x23
#define AIC32_ADCFLAG   0x24

//
// Analog Output
#define AIC32_OUTPWR    0x25
#define AIC32_OUTDRIVE  0x26
#define AIC32_OUTSTAGE  0x28
#define AIC32_DACOUT    0x29
#define AIC32_OUTPOP    0x2A
#define AIC32_DACLGAIN  0x2B
#define AIC32_DACRGAIN  0x2C

#define AIC32_L2L_HPL   0x2D
#define AIC32_PGAL_HPL  0x2E
#define AIC32_DACL_HPL  0x2F
#define AIC32_L2R_HPL   0x30
#define AIC32_PGAR_HPL  0x31
#define AIC32_DACR_HPL  0x32
#define AIC32_HPLLEVEL  0x33

#define AIC32_L2L_HPLC  0x34
#define AIC32_PGAL_HPLC 0x35
#define AIC32_DACL_HPLC 0x36
#define AIC32_L2R_HPLC  0x37
#define AIC32_PGAR_HPLC 0x38
#define AIC32_DACR_HPLC 0x39
#define AIC32_HPLCLEVEL 0x3A

#define AIC32_L2L_HPR   0x3B
#define AIC32_PGAL_HPR  0x3C
#define AIC32_DACL_HPR  0x3D
#define AIC32_L2R_HPR   0x3E
#define AIC32_PGAR_HPR  0x3F
#define AIC32_DACR_HPR  0x40
#define AIC32_HPRLEVEL  0x41

#define AIC32_L2L_HPRC  0x42
#define AIC32_PGAL_HPRC 0x43
#define AIC32_DACL_HPRC 0x44
#define AIC32_L2R_HPRC  0x45
#define AIC32_PGAR_HPRC 0x46
#define AIC32_DACR_HPRC 0x47
#define AIC32_HPRCLEVEL 0x48

#define AIC32_L2L_MONO  0x49
#define AIC32_PGAL_MONO 0x4A
#define AIC32_DACL_MONO 0x4B
#define AIC32_L2R_MONO  0x4C
#define AIC32_PGAR_MONO 0x4D
#define AIC32_DACR_MONO 0x4E
#define AIC32_MONOLEVEL 0x4F

#define AIC32_L2L_LEFT  0x50
#define AIC32_PGAL_LEFT 0x51
#define AIC32_DACL_LEFT 0x52
#define AIC32_L2R_LEFT  0x53
#define AIC32_PGAR_LEFT 0x54
#define AIC32_DACR_LEFT 0x55
#define AIC32_LEFTLEVEL 0x56

#define AIC32_L2L_RGHT  0x57
#define AIC32_PGAL_RGHT 0x58
#define AIC32_DACL_RGHT 0x59
#define AIC32_L2R_RGHT  0x5A
#define AIC32_PGAR_RGHT 0x5B
#define AIC32_DACR_RGHT 0x5C
#define AIC32_RGHTLEVEL 0x5D

#define AIC32_PWRSTATUS 0x5E
#define AIC32_SCPSTATUS 0x5F
#define AIC32_STKYINTR  0x60
#define AIC32_RTINTR    0x61

//#define AIC32_GPIO1       0x62
//#define AIC32_GPIO2       0x63
//#define AIC32_GPIOa       0x64
//#define AIC32_GPIOb       0x65

#define AIC32_CLKCTRL   0x65
#define AIC32_CLKGEN    0x66

/*******
// Page1: Digital Filter Coefficients Registers 
*******/
//
// Left Channel Audio Effects Filter (AEF) Coefficients
#define AIC32_LAEF_N0MSB    0x01
#define AIC32_LAEF_N0LSB    0x02
#define AIC32_LAEF_N1MSB    0x03
#define AIC32_LAEF_N1LSB    0x04
#define AIC32_LAEF_N2MSB    0x05
#define AIC32_LAEF_N2LSB    0x06
#define AIC32_LAEF_N3MSB    0x07
#define AIC32_LAEF_N3LSB    0x08
#define AIC32_LAEF_N4MSB    0x09
#define AIC32_LAEF_N4LSB    0x0A
#define AIC32_LAEF_N5MSB    0x0B
#define AIC32_LAEF_N5LSB    0x0C

#define AIC32_LAEF_D1MSB    0x0D
#define AIC32_LAEF_D1LSB    0x0E
#define AIC32_LAEF_D2MSB    0x0F
#define AIC32_LAEF_D2LSB    0x10
#define AIC32_LAEF_D4MSB    0x11
#define AIC32_LAEF_D4LSB    0x12
#define AIC32_LAEF_D5MSB    0x13
#define AIC32_LAEF_D5LSB    0x14

//
// Left Channel De_Emphasis Filter (DEF) Coefficients
#define AIC32_LDEF_N0MSB    0x15
#define AIC32_LDEF_N0LSB    0x16
#define AIC32_LDEF_N1MSB    0x17
#define AIC32_LDEF_N1LSB    0x18

#define AIC32_LDEF_D1MSB    0x19
#define AIC32_LDEF_D1LSB    0x1A

//
// Right Channel Audio Effects Filter (AEF) Coefficients
#define AIC32_RAEF_N0MSB    0x1B
#define AIC32_RAEF_N0LSB    0x1C
#define AIC32_RAEF_N1MSB    0x1D
#define AIC32_RAEF_N1LSB    0x1E
#define AIC32_RAEF_N2MSB    0x1F
#define AIC32_RAEF_N2LSB    0x20
#define AIC32_RAEF_N3MSB    0x21
#define AIC32_RAEF_N3LSB    0x22
#define AIC32_RAEF_N4MSB    0x23
#define AIC32_RAEF_N4LSB    0x24
#define AIC32_RAEF_N5MSB    0x25
#define AIC32_RAEF_N5LSB    0x26

#define AIC32_RAEF_D1MSB    0x27
#define AIC32_RAEF_D1LSB    0x28
#define AIC32_RAEF_D2MSB    0x29
#define AIC32_RAEF_D2LSB    0x2A
#define AIC32_RAEF_D4MSB    0x2B
#define AIC32_RAEF_D4LSB    0x2C
#define AIC32_RAEF_D5MSB    0x2D
#define AIC32_RAEF_D5LSB    0x2E

//
// Right Channel De_Emphasis Filter (DEF) Coefficients
#define AIC32_RDEF_N0MSB    0x2F
#define AIC32_RDEF_N0LSB    0x30
#define AIC32_RDEF_N1MSB    0x31
#define AIC32_RDEF_N1LSB    0x32

#define AIC32_RDEF_D1MSB    0x33
#define AIC32_RDEF_D1LSB    0x34

//
// 3-D Attenuation (3DA) Coefficients
#define AIC32_3DA_MSB       0x35
#define AIC32_3DA_LSB       0x36

//
//--------------------------------------------------------------
// AIC32 Register Definitions
//--------------------------------------------------------------
//

/*******
// Page0: Control Registers 
*******/

// for both AIC32_PAGE0 AND AIC32_PAGE1
#define PAGE_PAGE0          0x00
#define PAGE_PAGE1          0x01

// AIC32_RESET
#define RESET_RESET         0x80

// AIC32_RATE
#define RATE_ADC_FS1        0x00
#define RATE_ADC_FS1P5      0x10
#define RATE_ADC_FS2        0x20
#define RATE_ADC_FS2P5      0x30
#define RATE_ADC_FS3        0x40
#define RATE_ADC_FS3P5      0x50
#define RATE_ADC_FS4        0x60
#define RATE_ADC_FS4P5      0x70
#define RATE_ADC_FS5        0x80
#define RATE_ADC_FS5P5      0x90
#define RATE_ADC_FS6        0xA0
 
#define RATE_DAC_FS1        0x00
#define RATE_DAC_FS1P5      0x01
#define RATE_DAC_FS2        0x02
#define RATE_DAC_FS2P5      0x03
#define RATE_DAC_FS3        0x04
#define RATE_DAC_FS3P5      0x05
#define RATE_DAC_FS4        0x06
#define RATE_DAC_FS4P5      0x07
#define RATE_DAC_FS5        0x08
#define RATE_DAC_FS5P5      0x09
#define RATE_DAC_FS6        0x0A
 
// AIC32_PLLx
#define PLL_DISABLE         0x00
#define PLL_ENABLE          0x80            
            
#define PLL_Q_MASK          0x78            
#define PLL_Q_16            0x00            
#define PLL_Q_17            0x08            
#define PLL_Q_2             0x10            
#define PLL_Q_3             0x18            
#define PLL_Q_4             0x20            
#define PLL_Q_5             0x28        
#define PLL_Q_6             0x30            
#define PLL_Q_7             0x38            
#define PLL_Q_8             0x40            
#define PLL_Q_9             0x48            
#define PLL_Q_10            0x50            
#define PLL_Q_11            0x58            
#define PLL_Q_12            0x60            
#define PLL_Q_13            0x68            
#define PLL_Q_14            0x70            
#define PLL_Q_15            0x78            

#define PLL_P_MASK          0x07            
#define PLL_P_8             0x00            
#define PLL_P_1             0x01        
#define PLL_P_2             0x02            
#define PLL_P_3             0x03            
#define PLL_P_4             0x04            
#define PLL_P_5             0x05        
#define PLL_P_6             0x06            
#define PLL_P_7             0x07            

#define PLL_J_MASK          0xFC      // K=J.D  
#define PLL_D_MSB_MASK      0xFF00    // D is 14-bit long   
#define PLL_D_LSB_MASK      0x00FC          

// AIC32_DATAPATH
#define DATAPATH_FSREF_48k  0x00
#define DATAPATH_FSREF_44k  0x80

#define DATAPATH_ADC_NODR   0x00
#define DATAPATH_ADC_DR     0x40

#define DATAPATH_DAC_NODR   0x00
#define DATAPATH_DAC_DR     0x20

#define DATAPATH_L_OFF      0x00
#define DATAPATH_L_LEFT     0x08
#define DATAPATH_L_RGHT     0x10
#define DATAPATH_L_MIX      0x18

#define DATAPATH_R_OFF      0x00
#define DATAPATH_R_RGHT     0x02
#define DATAPATH_R_LEFT     0x04
#define DATAPATH_R_MIX      0x06

// AIC32_INTERF
#define INTERF_BCLK_IN      0x00
#define INTERF_BCLK_OUT     0x80

#define INTERF_WCLK_IN      0x00
#define INTERF_WCLK_OUT     0x40

#define INTERF_DOUT_NOTRI   0x00
#define INTERF_DOUT_TRI     0x20

#define INTERF_I2SCLKS_NO   0x00    //WHEN POWER-DOWN & AIC32 at MASTER MODE
#define INTERF_I2SCLKS      0x10

#define INTERF_3D_DISABLE   0x00
#define INTERF_3D_ENABLE    0x04

#define INTERF_DMIC_NO      0x00
#define INTERF_DMIC_128     0x01
#define INTERF_DMIC_64      0x02
#define INTERF_DMIC_32      0x03
#define INTERF_DMIC_MASK    0x03

#define INTERF_DMODE_I2S    0x00
#define INTERF_DMODE_DSP    0x40
#define INTERF_DMODE_RJST   0x80
#define INTERF_DMODE_LJST   0xC0

#define INTERF_WLEN_16      0x00
#define INTERF_WLEN_20      0x10
#define INTERF_WLEN_24      0x20
#define INTERF_WLEN_32      0x30

#define INTERF_TXMODE_CONT  0x00    //WHEN BCLK IS MASTER/OUTPUT
#define INTERF_TXMODE_256   0x08

//#define INTERF_DOFFSET    0xFF    //WHEN AT DSP DATA MODE

// AIC32_OVERFLOW
#define OVERFLOW_LADC       0x80
#define OVERFLOW_RADC       0x40
#define OVERFLOW_LDAC       0x20
#define OVERFLOW_RDAC       0x10

// AIC32_DIGFILT
#define DIGFILT_LADC_HPF_NO     0x00
#define DIGFILT_LADC_HPF_0045   0x40
#define DIGFILT_LADC_HPF_0125   0x80
#define DIGFILT_LADC_HPF_0250   0xC0

#define DIGFILT_RADC_HPF_NO     0x00
#define DIGFILT_RADC_HPF_0045   0x10
#define DIGFILT_RADC_HPF_0125   0x20
#define DIGFILT_RADC_HPF_0250   0x30

#define DIGFILT_HPF_MASK        0xF0

#define DIGFILT_LDAC_BOOST_OFF  0x00    // LEFT DAC BOOST FILTER
#define DIGFILT_LDAC_BOOST_ON   0x08

#define DIGFILT_LDAC_DEEMP_OFF  0x00    // LEFT DAC DE-EMPHASIS FILTER
#define DIGFILT_LDAC_DEEMP_ON   0x04

#define DIGFILT_RDAC_BOOST_OFF  0x00    // RIGHT DAC BOOST FILTER
#define DIGFILT_RDAC_BOOST_ON   0x02

#define DIGFILT_RDAC_DEEMP_OFF  0x00    // RIGHT DAC DE-EMPHASIS FILTER
#define DIGFILT_RDAC_DEEMP_ON   0x01

// AIC32_HEDET
#define HEDET_DISABLE           0x00
#define HEDET_ENABLE            0x80

#define HEDET_TYPE_NO           0x00
#define HEDET_TYPE_STEEREO      0x20
#define HEDET_TYPE_CELLULAR     0x40
#define HEDET_TYPE_BOTH         0x60
#define HEDET_TYPE_MASK         0x60

#define HEDET_JACK_DEB_MASK     0x1C
#define HEDET_JACK_DEB_16ms     0x00
#define HEDET_JACK_DEB_32ms     0x04
#define HEDET_JACK_DEB_64ms     0x08
#define HEDET_JACK_DEB_128ms    0x0C
#define HEDET_JACK_DEB_256ms    0x14
#define HEDET_JACK_DEB_512ms    0x18

#define HEDET_BUTT_DEB_MASK     0x03
#define HEDET_BUTT_DEB_0ms      0x00
#define HEDET_BUTT_DEB_8ms      0x01
#define HEDET_BUTT_DEB_16ms     0x02
#define HEDET_BUTT_DEB_32ms     0x03

#define HEDET_DRV_CAPLESS       0x00
#define HEDET_DRV_CAPCOUPLED    0x80

#define HEDET_FULL_DIFF_NO      0x00
#define HEDET_FULL_DIFF         0x40

#define HEDET_BUTTPRSS_DET_FLAG 0x20
#define HEDET_HEADSET_DET_FLAG  0x10

#define HEDET_PSEUDO_DIFF_NO    0x00
#define HEDET_PSEUDO_DIFF       0x08

// AIC32_ADCPGAL/R
#define ADCPGA_VOLUME_ACTIVE    0x00
#define ADCPGA_VOLUME_MUTE      0x80

#define ADCPGA_VOLUME_MASK      0x7F

// AIC32_MIC3_ADCL/R
#define MIC3L_NOT_TO_ADC        0xF0    // MIC3 LEFT TO LEFT OR RIGHT ADC
#define MIC3L_ADC_GAIN_0DB      0x00
#define MIC3R_NOT_TO_ADC        0x0F    // MIC3 RIGHT TO LEFT OR RIGHT ADC
#define MIC3R_ADC_GAIN_0DB      0x00

// AIC32_LINE(L1L/R L2L/R)_ADCL/R
#define LINE_SINGLE_ENDED       0x00
#define LINE_DIFFERENTIAL       0x80

#define LINE_NOT_TO_ADC         0x78
#define LINE_ADC_GAIN_0DB       0x00

#define LINE_ADC_POWERDOWN      0x00
#define LINE_ADC_POWERUP        0x04

#define LINE_ADC_SS_DISABLED    0x03
#define LINE_ADC_SS_1FS         0x00
#define LINE_ADC_SS_2FS         0x01

#define LINE_ADC_BIAS_NO        0x00    // FOR ADC UN-USED INPUTS BIAS TO ADC COMMON-MODE VOLTAGE
#define LINE_ADC_BIAS           0x04

// AIC32_MICBIAS
#define MICBIAS_POWER_DOWN      0x00
#define MICBIAS_POWER_2P0V      0x40
#define MICBIAS_POWER_2P5V      0x80
#define MICBIAS_POWER_AVDD      0xC0
#define MICBIAS_POWER_MASK      0xC0

// AIC32_AGCLx AND AIC32_AGCRx
#define AGC_DISABLE             0x00
#define AGC_ENABLE              0x80

#define AGC_TARGET_GAIN_5P5     0x00
#define AGC_TARGET_GAIN_8       0x10
#define AGC_TARGET_GAIN_10      0x20
#define AGC_TARGET_GAIN_12      0x30
#define AGC_TARGET_GAIN_14      0x40
#define AGC_TARGET_GAIN_17      0x50
#define AGC_TARGET_GAIN_20      0x60
#define AGC_TARGET_GAIN_24      0x70

#define AGC_ATTACK_TIME_8ms     0x00
#define AGC_ATTACK_TIME_11ms    0x04
#define AGC_ATTACK_TIME_16ms    0x08
#define AGC_ATTACK_TIME_20ms    0x0C

#define AGC_DECAY_TIME_100ms    0x00
#define AGC_DECAY_TIME_200ms    0x01
#define AGC_DECAY_TIME_400ms    0x02
#define AGC_DECAY_TIME_500ms    0x03

#define AGC_MAXGAIN_MASK        0xFE

#define AGC_NOISE_HYST_DISABLE  0x00
#define AGC_NOISE_HYST_1DB      0x40
#define AGC_NOISE_HYST_2DB      0x80
#define AGC_NOISE_HYST_3DB      0xC0

#define AGC_NOISE_THRESH_30DB   0x00
#define AGC_NOISE_THRESH_40DB   0x10
#define AGC_NOISE_THRESH_50DB   0x18
#define AGC_NOISE_THRESH_60DB   0x20
#define AGC_NOISE_THRESH_70DB   0x28
#define AGC_NOISE_THRESH_80DB   0x30
#define AGC_NOISE_THRESH_90DB   0x38

#define AGC_CLIP_STEP_DISABLE   0x00
#define AGC_CLIP_STEP_ENABLE    0x04

// AIC32_AGCL/RNOISE
#define AGCNOISE_DEBOUNCE_0ms   0x00
#define AGCNOISE_DEBOUNCE_0P5ms 0x20
#define AGCNOISE_DEBOUNCE_1ms   0x40
#define AGCNOISE_DEBOUNCE_2ms   0x60
#define AGCNOISE_DEBOUNCE_4ms   0x80
#define AGCNOISE_DEBOUNCE_8ms   0xA0
#define AGCNOISE_DEBOUNCE_16ms  0xC0
#define AGCNOISE_DEBOUNCE_32ms  0xE0

#define AGCSIGNL_DEBOUNCE_0ms   0x00
#define AGCSIGNL_DEBOUNCE_0P5ms 0x02
#define AGCSIGNL_DEBOUNCE_1ms   0x04
#define AGCSIGNL_DEBOUNCE_2ms   0x06
#define AGCSIGNL_DEBOUNCE_4ms   0x08
#define AGCSIGNL_DEBOUNCE_8ms   0x0A
#define AGCSIGNL_DEBOUNCE_16ms  0x0C
#define AGCSIGNL_DEBOUNCE_32ms  0x0E

// AIC32 ADCFLAG
#define ADCFLAG_LPGA_DONE       0x80
#define ADCFLAG_LADC_PWRUP      0x40
#define ADCFLAG_LAGC_DETECT     0x20
#define ADCFLAG_LAGC_SATURATE   0x10

#define ADCFLAG_RPGA_DONE       0x08
#define ADCFLAG_RADC_PWRUP      0x04
#define ADCFLAG_RAGC_DETECT     0x02
#define ADCFLAG_RAGC_SATURATE   0x01

// AIC32_OUTPWR
#define OUTPWR_LDAC_PWRDOWN     0x00
#define OUTPWR_LDAC_PWRUP       0x80

#define OUTPWR_RDAC_PWRDOWN     0x00
#define OUTPWR_RDAC_PWRUP       0x40

#define OUTPWR_HPLCOM_DIFF      0x00
#define OUTPWR_HPLCOM_VCM       0x10
#define OUTPWR_HPLCOM_SINGLE    0x20

// AIC32_OUTDRIVE
#define OUTDRIVE_HPRCOM_DIFF    0x00
#define OUTDRIVE_HPRCOM_VCM     0x08
#define OUTDRIVE_HPRCOM_SINGLE  0x10
#define OUTDRIVE_HPRCOM_HPLCOM  0x18
#define OUTDRIVE_HPRCOM_FDBK    0x20

#define OUTDRIVE_SCPC_DISABLE   0x00
#define OUTDRIVE_SCPC_ENABLE    0x04

#define OUTDRIVE_SCPM_LIMITMX   0x00
#define OUTDRIVE_SCPM_PWRDOWN   0x02

// AIC32_OUTSTAGE
#define OUTSTAGE_COMM_1P35V     0x00
#define OUTSTAGE_COMM_1P50V     0x40
#define OUTSTAGE_COMM_1P65V     0x80
#define OUTSTAGE_COMM_1P80V     0xC0
#define OUTSTAGE_COMM_MASK      0xC0

#define OUTSTAGE_L2LBP_DISABLE  0x00
#define OUTSTAGE_L2LBP_L2LP     0x10
//#define   OUTSTAGE_L2LBP_L2LM     0x20
//#define   OUTSTAGE_L2LBP_DIFF     0x30

#define OUTSTAGE_L2RBP_DISABLE  0x00
#define OUTSTAGE_L2RBP_L2RP     0x04
//#define   OUTSTAGE_L2RBP_L2RM     0x08
//#define   OUTSTAGE_L2RBP_DIFF     0x0C

#define OUTSTAGE_SS_DISABLE     0x02
#define OUTSTAGE_SS_1FS         0x00
#define OUTSTAGE_SS_2FS         0x01

// AIC32_DACOUT
#define DACOUT_LDAC_L1          0x00
#define DACOUT_LDAC_L3          0x40
#define DACOUT_LDAC_L2          0x80

#define DACOUT_RDAC_L1          0x00
#define DACOUT_RDAC_L3          0x10
#define DACOUT_RDAC_L2          0x20

#define DACOUT_DVC_INDEPENDENT  0x00
#define DACOUT_DVC_RCONTROL     0x01
#define DACOUT_DVC_LCOMTROL     0x02

// AIC32_OUTPOP
#define OUTPOP_DELAY_0us        0x00
#define OUTPOP_DELAY_10us       0x10
#define OUTPOP_DELAY_100us      0x20
#define OUTPOP_DELAY_1ms        0x30
#define OUTPOP_DELAY_10ms       0x40
#define OUTPOP_DELAY_50ms       0x50
#define OUTPOP_DELAY_100ms      0x60
#define OUTPOP_DELAY_200ms      0x70
#define OUTPOP_DELAY_400ms      0x80
#define OUTPOP_DELAY_800ms      0x90
#define OUTPOP_DELAY_2s         0xA0
#define OUTPOP_DELAY_4s         0xB0
#define OUTPOP_DELAY_MASK       0xF0

#define OUTPOP_RAMP_0ms         0x00
#define OUTPOP_RAMP_1ms         0x04
#define OUTPOP_RAMP_2ms         0x08
#define OUTPOP_RAMP_4ms         0x0C

#define OUTPOP_WCOMM_AVDD       0x00
#define OUTPOP_WCOMM_BANDGAP    0x02

// AIC32_DACL/RGAIN
#define DACGAIN_ACTIVE          0x00
#define DACGAIN_MUTE            0x80

#define DACGAIN_GAIN_MASK       0x7F

// AIC32 output routine/connection
#define OUTPUT_NOT_CONNECTED    0x00
#define OUTPUT_CONNECTED        0x80

#define OUTPUT_VOLUME_MASK      0x7F

// AIC32_xxx_LEVEL
#define HP_LEVEL_0DB            0x00
#define HP_LEVEL_1DB            0x10
#define HP_LEVEL_2DB            0x20
#define HP_LEVEL_3DB            0x30
#define HP_LEVEL_4DB            0x40
#define HP_LEVEL_5DB            0x50
#define HP_LEVEL_6DB            0x60
#define HP_LEVEL_7DB            0x70
#define HP_LEVEL_8DB            0x80
#define HP_LEVEL_9DB            0x90
#define HP_LEVEL_MASK           0xF0

#define HP_OUTPUT_MUTE          0x00
#define HP_OUTPUT_NOTMUTE       0x08

#define HP_PWDN_WEAKLY_DR       0x00
#define HP_PWDN_TRI_STATE       0x04

#define HP_OUT_GAIN_NOTDONE     0x02

#define HP_OUT_POWER_UP         0x01
#define HP_OUT_POWER_DOWN       0x00

// AIC32_PWRSTATUS
#define PWRSTATUS_LDAC_PWUP     0x80
#define PWRSTATUS_RDAC_PWUP     0x40
#define PWRSTATUS_MONOLO_PWUP   0x20
#define PWRSTATUS_LMONO_PWUP    0x10
#define PWRSTATUS_RMONO_PWUP    0x08
#define PWRSTATUS_HPLO_PWUP     0x04
#define PWRSTATUS_HPRO_PWUP     0x02

// AIC32_SCPSTATUS
#define SCPSTATUS_HPLO_SCD      0x80
#define SCPSTATUS_HPRO_SCD      0x40
#define SCPSTATUS_HPLC_SCD      0x20
#define SCPSTATUS_HPRC_SCD      0x10

#define SCPSTATUS_HPLC_PWUP     0x08
#define SCPSTATUS_HPRC_PWUP     0x04

// AIC32_GPIO1/2
#define GPIO1_MODE_DISABLED     0x00
#define GPIO1_MODE_ADCWCLK      0x10
#define GPIO1_MODE_CLKMUXP1     0x20
#define GPIO1_MODE_CLKMUXP2     0x30
#define GPIO1_MODE_CLKMUXP4     0x40
#define GPIO1_MODE_CLKMUXP8     0x50
#define GPIO1_MODE_SCPINTRF     0x60
#define GPIO1_MODE_AGCNOISEF    0x70
#define GPIO1_MODE_GPI          0x80
#define GPIO1_MODE_GPO          0x90
#define GPIO1_MODE_DMICCLKO     0xA0
#define GPIO1_MODE_WCLKI        0xB0
#define GPIO1_MODE_BUTTINTR     0xC0
#define GPIO1_MODE_HEDETINTR    0xD0

#define GPIO2_MODE_DISABLED     0x00
#define GPIO2_MODE_PLLI         0x10
#define GPIO2_MODE_DEDETINTR    0x20
#define GPIO2_MODE_GPI          0x30
#define GPIO2_MODE_GPO          0x40
#define GPIO2_MODE_DMICI_R      0x50
#define GPIO2_MODE_DMICI_F      0x60
#define GPIO2_MODE_DMICI_RF     0x70
#define GPIO2_MODE_BCLKI        0x80

#define GPIO_MODE_MASK          0xF0

#define GPIO_CLKMUX_PLLO        0x00
#define GPIO_CLKMUX_DIVO        0x08

#define GPIO_GPI_LOW            0x00
#define GPIO_GPI_HIGH           0x02

#define GPIO_GPO_LOW            0x00
#define GPIO_GPO_HIGH           0x01

// AIC32_GPIOa
#define GPIOa_SDAMODE_SDA       0x00
#define GPIOa_SDAMODE_GPI       0x40
#define GPIOa_SDAMODE_GPO       0x80

#define GPIOa_SDA_GPO_LOW       0x00
#define GPIOa_SDA_GPO_HIGH      0x20
#define GPIOa_SDA_GPI_LOW       0x00
#define GPIOa_SDA_GPI_HIGH      0x10

#define GPIOa_SCLMODE_SCL       0x00
#define GPIOa_SCLMODE_GPI       0x04
#define GPIOa_SCLMODE_GPO       0x08

#define GPIOa_SCL_GPO_LOW       0x00
#define GPIOa_SCL_GPO_HIGH      0x02
#define GPIOa_SCL_GPI_LOW       0x00
#define GPIOa_SCL_GPI_HIGH      0x01

// AIC32_GPIOb
#define GPIOb_MFP0_I2CADD0      0x80
#define GPIOb_MFP1_I2CADD1      0x40

#define GPIOb_MFP3_GPI_DIS      0x00
#define GPIOb_MFP3_GPI_ENA      0x20
#define GPIOb_MFP3_SDI_DIS      0x00
#define GPIOb_MFP3_SDI_ENA      0x10
#define GPIOb_MFP3_GPI_LOW      0x00
#define GPIOb_MFP3_GPI_HIGH     0x08

#define GPIOb_MFP2_GPO_DIS      0x00
#define GPIOb_MFP2_GPO_ENA      0x04
#define GPIOb_MFP2_GPO_LOW      0x00
#define GPIOb_MFP2_GPO_HIGH     0x02

#define CLKGEN_CLKDIV_IN_MCLK   0x00
#define CLKGEN_CLKDIV_IN_GPIO2  0x40
#define CLKGEN_CLKDIV_IN_BCLK   0x80

#define CLKGEN_PLLCLK_IN_MCLK   0x00
#define CLKGEN_PLLCLK_IN_GPIO2  0x08
#define CLKGEN_PLLCLK_IN_BCLK   0x10

#define CLKGEN_PLL_DIV_N_16     0x00
#define CLKGEN_PLL_DIV_N_17     0x01
#define CLKGEN_PLL_DIV_N_2      0x02
#define CLKGEN_PLL_DIV_N_3      0x03
#define CLKGEN_PLL_DIV_N_4      0x04
#define CLKGEN_PLL_DIV_N_5      0x05
#define CLKGEN_PLL_DIV_N_6      0x06
#define CLKGEN_PLL_DIV_N_7      0x07
#define CLKGEN_PLL_DIV_N_8      0x08
#define CLKGEN_PLL_DIV_N_9      0x09
#define CLKGEN_PLL_DIV_N_10     0x0A
#define CLKGEN_PLL_DIV_N_11     0x0B
#define CLKGEN_PLL_DIV_N_12     0x0C
#define CLKGEN_PLL_DIV_N_13     0x0D
#define CLKGEN_PLL_DIV_N_14     0x0E
#define CLKGEN_PLL_DIV_N_15     0x0F


/*******
// Page1: Digital Filter Coefficients Registers 
*******/
//
// 3dB Bass Boost Filter
#define AEF_3DBBASS_N0MSB       0x7F    // N0=32767
#define AEF_3DBBASS_N0LSB       0xFF
#define AEF_3DBBASS_N1MSB       0x82    // N1=-32013
#define AEF_3DBBASS_N1LSB       0xF3
#define AEF_3DBBASS_N2MSB       0x7A    // N2=31275
#define AEF_3DBBASS_N2LSB       0x2B
#define AEF_3DBBASS_N3MSB       0x7F    // N3=32766
#define AEF_3DBBASS_N3LSB       0xFE
#define AEF_3DBBASS_N4MSB       0x82    // N4=-32013
#define AEF_3DBBASS_N4LSB       0xF3
#define AEF_3DBBASS_N5MSB       0x7A    // N5=31275
#define AEF_3DBBASS_N5LSB       0x2B

#define AEF_3DBBASS_D1MSB       0x7D    // D1=32076
#define AEF_3DBBASS_D1LSB       0x4C
#define AEF_3DBBASS_D2MSB       0x85    // D2=-31397
#define AEF_3DBBASS_D2LSB       0x5B
#define AEF_3DBBASS_D4MSB       0x7D    // D4=32076
#define AEF_3DBBASS_D4LSB       0x4C
#define AEF_3DBBASS_D5MSB       0x85    // D5=-31397
#define AEF_3DBBASS_D5LSB       0x5B

//
// 3dB Middle Boost Filter
#define AEF_3DBMID_N0MSB        0x7F    // N0=32767
#define AEF_3DBMID_N0LSB        0xFF
#define AEF_3DBMID_N1MSB        0xD0    // N1=-12143
#define AEF_3DBMID_N1LSB        0x91
#define AEF_3DBMID_N2MSB        0xE2    // N2=-7626
#define AEF_3DBMID_N2LSB        0x36
#define AEF_3DBMID_N3MSB        0x00    // N3=0
#define AEF_3DBMID_N3LSB        0x00
#define AEF_3DBMID_N4MSB        0x4D    // N4=19900
#define AEF_3DBMID_N4LSB        0xBC
#define AEF_3DBMID_N5MSB        0x00    // N5=0
#define AEF_3DBMID_N5LSB        0x00

#define AEF_3DBMID_D1MSB        0x39    // D1=14757
#define AEF_3DBMID_D1LSB        0xA5
#define AEF_3DBMID_D2MSB        0x08    // D2=2212
#define AEF_3DBMID_D2LSB        0xA4
#define AEF_3DBMID_D4MSB        0x00    // D4=0
#define AEF_3DBMID_D4LSB        0x00
#define AEF_3DBMID_D5MSB        0x00    // D5=0
#define AEF_3DBMID_D5LSB        0x00

//
// 3dB Treble Boost Filter
#define AEF_3DBTREBLE_N0MSB     0x7F    // N0=32767
#define AEF_3DBTREBLE_N0LSB     0xFF
#define AEF_3DBTREBLE_N1MSB     0xFD    // N1=-547
#define AEF_3DBTREBLE_N1LSB     0xDD
#define AEF_3DBTREBLE_N2MSB     0x00    // N2=9
#define AEF_3DBTREBLE_N2LSB     0x09
#define AEF_3DBTREBLE_N3MSB     0x00    // N3=0
#define AEF_3DBTREBLE_N3LSB     0x00
#define AEF_3DBTREBLE_N4MSB     0x4B    // N4=19360
#define AEF_3DBTREBLE_N4LSB     0xA0
#define AEF_3DBTREBLE_N5MSB     0x00    // N5=0
#define AEF_3DBTREBLE_N5LSB     0x05

#define AEF_3DBTREBLE_D1MSB     0xF7    // D1=-2279
#define AEF_3DBTREBLE_D1LSB     0x19
#define AEF_3DBTREBLE_D2MSB     0xFF    // D2=-158
#define AEF_3DBTREBLE_D2LSB     0x62
#define AEF_3DBTREBLE_D4MSB     0x00    // D4=0
#define AEF_3DBTREBLE_D4LSB     0x00
#define AEF_3DBTREBLE_D5MSB     0x00    // D5=0
#define AEF_3DBTREBLE_D5LSB     0x00

//
// 32-kHZ De-emphasis Coefficients
#define DEF_32K_N0MSB           0x42    // N0=16950
#define DEF_32K_N0LSB           0x36
#define DEF_32K_N1MSB           0xFB    // N1=-1220
#define DEF_32K_N1LSB           0x3C

#define DEF_32K_D1MSB           0x42    // D1=17037
#define DEF_32K_D1LSB           0x8D

//
// 44.1-kHZ De-emphasis Coefficients
#define DEF_44P1K_N0MSB         0x3A    // N0=15091
#define DEF_44P1K_N0LSB         0xF3
#define DEF_44P1K_N1MSB         0xF4    // N1=-2877
#define DEF_44P1K_N1LSB         0xC3

#define DEF_44P1K_D1MSB         0x50    // D1=20555
#define DEF_44P1K_D1LSB         0x4B

//
// 48-kHZ De-emphasis Coefficients
#define DEF_48K_N0MSB           0x39    // N0=14677
#define DEF_48K_N0LSB           0x55
#define DEF_48K_N1MSB           0xF3    // N1=-3283
#define DEF_48K_N1LSB           0x2D

#define DEF_48K_D1MSB           0x53    // D1=21374
#define DEF_48K_D1LSB           0x7E

//
//--------------------------------------------------------------
// AIC32 Initialization Value Definitions
//--------------------------------------------------------------
//

//
// Digital Features

// Sample rates for ADC and DAC = FSref = 44.1K/1 = fs=44.1K
#define RATE_INIT_VALUE     (RATE_ADC_FS1 | RATE_DAC_FS1)           // 0x00

// Using 12Mhz System Clock and to achieve 44.1Khz we need
// P = 1, R = 1, K = 7.5264=> J = 7 D = 5264
#define PLLa_INIT_VALUE     (PLL_ENABLE | PLL_Q_2 | PLL_P_1)        // P = 1
#define PLLb_INIT_VALUE     (7 << 2)                                // J = 7
#define PLLc_INIT_VALUE     ((5264 & 0x3FC0) >> 6)                  // D = 5264 8 most significant bits
#define PLLd_INIT_VALUE     ((5264 & 0x003F) << 2)                  // 6 least significant bits
#define OVERFLOW_INIT_VALUE (0x01)                                  // R=1

// Using 12Mhz System Clock and to achieve FSref=48Khz we need
// P=1, R=1, J=8 D=1920
#define PLLb_12MHzFSref48k_VALUE  (8 << 2)                          // J = 8
#define PLLc_12MHzFSref48k_VALUE  ((1920 & 0x3FC0) >> 6)            // D = 1920 8 most significant bits
#define PLLd_12MHzFSref48k_VALUE  ((1920 & 0x003F) << 2)            // 6 least significant bits

// Configure FSref as 44.1K; Left DAC plays left chnl & right DAC plays right.
#define DATAPATH_INIT_VALUE (DATAPATH_FSREF_44k | \
                             DATAPATH_ADC_NODR  | \
                             DATAPATH_DAC_NODR  | \
                             DATAPATH_L_LEFT    | \
                             DATAPATH_R_RGHT    )                   // 0x8A
// Configure FSref as 48K; Left DAC plays left chnl & right DAC plays right.
#define DATAPATH_FSref48k_VALUE (DATAPATH_FSREF_48k | \
                                 DATAPATH_ADC_NODR  | \
                                 DATAPATH_DAC_NODR  | \
                                 DATAPATH_L_LEFT    | \
                                 DATAPATH_R_RGHT    )               // 0x0A

// I2S interface is slave, at I2S and 16-bit mode
#define INTERFa_INIT_VALUE  (INTERF_BCLK_IN    | \
                             INTERF_WCLK_IN    | \
                             INTERF_DOUT_NOTRI | \
                             INTERF_3D_DISABLE )
                                            
#define INTERFb_INIT_VALUE  (INTERF_DMODE_I2S | INTERF_WLEN_16)     // 0x00
#define INTERFc_INIT_VALUE   0                                      // 0x00

// Digital HP filter, and Out Effect/Deemp filters all OFF
#define DIGFILT_INIT_VALUE  (DIGFILT_LADC_HPF_NO    | \
                             DIGFILT_RADC_HPF_NO    | \
                             DIGFILT_LDAC_BOOST_OFF | \
                             DIGFILT_LDAC_DEEMP_OFF | \
                             DIGFILT_RDAC_BOOST_OFF | \
                             DIGFILT_RDAC_DEEMP_OFF )               // 0x00

// Headset Detect Disabled and Output Driver at Capless mode
#define HEDETa_INIT_VALUE   (HEDET_DISABLE       | \
                             HEDET_JACK_DEB_32ms | \
                             HEDET_BUTT_DEB_8ms  )                  // 0x05
#define HEDETb_INIT_VALUE   (HEDET_DRV_CAPCOUPLED | \
                             HEDET_FULL_DIFF_NO  | \
                             HEDET_PSEUDO_DIFF_NO)                  // 0x00

//
// Analog Input Features

// ADC PGA muted and 0dB
#define ADCPGAL_INIT_VALUE  (ADCPGA_VOLUME_MUTE)                    // 0x80
#define ADCPGAR_INIT_VALUE  (ADCPGA_VOLUME_MUTE)                    // 0x80

// MIC1L/R or Line1L/R not connected 
#define L1L_ADCL_INIT_VALUE (LINE_NOT_TO_ADC) 
#define L1R_ADCR_INIT_VALUE (LINE_NOT_TO_ADC)
#define L1R_ADCL_INIT_VALUE (LINE_NOT_TO_ADC) 
#define L1L_ADCR_INIT_VALUE (LINE_NOT_TO_ADC)

// MIC2L/R or Line2L/R NOT connected to ADCs
#define L2L_ADCL_INIT_VALUE (LINE_NOT_TO_ADC) 
#define L2R_ADCR_INIT_VALUE (LINE_NOT_TO_ADC) 

// MIC3L/R or Line3L/R not connected 
#define MIC3_ADCL_INIT_VALUE (MIC3L_NOT_TO_ADC | MIC3R_NOT_TO_ADC) // 0xFF
#define MIC3_ADCR_INIT_VALUE (MIC3L_NOT_TO_ADC | MIC3R_NOT_TO_ADC) // 0xFF
#define MICBIAS_INIT_VALUE   (MICBIAS_POWER_2P5V)                    // 0x80

//
// Analog Output Features

// Both HPL/RCOM are consVCM; short circuit protecton for high pwr out enabled;
// Output VCM at +1.5V; line2 left bypass uses full differential;
// Output pop reduction at 1ms delay and 1ms ramp-up step; Do NOT enable DAC yet
#define OUTPWR_INIT_VALUE   (OUTPWR_LDAC_PWRUP | \
                             OUTPWR_RDAC_PWRUP | \
                             OUTPWR_HPLCOM_SINGLE )                 // 0xD0
#define OUTDRIVE_INIT_VALUE (OUTDRIVE_HPRCOM_DIFF  | \
                             OUTDRIVE_SCPC_ENABLE | \
                             OUTDRIVE_SCPM_LIMITMX)                 // 0x0C
#define OUTSTAGE_INIT_VALUE (OUTSTAGE_COMM_1P35V    | \
                             OUTSTAGE_L2LBP_L2LP    | \
                             OUTSTAGE_L2RBP_L2RP | \
                             OUTSTAGE_SS_1FS        )               // 0x30
#define OUTPOP_INIT_VALUE   (OUTPOP_DELAY_100ms | \
                             OUTPOP_RAMP_4ms  | \
                             OUTPOP_WCOMM_AVDD)                     // 0x6C

// DAC Left and Right at 0dB and muted for now
#define DACLGAIN_INIT_VALUE (DACGAIN_MUTE)                          // 0x80
#define DACRGAIN_INIT_VALUE (DACGAIN_MUTE)                          // 0x80

// DAC left to HPL and DAC right to HPR, but mute HPL/R now
#define DACL_HPL_INIT_VALUE (OUTPUT_CONNECTED)                      // 0x80
#define HPLLEVEL_INIT_VALUE (HP_LEVEL_0DB      | \
                             HP_OUTPUT_MUTE    | \
                             HP_PWDN_TRI_STATE | \
                             HP_OUT_POWER_DOWN )                    // 0x04
#define DACR_HPR_INIT_VALUE (OUTPUT_CONNECTED)                      // 0x80
#define HPRLEVEL_INIT_VALUE (HP_LEVEL_0DB      | \
                             HP_OUTPUT_MUTE    | \
                             HP_PWDN_TRI_STATE | \
                             HP_OUT_POWER_DOWN )                    // 0x04

// Line2 left is routed to MONO with 0dB volume; unmuted with 0dB 
#define L2L_MONO_INIT_VALUE  (OUTPUT_CONNECTED)                     // 0x80
#define MONOLEVEL_INIT_VALUE (HP_LEVEL_0DB      | \
                              HP_OUTPUT_NOTMUTE | \
                              HP_OUT_POWER_UP   )                   // 0x09

//
// GPIO Features - Not present in AIC32

// GPIO1 is used as PLL output (if PLL enabled)
//#define GPIO1_INIT_VALUE  (GPIO1_MODE_CLKMUXP1 | GPIO_CLKMUX_PLLO) // 0x20
//#define GPIO2_INIT_VALUE  (GPIO2_MODE_GPO | GPIO_GPO_HIGH)         // 0x41

//
// Clock Features
#define CLKGEN_INIT_VALUE   (CLKGEN_CLKDIV_IN_MCLK | \
                             CLKGEN_PLLCLK_IN_MCLK | \
                             CLKGEN_PLL_DIV_N_2   )                 // 0x02

unsigned int IIC_READ_AIC32(unsigned int count, unsigned int * pBuffer); 

