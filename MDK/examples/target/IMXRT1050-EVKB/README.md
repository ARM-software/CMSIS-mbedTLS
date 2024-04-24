Board: NXP IMXRT1050-EVKB
-------------------------

The tables below list the device configuration for this board. The board layer for the NXP IMXRT1050-EVKB is using the software component `::Board Support: SDK Project Template: project_template (Variant: evkbimxrt1050)` from `NXP.EVKB-IMXRT1050_BSP.17.0.0` pack.

The heap/stack setup and the CMSIS-Driver assignment is in configuration files of related software components.

The example project can be re-configured to work on custom hardware. Refer to ["Configuring Example Projects with MCUXpresso Config Tools"](https://github.com/MDK-Packs/Documentation/tree/master/Using_MCUXpresso) for information.

### System Configuration

| System Component        | Setting
|:------------------------|:----------------------------------------
| Device                  | MIMXRT1052DVL6B
| Board                   | IMXRT1050-EVKB
| SDK Version             | ksdk2_0
| Heap                    | 64 kB (configured in linker script MIMXRT1052xxxxx_*.scf file)
| Stack (MSP)             |  1 kB (configured in linker script MIMXRT1052xxxxx_*.scf file)

### Clock Configuration

| Clock                   | Setting
|:------------------------|:----------------------------------------
| AHB_CLK_ROOT            | 600 MHz
| IPG_CLK_ROOT            | 150 MHz
| PERCLK_CLK_ROOT         |  75 MHz
| USDHC1_CLK_ROOT         | 198 MHz
| UART_CLK_ROOT           |  80 MHz
| ENET_125M_CLK           |  50 MHz
| ENET_25M_REF_CLK        |  25 MHz
| ENET_REF_CLK            |  50 MHz
| SAI1_CLK_ROOT           | 12.28 MHz

**Note:** configured with Functional Group: `BOARD_BootClockRUN`

### GPIO Configuration and usage

| Functional Group       | Pin | Peripheral | Signal      | Identifier  | Pin Settings                                        | Usage
|:-----------------------|:----|:-----------|:------------|:------------|:----------------------------------------------------|:-----
| BOARD_InitDEBUG_UART   | K14 | LPUART1    | TX          | UART1_TXD   | default                                             | UART1 TX for debug console (GPIO_AD_B0_12)
| BOARD_InitDEBUG_UART   | L14 | LPUART1    | RX          | UART1_RXD   | default                                             | UART1 RX for debug console (GPIO_AD_B0_13)
| BOARD_InitENET         | A7  | ENET       | MDC         | ENET_MDC    | default                                             | Ethernet KSZ8081RNB pin MDC (GPIO_EMC_40)
| BOARD_InitENET         | C7  | ENET       | MDIO        | ENET_MDIO   | default                                             | Ethernet KSZ8081RNB pin MDIO (GPIO_EMC_41)
| BOARD_InitENET         | B13 | ENET       | REF_CLK     | ENET_TX_CLK | Direction Output, Software Input On Enabled         | Ethernet KSZ8081RNB pin XI (GPIO_B1_10)
| BOARD_InitENET         | E12 | ENET       | RX_DATA, 0  | ENET_RXD0   | default                                             | Ethernet KSZ8081RNB pin RXD0 (GPIO_B1_04)
| BOARD_InitENET         | D12 | ENET       | RX_DATA, 1  | ENET_RXD1   | default                                             | Ethernet KSZ8081RNB pin RXD1 (GPIO_B1_05)
| BOARD_InitENET         | C12 | ENET       | RX_EN       | ENET_CRS_DV | default                                             | Ethernet KSZ8081RNB pin CRS_DV (GPIO_B1_06)
| BOARD_InitENET         | C13 | ENET       | RX_ER       | ENET_RXER   | default                                             | Ethernet KSZ8081RNB pin RXER (GPIO_B1_11)
| BOARD_InitENET         | B12 | ENET       | TX_DATA, 0  | ENET_TXD0   | default                                             | Ethernet KSZ8081RNB pin TXD0 (GPIO_B1_07)
| BOARD_InitENET         | A12 | ENET       | TX_DATA, 1  | ENET_TXD1   | default                                             | Ethernet KSZ8081RNB pin TXD1 (GPIO_B1_08)
| BOARD_InitENET         | A13 | ENET       | TX_EN       | ENET_TXEN   | default                                             | Ethernet KSZ8081RNB pin TXEN (GPIO_B1_09)
| BOARD_InitENET         | G13 | GPIO1      | gpio_io, 10 | ENET_INT    | Direction Output, GPIO initial state 1              | Ethernet KSZ8081RNB pin INTRP (GPIO_AD_B0_10)
| BOARD_InitUSDHC        | J2  | USDHC1     | DATA, 3     | SD1_D3      | default                                             | SD Card pin D3 (GPIO_SD_B0_05)
| BOARD_InitUSDHC        | H2  | USDHC1     | DATA, 2     | SD1_D2      | default                                             | SD Card pin D2 (GPIO_SD_B0_04)
| BOARD_InitUSDHC        | K1  | USDHC1     | DATA, 1     | SD1_D1      | default                                             | SD Card pin D1 (GPIO_SD_B0_03)
| BOARD_InitUSDHC        | J1  | USDHC1     | DATA, 0     | SD1_D0      | default                                             | SD Card pin D0 (GPIO_SD_B0_02)
| BOARD_InitUSDHC        | J4  | USDHC1     | CMD         | SD1_CMD     | default                                             | SD Card pin CMD (GPIO_SD_B0_00)
| BOARD_InitUSDHC        | J3  | USDHC1     | CLK         | SD1_CLK     | default                                             | SD Card pin CLK (GPIO_SD_B0_01)
| BOARD_InitARDUINO_UART | J12 | LPUART3    | TX          | LPUART3_TX  | default                                             | Arduino UNO R3 pin D1 (GPIO_AD_B1_06)
| BOARD_InitARDUINO_UART | K10 | LPUART3    | RX          | LPUART3_RX  | default                                             | Arduino UNO R3 pin D0 (GPIO_AD_B1_07)
| BOARD_InitUSER_LED     | F14 | GPIO1      | gpio_io, 09 | USER_LED    | Direction Output, GPIO initial state 1, mode PullUp | User LED (GPIO_AD_B0_09)
| BOARD_InitUSER_BUTTON  | L6  | GPIO5      | gpio_io, 00 | USER_BUTTON | Direction Input, mode PullUp                        | User Button SW8 (WAKEUP)
| BOARD_InitI2C          | J11 | LPI2C1     | SCL         | I2C1_SCL    | Software Input On, Open drain                       | I2C1 for FXOS8700CQ and WM8960 (GPIO_AD_B1_00)
| BOARD_InitI2C          | K11 | LPI2C1     | SDA         | I2C1_SDA    | Software Input On, Open drain                       | I2C1 for FXOS8700CQ and WM8960 (GPIO_AD_B1_01)
| BOARD_InitAudio        | H13 | GPIO1      | gpio_io, 24 | CSI_D9      | Direction Input, Rising Edge, mode PullUp           | Audio Codec WM8960 pin GPIO1 (GPIO_AD_B1_08)
| BOARD_InitAudio        | M13 | SAI1       | MCLK        | CSI_D8      | Direction Output, Software Input On                 | Audio Codec WM8960 pin MCLK (GPIO_AD_B1_09)
| BOARD_InitAudio        | H12 | SAI1       | RX_DATA, 0  | CSI_D5      | default                                             | Audio Codec WM8960 pin ADCDAT (GPIO_AD_B1_12)
| BOARD_InitAudio        | H11 | SAI1       | TX_DATA, 0  | CSI_D4      | default                                             | Audio Codec WM8960 pin DACDAT (GPIO_AD_B1_13)
| BOARD_InitAudio        | G12 | SAI1       | TX_BCLK     | CSI_D3      | default                                             | Audio Codec WM8960 pin BCLK (GPIO_AD_B1_14)
| BOARD_InitAudio        | J14 | SAI1       | TX_SYNC     | CSI_D2      | default                                             | Audio Codec WM8960 pin DACLRC (GPIO_AD_B1_15)

### NVIC Configuration

| NVIC Interrupt      | Priority
|:--------------------|:--------
| ENET                | 8
| USDHC1              | 8
| LPUART3             | 8

**STDIO** is routed to debug console through Virtual COM port (DAP-Link, peripheral = LPUART1, baudrate = 115200)

### CMSIS-Driver mapping

| CMSIS-Driver | Peripheral
|:-------------|:----------
| ETH_MAC0     | ENET
| ETH_PHY0     | KSZ8081RNB (external)
| MCI0         | USDHC1
| USART3       | LPUART3

### CMSIS-Driver Virtual I/O mapping

| CMSIS-Driver VIO  | Physical board hardware
|:------------------|:-----------------------
| vioBUTTON0        | User Button SW8 (WAKEUP)
| vioLED0           | User LED (GPIO_AD_B0_09)

### Arduino UNO mapping

| Arduino resource  | Driver
|:------------------|:-------------------------------
| UART (D0,D1)      | USART3 Driver (ARDUINO_UNO_UART)
| Digital I/O: D2   | GPIO0 Driver (ARDUINO_UNO_D2)
| Digital I/O: D3   | GPIO0 Driver (ARDUINO_UNO_D3)
| Digital I/O: D4   | GPIO0 Driver (ARDUINO_UNO_D4)
| Digital I/O: D5   | GPIO0 Driver (ARDUINO_UNO_D5)
| Digital I/O: D6   | GPIO0 Driver (ARDUINO_UNO_D6)
| Digital I/O: D7   | GPIO0 Driver (ARDUINO_UNO_D7)
| Digital I/O: D8   | GPIO0 Driver (ARDUINO_UNO_D8)
| Digital I/O: D9   | GPIO0 Driver (ARDUINO_UNO_D9)
| Digital I/O: D14  | GPIO0 Driver (ARDUINO_UNO_D14)
| Digital I/O: D15  | GPIO0 Driver (ARDUINO_UNO_D15)
| Digital I/O: D16  | GPIO0 Driver (ARDUINO_UNO_D16)
| Digital I/O: D17  | GPIO0 Driver (ARDUINO_UNO_D17)
| Digital I/O: D18  | GPIO0 Driver (ARDUINO_UNO_D18)
| Digital I/O: D19  | GPIO0 Driver (ARDUINO_UNO_D19)
