@echo off
setlocal

rem Define a label for error handling
:Error

call Third-Party\premake\premake5.exe vs2022

rem Check the error code of the last command
if %errorlevel% neq 0 (
    echo The error occurred in the file or directory: %CD%
)

pause

endlocal
