
/*
 * Auto generated Run-Time-Environment Configuration File
 *      *** Do not modify ! ***
 *
 * Project: 'ssl_server' 
 * Target:  'MCB1800' 
 */

#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H


/*
 * Define the Device Header File: 
 */
#define CMSIS_device_header "LPC18xx.h"

/*  ARM::CMSIS:RTOS2:Keil RTX5:Library:5.5.0 */
#define RTE_CMSIS_RTOS2                 /* CMSIS-RTOS2 */
        #define RTE_CMSIS_RTOS2_RTX5            /* CMSIS-RTOS2 Keil RTX5 */
/*  ARM::Security:mbed TLS:2.17.0 */
#define RTE_Security_mbedTLS            /* Security mbed TLS */
/*  Keil.ARM Compiler::Compiler:I/O:STDOUT:ITM:1.2.0 */
#define RTE_Compiler_IO_STDOUT          /* Compiler I/O: STDOUT */
          #define RTE_Compiler_IO_STDOUT_ITM      /* Compiler I/O: STDOUT ITM */
/*  Keil.MDK-Pro::Network:CORE:IPv4/IPv6 Release:7.10.0 */
#define RTE_Network_Core                /* Network Core */
          #define RTE_Network_IPv4                /* Network IPv4 Stack */
          #define RTE_Network_IPv6                /* Network IPv6 Stack */
          #define RTE_Network_Release             /* Network Release Version */
/*  Keil.MDK-Pro::Network:Interface:ETH:7.10.0 */
#define RTE_Network_Interface_ETH_0     /* Network Interface ETH 0 */

/*  Keil.MDK-Pro::Network:Service:DNS Client:7.10.0 */
#define RTE_Network_DNS_Client          /* Network DNS Client */
/*  Keil.MDK-Pro::Network:Socket:BSD:7.10.0 */
#define RTE_Network_Socket_BSD          /* Network Socket BSD */
/*  Keil.MDK-Pro::Network:Socket:TCP:7.10.0 */
#define RTE_Network_Socket_TCP          /* Network Socket TCP */
/*  Keil.MDK-Pro::Network:Socket:UDP:7.10.0 */
#define RTE_Network_Socket_UDP          /* Network Socket UDP */
/*  Keil::CMSIS Driver:Ethernet MAC:2.8.0 */
#define RTE_Drivers_ETH_MAC0            /* Driver ETH_MAC0 */
/*  Keil::CMSIS Driver:Ethernet PHY:DP83848C:6.2.0 */
#define RTE_Drivers_PHY_DP83848C        /* Driver PHY DP83848C */
/*  Keil::Device:Startup:1.0.0 */
#define RTE_DEVICE_STARTUP_LPC18XX      /* Device Startup for NXP18XX */


#endif /* RTE_COMPONENTS_H */
