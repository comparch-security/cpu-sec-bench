@echo off
rem this script is used to get target-address's offset of current func by disassemble test-case's code section 
rem read one function's multiple labels
rem %1 is test case name
rem %2 is func name
rem %3 is target func name

IF EXIST offset.tmp del offset.tmp

dumpbin.exe /DISASM:NOBYTES %1 > file.dump

call .\script\jrepl.bat  ".*"  "T" /k 0 /I /INC "/^^%2:$/+1:/.*:$/-1" /f file.dump /o func.dump

rem read func first line
set /p start_address=<func.dump
set start_address=0x%start_address:~13,5%
 echo start_address is %start_address%
rem change it to num value
set /A "start_num=%start_address%"
 echo start num is %start_num%

rem read target line
type func.dump | findstr %3 > target.dump

for /F "tokens=*" %%T in (target.dump) do call :process %%T
goto label
:process
set  target_address=%~1
set  target_address=0x%target_address:~11,5%
 echo target address is %target_address%
rem change it to num value
set /A "target_num=%target_address%"
 echo target num is %target_num%
set /A offset=%target_num% - %start_num%
echo %offset%>> offset.tmp
EXIT /B 0

:label
rem del file.dump func.dump target.dump