#!/usr/bin/env bash
# Version: 3.1
# Date: 2024-04-17
# This bash script generates a CMSIS Software Pack:
#

set -o pipefail

# Set version of gen pack library
# For available versions see https://github.com/Open-CMSIS-Pack/gen-pack/tags.
# Use the tag name without the prefix "v", e.g., 0.7.0
REQUIRED_GEN_PACK_LIB="0.14.0"

# Set default command line arguments
DEFAULT_ARGS=(-c "cmsis_mbedtls_")

# Pack warehouse directory - destination
# Default: ./output
#
# PACK_OUTPUT=./output

# Temporary pack build directory,
# Default: ./build
#
# PACK_BUILD=./build

# Specify directory names to be added to pack base directory
# An empty list defaults to all folders next to this script.
# Default: empty (all folders)
#
PACK_DIRS="
  configs
  docs
  include
  library
  MDK
  programs
  tf-psa-crypto/configs
  tf-psa-crypto/core
  tf-psa-crypto/dispatch
  tf-psa-crypto/docs
  tf-psa-crypto/drivers/builtin
  tf-psa-crypto/extras
  tf-psa-crypto/include
  tf-psa-crypto/platform
  tf-psa-crypto/programs
  tf-psa-crypto/utilities
"

# Specify file names to be added to pack base directory
# Default: empty
#
PACK_BASE_FILES="
  ChangeLog
  dco.txt
  LICENSE
  tf-psa-crypto/ChangeLog
  tf-psa-crypto/LICENSE
"

# Specify file names to be deleted from pack build directory
# Default: empty
#
PACK_DELETE_FILES="
  **/Makefile
  **/CMakeLists.txt
  tf-psa-crypto/core/crypto-library.make
  tf-psa-crypto/programs/crypto-programs.make
"

# Specify patches to be applied
# Default: empty
#
# PACK_PATCH_FILES="
#     <list patches here>
# "

# Specify addition argument to packchk
# Default: empty
#
PACKCHK_ARGS=(-x M317,M395)

# Specify additional dependencies for packchk
# Default: empty
#
PACKCHK_DEPS="
  ARM.CMSIS.pdsc
  ARM.CMSIS-RTX.pdsc
  Keil.MDK-Middleware.pdsc
  MDK-Packs.IoT_Socket.pdsc
"

# Optional: restrict fallback modes for changelog generation
# Default: full
# Values:
# - full      Tag annotations, release descriptions, or commit messages (in order)
# - release   Tag annotations, or release descriptions (in order)
# - tag       Tag annotations only
#
PACK_CHANGELOG_MODE="tag"

# Specify file patterns to be excluded from the checksum file
# Default: <empty>
# Values:
# - empty          All files packaged are included in the checksum file
# - glob pattern   One glob pattern per line. Files matching a given pattern are excluded
#                  from the checksum file
# - "*"            The * (match all pattern) can be used to skip checksum file creating completely.
# 
# PACK_CHECKSUM_EXCLUDE="
#   <list file patterns here>
# "

