@echo off
rem RCS: @(#) $Id: mkd.bat 30140 2008-01-29 18:23:59Z erikgreenwald $

if exist %1\nul goto end

md %1
if errorlevel 1 goto end

echo Created directory %1

:end



