@echo off
@echo "Create Doxygen template..."
if "%~1"=="" (doxygen -g) else (doxygen -g %1)
pause