@echo off
rem Do not edit! This batch file is created by CASIO fx-9860G SDK.


if exist CAVEDASH.G1A  del CAVEDASH.G1A

cd debug
if exist FXADDINror.bin  del FXADDINror.bin
"D:\Programs\OS\SH\Bin\Hmake.exe" Addin.mak
cd ..
if not exist debug\FXADDINror.bin  goto error

"D:\Programs\Tools\MakeAddinHeader363.exe" "C:\Users\pinks\Documents\CASIO\fx-9860G SDK\CaveDash"
if not exist CAVEDASH.G1A  goto error
echo Build has completed.
goto end

:error
echo Build was not successful.

:end

