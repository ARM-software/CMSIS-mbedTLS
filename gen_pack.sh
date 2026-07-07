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

  # Get generated source files from upstream mbedtls artifacts
  ./get_artifacts.sh

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
