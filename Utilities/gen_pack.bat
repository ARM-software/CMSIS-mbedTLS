:: Batch file for generating CMSIS-mbedTLS pack
:: This batch file uses:
::    7-Zip for packaging
::    Doxygen version 1.8.2 and Mscgen version 0.20 for generating html documentation.
:: The generated pack and pdsc file are placed in folder %RELEASE_PATH% (../../Local_Release)
@ECHO off

SETLOCAL

:: Tool path for zipping tool 7-Zip
SET ZIPPATH=C:\Program Files\7-Zip

:: Tool path for doxygen
SET DOXYGENPATH=C:\Program Files\doxygen\bin

:: Tool path for mscgen utility
SET MSCGENPATH=C:\Program Files (x86)\Mscgen

:: These settings should be passed on to subprocesses as well
SET PATH=%ZIPPATH%;%DOXYGENPATH%;%MSCGENPATH%;%PATH%

:: Pack Path (where generated pack is stored)
SET RELEASE_PATH=..\Local_Release

:: !!!!!!!!!!!!!!!!!
:: DO NOT EDIT BELOW
:: !!!!!!!!!!!!!!!!! 

:: Remove previous build
IF EXIST %RELEASE_PATH% (
  ECHO removing %RELEASE_PATH%
  RMDIR /Q /S  %RELEASE_PATH%
)

:: Create build output directory
MKDIR %RELEASE_PATH%

:: Copy PDSC file
COPY ..\ARM.mbedTLS.pdsc %RELEASE_PATH%\ARM.mbedTLS.pdsc

:: Copy various root file
COPY ..\apache-2.0.txt %RELEASE_PATH%\apache-2.0.txt
COPY ..\ChangeLog %RELEASE_PATH%\ChangeLog
COPY ..\LICENSE %RELEASE_PATH%\LICENSE

:: Copy configs folder
XCOPY /Q /S /Y ..\configs\*.* %RELEASE_PATH%\configs\*.*

:: Copy include folder
XCOPY /Q /S /Y ..\include\*.* %RELEASE_PATH%\include\*.*
DEL /Q %RELEASE_PATH%\include\.gitignore
DEL /Q %RELEASE_PATH%\include\CMakeLists.txt

:: Copy library folder
XCOPY /Q /S /Y ..\library\*.* %RELEASE_PATH%\library\*.*
DEL /Q %RELEASE_PATH%\library\.gitignore
DEL /Q %RELEASE_PATH%\library\CMakeLists.txt
DEL /Q %RELEASE_PATH%\library\Makefile

:: Copy programs folder
XCOPY /Q /S /Y ..\programs\*.* %RELEASE_PATH%\programs\*.*
DEL /Q %RELEASE_PATH%\programs\.gitignore
DEL /Q %RELEASE_PATH%\programs\CMakeLists.txt
DEL /Q %RELEASE_PATH%\programs\Makefile
DEL /Q %RELEASE_PATH%\programs\wince_main.c
DEL /Q %RELEASE_PATH%\programs\aes\CMakeLists.txt
DEL /Q %RELEASE_PATH%\programs\hash\CMakeLists.txt
DEL /Q %RELEASE_PATH%\programs\pkey\CMakeLists.txt
DEL /Q %RELEASE_PATH%\programs\random\CMakeLists.txt
DEL /Q %RELEASE_PATH%\programs\ssl\CMakeLists.txt
DEL /Q %RELEASE_PATH%\programs\test\CMakeLists.txt
DEL /Q %RELEASE_PATH%\programs\util\CMakeLists.txt
DEL /Q %RELEASE_PATH%\programs\x509\CMakeLists.txt

:: Copy tests folder
XCOPY /Q /S /Y ..\tests\*.* %RELEASE_PATH%\tests\*.*
DEL /Q %RELEASE_PATH%\tests\.gitignore
DEL /Q %RELEASE_PATH%\tests\CMakeLists.txt
DEL /Q %RELEASE_PATH%\tests\Makefile
DEL /Q %RELEASE_PATH%\tests\data_files\.gitignore
DEL /Q %RELEASE_PATH%\tests\data_files\Makefile

:: Copy MDK folder
XCOPY /Q /S /Y ..\MDK\*.* %RELEASE_PATH%\MDK\*.*

:: Copy doxygen folder
XCOPY /Q /S /Y ..\doxygen\*.* %RELEASE_PATH%\doxygen\*.*

::  Build documentation
PUSHD %RELEASE_PATH%\doxygen
CALL gen_doc.bat
IF NOT "%ERRORLEVEL%"=="0" (
	POPD
	ECHO **************************************************
	ECHO *
	ECHO *  %RELEASE_PATH%\doxygen\gen_doc.bat
	ECHO *  failed, aborting %~nx0 ...                       
	ECHO *
	ECHO **************************************************
	@PAUSE
	EXIT /B 1
)
POPD

::  Remove doxygen folder after build
DEL %RELEASE_PATH%\doxygen\gen_doc.bat
DEL %RELEASE_PATH%\doxygen\mbedtls.dxy
DEL %RELEASE_PATH%\doxygen\mbedtls.doxyfile
RMDIR /Q /S %RELEASE_PATH%\doxygen\DoxyTemplates
RMDIR /Q /S %RELEASE_PATH%\doxygen\images
RMDIR /Q /S %RELEASE_PATH%\doxygen\input

:: Checking 
Win32\PackChk.exe %RELEASE_PATH%\ARM.mbedTLS.pdsc -n %RELEASE_PATH%\PackName.txt -x M324

:: --Check if PackChk.exe has completed successfully
IF %errorlevel% neq 0 GOTO ErrPackChk

:: Packing 
PUSHD %RELEASE_PATH%

:: -- Pipe Pack's Name into Variable
SET /P PackName=<PackName.txt
DEL /Q PackName.txt

:: Pack files
ECHO Creating pack file ...
7z.exe a %PackName% -tzip > zip.log
ECHO Packaging complete
POPD
GOTO End

:ErrPackChk
ECHO PackChk.exe has encountered an error!
EXIT /b

:End
ECHO Removing temporary files and folders
PUSHD %RELEASE_PATH%
FOR %%A IN (configs include library programs tests MDK doxygen) DO IF EXIST %%A (RMDIR /S /Q %%A)
DEL apache-2.0.txt
DEL ChangeLog
DEL LICENSE
DEL zip.log
POPD

ECHO gen_pack.bat completed successfully
