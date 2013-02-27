Build Instructions:

To build C55_usbac2 in CCS 3.3, please follow the steps below: 

1. Unzip the c55_usbac2_yyyymmdd_v00_nn_00_00.zip to c:\c55_usbac2_yyyymmdd_v00_nn_00_00
2. Launch CCS 3.3
3. Open project c:\c55_usbac2_2012mmdd_v00_nn_00_00_mod\build\CSL_USB_IsoHighSpeedExample_Out.pjt using "Project" --> "Open..."
4. Make the CSL_USB_IsoHighSpeedExample_Out.pjt to be the active project
5. Change the current profile to Release
6. Build the project using "Project" --> "Build"
7. The Built OUT file CSL_USB_IsoHighSpeedExample_Out.out will be in c:\c55_usbac2_yyyymmdd_v00_nn_00_00_mod\build\Release
8. Use the CCS 3.3 to load the CSL_USB_IsoHighSpeedExample_Out.out onto the C5515 EVM 

To build C55_usbac2 in CCS 4.x, please follow the steps below: 

1. Unzip the c55_usbac2_yyyymmdd_v00_nn_00_00.zip to c:\c55_usbac2_yyyymmdd_v00_nn_00_00
2. Launch CCS 4.x 
3. Choose the c:\c55_usbac2_yyyymmdd_v00_nn_00_00 as your Workspace 
4. Import the projects by using "Project" --> "Import Existing CCS/CCE Eclipse Proejct".
5. Choose c:\c55_usbac2_yyyymmdd_v00_nn_00_00 for "Select search-directory" 
6. Import all the proejcts (default)
7. Set the CSL_USB_IsoHighSpeedExample_Out as your active project by right click on the project then select "Set as Active Project"
8. Change Active Build Configuration to "Release" by by right click on the project then select "Active Build Configuration" --> "Release"
9. Build the project by using "Project" --> "Build Active Project"
10. The Built OUT file CSL_USB_IsoHighSpeedExample_Out.out will be in c:\c55_usbac2_yyyymmdd_v00_nn_00_00_mod\build\Release
11. Use the CCS 4.x to load the CSL_USB_IsoHighSpeedExample_Out.out onto the C5515 EVM 

Build Options:

Bit Per Sample: SAMP_24BIT (not defined means 16 bit sample)
Number of Audio Codecs: USE_FOUR_CODEC (8 channel IN and 8 channel OUT), USE_THREE_CODEC (6 channel IN and 6 channel OUT), 
USE_TWO_CODEC(4 channel IN and 4 channel OUT), Not defined means 2 channel IN and 2 channel OUT

The above build options have to be changed in all the follwoing projects at the same time:
CSL_USB_IsoHighSpeedExample_Out.pjt
cslVC5505.pjt
dma_bios_drv_lib.pjt
i2s_bios_drv_lib.pjt

To change the development platform, you will need to define the following compiler option:
C5535_EZDSP in CSL_USB_IsoHighSpeedExample_Out.pjt will generate executable for C5535 eZdsp. 
If C5535_EZDSP is not defined then the generated executable is for C5515 EVM
