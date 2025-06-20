/*
 * Copyright (c) 2025 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#define APP_MAIN_STK_SZ (8192)
static uint64_t app_main_stk[APP_MAIN_STK_SZ/8];

static const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

extern int ssl_server(void);

__NO_RETURN static void app_main_thread (void *argument) {
  (void)argument;

  ssl_server();
  for (;;) {}
}

int32_t app_main (void) {

  osKernelInitialize();                                // Initialize CMSIS-RTOS
  osThreadNew(app_main_thread, NULL, &app_main_attr);  // Create application main thread
  osKernelStart();                                     // Start thread execution
  return 0;
}
