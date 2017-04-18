/**
 * Entropy hardware poll function for MDK-Pro Network
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
 #include "mbedtls/config.h"
#else
 #include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

#include <string.h>
#include "entropy_poll.h"
#include "cmsis_os.h"

/**
 * Entropy poll callback for a hardware source
 */
int mbedtls_hardware_poll (void *data,
                           unsigned char *output, size_t len, size_t *olen) {
  uint32_t timer;

  if (len < sizeof(uint32_t)) {
    *olen = 0;
    return (0);
  }
  /* Note: This is weak entropy source */
  timer = osKernelSysTick ();
  memcpy (output, &timer, sizeof(timer));
  *olen = sizeof (timer);
  return (0);
}

#endif /* MBEDTLS_ENTROPY_HARDWARE_ALT */