#
# custom pre-processing steps
#
# usage: preprocess <build>
#   <build>  The build folder
#
function preprocess() {
  # add custom steps here to be executed
  # before populating the pack build folder

  find_ghcli

  VERSION=$(sed -n 's/.*Cclass="Security".*Cgroup="mbed TLS".*Cversion="\([^"]*\)".*/\1/p' ARM.mbedTLS.pdsc)
  echo "Fetching mbedtls sources from upstream '${VERSION}' ..."
  "${UTILITY_GHCLI}" release download "mbedtls-${VERSION}" -p mbedtls-${VERSION}.tar.bz2 --repo Mbed-TLS/mbedtls

  mbedtls_extract_dirs=(
    "mbedtls-${VERSION}/library"
    "mbedtls-${VERSION}/programs/test"
    "mbedtls-${VERSION}/tf-psa-crypto/core"
    "mbedtls-${VERSION}/tf-psa-crypto/programs/psa"
    "mbedtls-${VERSION}/tests/include"
    "mbedtls-${VERSION}/tests/src"
  )

  echo "Extracting mbedtls sources ..."
  "${UTILITY_ZIP}" x mbedtls-${VERSION}.tar.bz2 > /dev/null
  "${UTILITY_ZIP}" x mbedtls-${VERSION}.tar "${mbedtls_extract_dirs[@]}" > /dev/null

  echo "Copy generated mbedtls source files ..."

  mbed_files=$(sed -n '
    /^GENERATED_FILES[[:space:]]*:=/{
      :a
      n
      /^[[:space:]]*\$(TF_PSA_CRYPTO_LIBRARY_GENERATED_FILES)$/q
      /\\$/{
        s/[[:space:]]*\\$//
        p
        ba
      }
      p
    }
  ' mbedtls-${VERSION}/library/Makefile)

  crypto_files=$(sed -n '
    /^TF_PSA_CRYPTO_LIBRARY_GENERATED_FILES[[:space:]]*:=/{
      :a
      n
      /\\$/{
        s/[[:space:]]*\\$//
        s|\$(TF_PSA_CRYPTO_CORE_PATH)/||
        p
        ba
      }
      s|\$(TF_PSA_CRYPTO_CORE_PATH)/||
      p
      q
    }
  ' mbedtls-${VERSION}/tf-psa-crypto/core/crypto-library.make)

  mkdir -p "$1/library"
  pushd mbedtls-${VERSION}/library > /dev/null
  cp $mbed_files "$1/library"
  popd > /dev/null

  mkdir -p "$1/tf-psa-crypto/core"
  pushd mbedtls-${VERSION}/tf-psa-crypto/core > /dev/null
  cp $crypto_files "$1/tf-psa-crypto/core"
  popd > /dev/null

  mkdir -p "$1/programs/test"
  cp mbedtls-${VERSION}/programs/test/query_config.c "$1/programs/test/"

  mkdir -p "$1/tf-psa-crypto/programs/psa"
  cp mbedtls-${VERSION}/tf-psa-crypto/programs/psa/psa_constant_names_generated.c "$1/tf-psa-crypto/programs/psa/"

  mkdir -p "$1/MDK/examples/tests/include/test"
  cp mbedtls-${VERSION}/tests/include/test/certs.h "$1/MDK/examples/tests/include/test/"
  cp mbedtls-${VERSION}/tests/include/test/test_certs.h "$1/MDK/examples/tests/include/test/"

  mkdir -p "$1/MDK/examples/tests/src"
  cp mbedtls-${VERSION}/tests/src/certs.c "$1/MDK/examples/tests/src/"

  echo "Deleting mbedtls archives and sources ..."
  rm mbedtls-${VERSION}.* -r mbedtls-${VERSION}

  # Generate documentation
  # ./gen_doc.sh

  return 0
}

#
# custom post-processing steps
#
# usage: postprocess <build>
#   <build>  The build folder
#
function postprocess() {
  # add custom steps here to be executed
  # after populating the pack build folder
  # but before archiving the pack into output folder
  rm -rf ./apidoc
  cp -f LICENSE "$1/tf-psa-crypto/"
  find ./ -type d -name "__pycache__" -exec rm -rf {} +
  return 0
}

############ DO NOT EDIT BELOW ###########

# Set GEN_PACK_LIB_PATH to use a specific gen-pack library root
# ... instead of bootstrap based on REQUIRED_GEN_PACK_LIB
if [[ -n "${GEN_PACK_LIB_PATH}" ]] && [[ -f "${GEN_PACK_LIB_PATH}/gen-pack" ]]; then
  . "${GEN_PACK_LIB_PATH}/gen-pack"
else
  . <(curl -sL "https://raw.githubusercontent.com/Open-CMSIS-Pack/gen-pack/main/bootstrap")
fi

gen_pack "${DEFAULT_ARGS[@]}" "$@"

exit 0
