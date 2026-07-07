# Mbed TLS Reference Applications

**Mbed TLS Reference Examples** contain the following projects (in the corresponding subfolders) that demonstrate how to use various services implemented in the mbed TLS component:

| Example Project                  | Description                                                                   |
|----------------------------------|-------------------------------------------------------------------------------|
| [SSL/TLS Client](./ssl_client1)  | Shows how to use SSL/TLS for secure network communication from a client side. |
| [SSL/TLS Server](./ssl_server)   | Shows how to use SSL/TLS for secure network communication from a server side. |

For more details refer to [SSL/TLS section in the Mbed TLS tutorial](https://mbed-tls.readthedocs.io/en/latest/kb/how-to/mbedtls-tutorial/#ssl-tls).

Also see [CMSIS-Toolbox - Reference Applications](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/) to learn more about the concept of reference application in Open CMSIS Pack used by these examples.

The default configuration uses a AVH-FVP simulation model. No physical hardware is required to explore these examples. By using different layers they can run on physical evaluation boards and use different communication stacks.

> The examples are assuming `localhost` as the server address. If you want to run the examples on a physical board, you need to change the server address in the client application (`#define SERVER_NAME` in [`ssl_client1/ssl_client1.c`](./ssl_client1/ssl_client1.c)) to the IP address of the board running the server application. The test certificates are also generated for `localhost` and will not work with a different server address (a warning will be displayed). You can generate your own test certificates and embed them into [tests/include/test/test_certs.h](tests/include/test/test_certs.h).

## Run on AVH-FVP Simulation Model

Once the examples are built, they can be run on AVH-FVP simulation models.

### Build the examples

```bash
cbuild examples.csolution.yml --context .Debug+Simulator --packs --toolchain AC6
```

### Run the server application

```bash
FVP_Corstone_SSE-300 -f Board/Corstone-300/fvp_config.txt out/ssl_server/Simulator/Debug/ssl_server.hex
```

### Run the client application

```bash
FVP_Corstone_SSE-300 -f Board/Corstone-300/fvp_config.txt out/ssl_client1/Simulator/Debug/ssl_client1.hex
```

## Configure for Evaluation Boards

The examples can be deployed to physical evaluation boards using these steps:

- The [`pack: MDK-Packs::IoT_Socket`](https://www.keil.arm.com/packs/iot_socket-mdk-packs) is the interface to the [communication stack](https://mdk-packs.github.io/IoT_Socket/latest/iot_socket_using.html#iot_socket_select).
- Select a Board that offers a suitable board layer. Several [Board Support Packs for ST Boards](https://www.keil.arm.com/boards/?q=&vendor=stmicroelectronics) contain a suitable board layer. The pack overview lists the [Provided API Interfaces](https://www.keil.arm.com/packs/nucleo-f756zg_bsp-keil). The IoT Socket interface requires for Ethernet `CMSIS_ETH`, for WiFi `ARDUINO_UNO_I2C` or `ARDUINO_UNO_UART`.
- The [`pack: ARM::CMSIS-Driver`](https://www.keil.arm.com/packs/cmsis-driver-arm) provides the layers for various [WiFi modules](https://arm-software.github.io/CMSIS-Driver/latest/shield_layer.html#shield_WiFi). These connect to boards that provide Arduino connectors.

Depending on the selected hardware, the file [`examples.csolution.yml`](examples.csolution.yml) is configured.  Below the configuration for `NUCLEO-F756ZG` is shown.

```yml
  packs:
    - pack: ARM::V2M_MPS3_SSE_300_BSP@1.5.0
    - pack: Keil::NUCLEO-F756ZG_BSP@2.0.0       # Add BSP
    - pack: ARM::CMSIS-Driver@2.10.0            # Add CMSIS-Driver for WiFi Shields

  target-types:
    - type: Simulator
      board: ARM::V2M-MPS3-SSE-300-FVP
        :

    - type: MyBoard
      board: NUCLEO-F756ZG                      # Add board name
```

### Using [Keil Studio for VS Code](https://www.keil.arm.com/)

Once, the file [`examples.csolution.yml`](examples.csolution.yml) is configured, use the **Manage Solution** view and change the **Active Target** to `MyBoard`.

The IDE will evaluate the compatible software layers and shows the **Configure Solution** view. Depending on the board several options can be selected.  Click **OK** to choose a selection.

This completes the setup and the file [`examples.csolution.yml`](examples.csolution.yml) now contains the settings for the layers.

```yml
  target-types:
    - type: Simulator
      board: ARM::V2M-MPS3-SSE-300-FVP
        :
    - type: MyBoard
      board: NUCLEO-F756ZG
      variables:
        - Board-Layer: $SolutionDir()$/Board/NUCLEO-F756ZG/Board.clayer.yml
        - Shield-Layer: $SolutionDir()$/Shield/WiFi/Sparkfun_DA16200/Shield.clayer.yml
        - Socket-Layer: $SolutionDir()$/Socket/WiFi/Socket.clayer.yml
```

Use **Build solution** to build the examples.

### Manual Configuration

Refer to [CMSIS-Toolbox - Reference Applications - Usage](https://open-cmsis-pack.github.io/cmsis-toolbox/ReferenceApplications/#usage) for use command line tools to obtain above information.  However you may also use the CMSIS-Toolbox command `csolution list layers` to obtain information about the layers that are available in the installed packs.  These layers may be copied to your project directory and defined as shown above.

```bash
csolution list layers
.../Arm/Packs/ARM/CMSIS-Driver/2.10.0/Shield/WiFi/Inventek_ISMART43362-E/Shield.clayer.yml (layer type: Shield)
.../Arm/Packs/ARM/CMSIS-Driver/2.10.0/Shield/WiFi/Sparkfun_DA16200/Shield.clayer.yml (layer type: Shield)
.../Arm/Packs/ARM/CMSIS-Driver/2.10.0/Shield/WiFi/Sparkfun_ESP8266/Shield.clayer.yml (layer type: Shield)
.../Arm/Packs/ARM/CMSIS-Driver/2.10.0/Shield/WiFi/WizNet_WizFi360-EVB/Shield.clayer.yml (layer type: Shield)
.../Arm/Packs/Keil/NUCLEO-F756ZG_BSP/2.0.0/Layers/Default/Board.clayer.yml (layer type: Board)
.../Arm/Packs/MDK-Packs/IoT_Socket/1.4.0/layer/FreeRTOS_Plus_TCP/Socket.clayer.yml (layer type: Socket)
.../Arm/Packs/MDK-Packs/IoT_Socket/1.4.0/layer/MDK_Network_ETH/Socket.clayer.yml (layer type: Socket)
.../Arm/Packs/MDK-Packs/IoT_Socket/1.4.0/layer/VSocket/Socket.clayer.yml (layer type: Socket)
.../Arm/Packs/MDK-Packs/IoT_Socket/1.4.0/layer/WiFi/Socket.clayer.yml (layer type: Socket)
```

Use **cbuild** to build the examples.

```bash
cbuild examples.csolution.yml --context .Debug+MyBoard --packs
```

## Run on Evaluation Boards

Once the application is built, use:

- A programmer or debugger to download the application.
- Run the application and view messages in a debug printf or terminal window.
