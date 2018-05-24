@echo off & setlocal
setlocal enabledelayedexpansion enableextensions

for /f %%i in ('git describe --long --always') do set describe=%%i

set t=%describe%
set i=-1
:loop
for /f "tokens=1* delims=.-" %%a in ("%t%") do (
    set /a i= !i! + 1
    set values[!i!]=%%a
    set t=%%b
)
if defined t goto :loop

if !i! EQU 0 (
    echo 0.0.0
) else (
    set /a values[2]= !values[2]! + !values[3]!
    echo !values[0]!.!values[1]!.!values[2]!
)