@echo off
rem RCS: @(#) $Id: rmd.bat 30140 2008-01-29 18:23:59Z erikgreenwald $

if not exist %1\nul goto end

echo Removing directory %1

if "%OS%" == "Windows_NT" goto winnt

deltree /y %1
if errorlevel 1 goto end
goto success

:winnt
rmdir /s /q %1
if errorlevel 1 goto end

:success
echo Deleted directory %1

:end

