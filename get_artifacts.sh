#!/bin/bash
# Get generated source files from upstream mbedtls artifacts

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${repo_root}"

VERSION=$(sed -n 's/.*Cclass="Security".*Cgroup="mbed TLS".*Cversion="\([^"]*\)".*/\1/p' ARM.mbedTLS.pdsc)

echo "Fetching mbedtls sources from upstream '${VERSION}' ..."
gh release download "mbedtls-${VERSION}" -p mbedtls-${VERSION}.tar.bz2 --repo Mbed-TLS/mbedtls

mbedtls_extract_dirs=(
  "mbedtls-${VERSION}/library"
  "mbedtls-${VERSION}/programs/test"
  "mbedtls-${VERSION}/tf-psa-crypto/core"
  "mbedtls-${VERSION}/tf-psa-crypto/programs/psa"
)

echo "Extracting mbedtls sources ..."
tar -xjf "mbedtls-${VERSION}.tar.bz2" "${mbedtls_extract_dirs[@]}"

echo "Copying generated mbedtls source files ..."

mbed_library_files=(
  "mbedtls_config_check_before.h"
  "mbedtls_config_check_final.h"
  "mbedtls_config_check_user.h"
  "error.c"
  "version_features.c"
  "ssl_debug_helpers_generated.c"
)

crypto_library_files=(
  "psa_crypto_driver_wrappers.h"
  "psa_crypto_driver_wrappers_no_static.c"
  "tf_psa_crypto_config_check_before.h"
  "tf_psa_crypto_config_check_final.h"
  "tf_psa_crypto_config_check_user.h"
)

pushd mbedtls-${VERSION}/library > /dev/null
cp "${mbed_library_files[@]}" "${repo_root}/library"
popd > /dev/null

pushd mbedtls-${VERSION}/tf-psa-crypto/core > /dev/null
cp "${crypto_library_files[@]}" "${repo_root}/tf-psa-crypto/core"
popd > /dev/null

cp mbedtls-${VERSION}/programs/test/query_config.c "${repo_root}/programs/test/"

cp mbedtls-${VERSION}/tf-psa-crypto/programs/psa/psa_constant_names_generated.c "${repo_root}/tf-psa-crypto/programs/psa/"

echo "Deleting mbedtls archive and sources ..."
rm mbedtls-${VERSION}.tar.bz2  -r mbedtls-${VERSION}
