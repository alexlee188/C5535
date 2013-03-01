@echo off
pushd ..\..\
setlocal

:process_arg
if "%1"=="" goto end_process_arg
set name=%1
set value=

:process_arg_value
if NOT "%value%"=="" set value=%value% %2
if "%value%"=="" set value=%2
shift
if "%2"=="!" goto set_arg
if "%2"=="" goto set_arg
goto process_arg_value

:set_arg
set %name%=%value%
shift
shift
goto process_arg
:end_process_arg

echo. > temp_postBuildStep_Release.bat

echo c:\Perl\ofd\ofd55 -x -o=CSL_USB_IsoHighSpeedExample_Out\Release\CSL_USB_IsoHighSpeedExample_Out.xml CSL_USB_IsoHighSpeedExample_Out\Release\CSL_USB_IsoHighSpeedExample_Out.out >> temp_postBuildStep_Release.bat
echo perl -S c:\Perl\ofd\sectti.pl CSL_USB_IsoHighSpeedExample_Out\Release\CSL_USB_IsoHighSpeedExample_Out.xml >> temp_postBuildStep_Release.bat

call temp_postBuildStep_Release.bat
del temp_postBuildStep_Release.bat

endlocal
popd
