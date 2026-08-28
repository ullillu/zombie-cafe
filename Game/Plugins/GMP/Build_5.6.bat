@echo off
:Rebuild
@pushd %~dp0
@pushd ..
@call "C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin -Plugin=%~dp0GMP.uplugin -Package="D:\AdditionalWorkTree\kombez-main\Game\Plugins\GMP\..\GMP5.6" -TargetPlatforms=Win64+Android -compile
if ERRORLEVEL 1 pause & goto Retry
pause
rmdir /S /Q "GMP5.6"
goto :eof
:Retry
rmdir /S /Q "GMP5.6"
goto :Rebuild
