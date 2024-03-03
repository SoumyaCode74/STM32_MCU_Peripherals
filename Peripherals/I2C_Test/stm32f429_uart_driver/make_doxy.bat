@echo off
@echo "Build Doxygen document..."
if "%~1"=="" (doxygen Doxyfile) else (doxygen %1)
call latex\make.bat
pause