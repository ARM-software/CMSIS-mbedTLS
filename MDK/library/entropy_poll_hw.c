/*
 * Entropy hardware poll function
 *
 *  Copyright (C) 2006-2021, Arm Limited, All Rights Reserved
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
 */

#include "mbedtls/build_info.h"

#if defined(MBEDTLS_ENTROPY_HARDWARE_ALT)

#include <string.h>
#include "cmsis_compiler.h"
#include "RTE_Components.h"
#if defined(RTE_CMSIS_RTOS)
 #include "cmsis_os.h"
#elif defined(RTE_CMSIS_RTOS2)
 #include "cmsis_os2.h"
#else
 #error "::CMSIS:RTOS selection invalid"
#endif

int mbedtls_hardware_poll(void *data,
                          unsigned char *output, size_t len, size_t *olen);

/**
 * Entropy poll callback for a hardware source
 */
__WEAK int mbedtls_hardware_poll (void *data, unsigned char *output,
                                  size_t len, size_t *olen) {
  (void)data;
  uint32_t timer;

  if (len < sizeof(uint32_t)) {
    *olen = 0;
    return (0);
  }
  /* Note: This is weak entropy source */
#if defined(RTE_CMSIS_RTOS)
  timer = osKernelSysTick ();
#else
  timer = osKernelGetTickCount ();
#endif
  memcpy (output, &timer, sizeof(timer));
  *olen = sizeof (timer);
  return (0);
}

#endif /* MBEDTLS_ENTROPY_HARDWARE_ALT */
