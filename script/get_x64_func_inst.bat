@echo off
rem Usage:   ./get_x64_func_inst.bat proc_name func_name byte_nums file_name
rem Example: ./get_x64_func_inst.bat ./test/acc-read-func helper 8 func_helper.tmp

IF EXIST %4 del %4

set byte_nums=%3

dumpbin.exe /DISASM %1 > file.dump

rem grep func dump from file.dump
rem /k means only search no replace, 0 means not add after context
rem /INC means only search and replace in it's defined context
call .\script\jrepl.bat  ".*"  "T" /k 0 /I /INC "/^^%2:$/+1:/.*:$/-1" /f file.dump /o func.dump
rem extract bytecode into bytecode.dump sequently
call .\script\jrepl.bat "[0-9A-F][0-9A-F] | {20}[0-9A-F][0-9A-F]" "T" /M /MATCH /f func.dump /o bytecode.dump
rem remove the extra blank 
call .\script\jrepl.bat "[0-9A-F][0-9A-F]" "T" /M /MATCH /f bytecode.dump /o neat_bytecode.dump
rem get the count(byte_num defined) of bytecodes
call .\script\jrepl.bat ".*" "T" /k 0 /INC "1:8" /f neat_bytecode.dump  /o final_bytecode.dump

rem little endian
set le_val=

rem read each line's bytecode and add it to final val
for /F "tokens=*" %%T in (final_bytecode.dump) do call :process %%T

goto label

:process
set le_val=%~1%le_val%
EXIT /B 0

:label
set le_val=0x%le_val%
 echo le_val is %le_val%
echo %le_val%>> %4
del file.dump func.dump bytecode.dump neat_bytecode.dump final_bytecode.dump