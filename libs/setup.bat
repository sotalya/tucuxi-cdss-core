@ECHO off
SETLOCAL 

SET scriptpath=%~dp0

REM Check if the number of argument is correct
IF "%1" EQU "" (
  ECHO No argument given!! Defaulting to building all!
  SET build_config="all"
)
IF "%2" NEQ "" (
  ECHO Too many arguments given!!
  ECHO Rerun build.bat with only one of the following arguments
  ECHO    release : To build the library in release mode
  ECHO    debug   : To build the library in debug mode
  ECHO    all     : To build the library in both release and debug mode
  GOTO exit
)


CALL "%scriptpath%/tucuxi-core/setup.bat" %1

:exit
ENDLOCAL 
