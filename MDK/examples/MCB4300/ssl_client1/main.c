/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
#define APP_MAIN_STK_SZ (8192)
static uint64_t app_main_stk[APP_MAIN_STK_SZ/8];

static const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

extern int ssl_client1(void);

__NO_RETURN static void app_main (void *argument) {
  (void)argument;

  ssl_client1();
  for (;;) {}
}

int main (void) {

  // System Initialization
  SystemCoreClockUpdate();

  osKernelInitialize();                         // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, &app_main_attr);  // Create application main thread
  osKernelStart();                              // Start thread execution
  for (;;) {}
}
