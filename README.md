# README for CMSIS-mbedTLS

## Introduction

This is a fork of the original [mbed TLS](https://github.com/ARMmbed/mbedtls). It uses the source files from the mbed TLS repository, and adds capabilities to build a [software pack](http://arm-software.github.io/CMSIS_5/Pack/html/cp_SWComponents.html) from it. It also contains the complete mbed TLS documentation that is built with Doxygen. This documentation is linked in the PDSC file so that it is easily accessible from development systems with pack support.

## Create the software pack

### Prerequisites

The pack generation flow described below is currently only working on Windows PCs. To build the pack you will need the following software:
 - [Doxygen 1.8.6](https://sourceforge.net/projects/doxygen/files/snapshots/doxygen-1.8-svn/windows/) to build the documentation
 - [Mscgen](http://www.mcternan.me.uk/mscgen/) to create sequence charts in the documentation
 - [7-ZIP](http://www.7-zip.org/download.html) to create the pack file
 - PackChk.exe that is part of this repository, available in the Utilities\\Win32 directory (no install required)
 
**Note:** If you install the tools in non-standard directories, you need to edit the gen_pack.bat file in the Utilities-directory to reflect these custom installation locations.

### Build the software pack

To build a software pack, clone the repository or download the ZIP file and save it on your local drive. Go to the directory "Utilities" and run the Windows batch file "gen_pack.bat". The resulting software pack will be available in the directory ..\Local_Release under the name **ARM.mbedTLS.X.X.X.pack**. To use it, simply double-click the pack file and it will be added to your development environment's installation.

## License

ARM CMSIS-mbedTLS is licensed under Apache-2.0.
