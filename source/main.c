#include <stdint.h>
#include <string.h>
#include "nrf.h"
#include "ble_hci.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "app_advertising.h"
#include "app_nnet.h"
#include "uart_commands.h"
#include "nrf_delay.h"

#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define FIRMWARE_VERSION 1

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

static void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

   // NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, NULL, NULL);
}

static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

nnet_config_t nnet_config = 
    {
        .adv_interval = MSEC_TO_UNITS(20, UNIT_0_625_MS),
        .adv_duration = MSEC_TO_UNITS(300, UNIT_10_MS),
        .aes_key = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
        .start_counter = 1005
    };

void commands_handler(char * command_line)
{   
    char command[10] = {0};

    sprintf(command, "%s", "SEND,");
    if (strstr(command_line, command))
    {
        nnet_send_switch_message(&nnet_config, 
                                 atoi(strstr(command_line, "ADDR:") + 5),
                                 atoi(strstr(command_line, "VAL:") + 4)); 
    }
    
}

int main(void)
{
    log_init();
    timers_init();
    
    power_management_init();
    ble_stack_init();
    
    

    nnet_init(&nnet_config);
    
    uart_commands_init(8, 6, commands_handler);

    NRF_LOG_INFO("Debug logging for UART over RTT started.");

    // Enter main loop.
    bool state = 0;
    for (;;)
    {
        state = !state;
        //nnet_send_switch_message(&nnet_config, 0xFF, state); 
        //nrf_delay_ms(300);
        NRF_LOG_PROCESS();
        //nrf_pwr_mgmt_run();
        uart_commands_service();
    }
}
