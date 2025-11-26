# README for CMSIS-mbedTLS

## Introduction

This is a fork of the original [mbed TLS](https://github.com/Mbed-TLS/mbedtls).
It uses the source files from the mbed TLS repository, and adds capabilities to build a [software pack](https://open-cmsis-pack.github.io/cmsis-toolbox/pack-tools) from it.
It also contains the complete mbed TLS documentation that is built with Doxygen. This documentation is linked in the PDSC file so that it is easily accessible from development systems with pack support.

## Create the software pack

### Prerequisites

To build the pack you will need the following software:
 - bash compatible shell (under Windows, use for example [git bash](https://gitforwindows.org/))
 - [Python](https://www.python.org/downloads/) v3.10 or later
 - [Doxygen 1.8.x](https://sourceforge.net/projects/doxygen/files/snapshots/doxygen-1.8-svn/windows/) to build the documentation
 - [Graphviz](https://graphviz.org/download/) to create graphs in the documentation
 - [7-ZIP](http://www.7-zip.org/download.html) to create the pack file
 - CMSIS Pack installed in CMSIS_PACK_ROOT (for PackChk utility)
 - xmllint in path (XML schema validation; available only for Linux)
 
### Build the software pack

To build a software pack, clone the repository or download the ZIP file and save it on your local drive. 
Open a bash shell and run `gen_doc.sh` to build the documentation and run `gen_pack.sh` to build the pack.
The resulting software pack will be available in the directory `..\output` under the name **ARM.mbedTLS.X.X.X.pack**.
To use it, simply double-click the pack file and it will be added to your development environment's installation.

## License

ARM CMSIS-mbedTLS is licensed under Apache-2.0.
