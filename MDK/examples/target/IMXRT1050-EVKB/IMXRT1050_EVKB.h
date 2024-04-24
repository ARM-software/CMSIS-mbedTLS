/*---------------------------------------------------------------------------
 * Copyright (c) 2023 Arm Limited (or its affiliates).
 * All rights reserved.
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
 *---------------------------------------------------------------------------*/

#ifndef IMXRT1050_EVKB_H_
#define IMXRT1050_EVKB_H_

#include "GPIO_iMXRT1050.h"
#include "Driver_USART.h"

// IMXRT1050-EVKB Arduino Connector Pin Defintions
#define ARDUINO_UNO_D0  GPIO_PORT1(23U) /* GPIO_AD_B1_07 - USART3: RX */
#define ARDUINO_UNO_D1  GPIO_PORT1(22U) /* GPIO_AD_B1_06 - USART3: TX */
#define ARDUINO_UNO_D2  GPIO_PORT1(11U) /* GPIO_AD_B0_11 */
#define ARDUINO_UNO_D3  GPIO_PORT1(24U) /* GPIO_AD_B1_08 */
#define ARDUINO_UNO_D4  GPIO_PORT1(9U)  /* GPIO_AD_B0_09 */
#define ARDUINO_UNO_D5  GPIO_PORT1(10U) /* GPIO_AD_B0_10 */
#define ARDUINO_UNO_D6  GPIO_PORT1(18U) /* GPIO_AD_B1_02 */
#define ARDUINO_UNO_D7  GPIO_PORT1(19U) /* GPIO_AD_B1_03 */
#define ARDUINO_UNO_D8  GPIO_PORT1(3U)  /* GPIO_AD_B0_03 */
#define ARDUINO_UNO_D9  GPIO_PORT1(2U)  /* GPIO_AD_B0_02 */
#ifdef  ARDUINO_UNO_SPI_PINS
#define ARDUINO_UNO_D10 GPIO_PORT3(13U) /* GPIO_SD_B0_01 */
#define ARDUINO_UNO_D11 GPIO_PORT3(14U) /* GPIO_SD_B0_02 - SPI: MOSI */
#define ARDUINO_UNO_D12 GPIO_PORT3(15U) /* GPIO_SD_B0_03 - SPI: MISO */
#define ARDUINO_UNO_D13 GPIO_PORT3(12U) /* GPIO_SD_B0_00 - SPI: SCK  */
#endif
#define ARDUINO_UNO_D14 GPIO_PORT1(26U) /* GPIO_AD_B1_10 */
#define ARDUINO_UNO_D15 GPIO_PORT1(27U) /* GPIO_AD_B1_11 */
#define ARDUINO_UNO_D16 GPIO_PORT1(20U) /* GPIO_AD_B1_04 */
#define ARDUINO_UNO_D17 GPIO_PORT1(21U) /* GPIO_AD_B1_05 */
#define ARDUINO_UNO_D18 GPIO_PORT1(17U) /* GPIO_AD_B1_01 */
#define ARDUINO_UNO_D19 GPIO_PORT1(16U) /* GPIO_AD_B1_00 */
#define ARDUINO_UNO_D20 GPIO_PORT1(17U) /* GPIO_AD_B1_01 - I2C: SDA */
#define ARDUINO_UNO_D21 GPIO_PORT1(16U) /* GPIO_AD_B1_00 - I2C: SCL */

// IMXRT1050-EVKB Arduino CMSIS Driver instances
#define ARDUINO_UNO_UART    3

// CMSIS Drivers
extern ARM_DRIVER_USART Driver_USART3;

#endif /* IMXRT1050_EVKB_H_ */
