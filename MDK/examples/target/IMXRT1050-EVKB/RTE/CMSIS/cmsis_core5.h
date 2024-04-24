/*
 * Copyright (c) 2024 Arm Limited. All rights reserved.
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

/*
 * Legacy Header File 
 */

#ifndef __CMSIS_CORE_5_H
#define __CMSIS_CORE_5_H

#include "cmsis_version.h"

#if defined (__CM_CMSIS_VERSION_MAIN) && (__CM_CMSIS_VERSION_MAIN >= 6)
#define CoreDebug                   DCB
#define CoreDebug_DEMCR_TRCENA_Msk  DCB_DEMCR_TRCENA_Msk
#endif

#endif /* __CMSIS_CORE_5_H */
