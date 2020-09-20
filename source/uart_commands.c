#include "uart_commands.h"

#include "nrf_serial.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define COMMAND_BUFFER_SIZE             256
#define OP_QUEUES_SIZE          3
#define SERIAL_FIFO_TX_SIZE 32
#define SERIAL_FIFO_RX_SIZE 32
#define SERIAL_BUFF_TX_SIZE 1
#define SERIAL_BUFF_RX_SIZE 1
void uart_event_handle(struct nrf_serial_s const* p_serial, nrf_serial_event_t event);
NRF_SERIAL_DRV_UART_CONFIG_DEF(m_uart0_drv_config,
                               8, 6,
                               0, 0,
                               NRF_UART_HWFC_ENABLED, NRF_UART_PARITY_EXCLUDED,
                               NRF_UART_BAUDRATE_115200,
                               UART_DEFAULT_CONFIG_IRQ_PRIORITY);
NRF_SERIAL_QUEUES_DEF(serial_queues, SERIAL_FIFO_TX_SIZE, SERIAL_FIFO_RX_SIZE);
NRF_SERIAL_BUFFERS_DEF(serial_buffs, SERIAL_BUFF_TX_SIZE, SERIAL_BUFF_RX_SIZE);
NRF_SERIAL_CONFIG_DEF(serial_config, NRF_SERIAL_MODE_DMA,
                      &serial_queues, &serial_buffs, &uart_event_handle, NULL);
NRF_SERIAL_UART_DEF(serial_uart, 0);

static struct {
    char buffer[COMMAND_BUFFER_SIZE];
    uint16_t current_byte;
} command = {0};
static commands_ready_parser_t user_parser;
char tst_buf[100] = {0};
uint8_t errs = 0;
void uart_event_handle(struct nrf_serial_s const* p_serial, nrf_serial_event_t event){
    char byte = 0;

    switch (event){
        case NRF_SERIAL_EVENT_DRV_ERR:
            errs++;
            NRF_UART0 -> EVENTS_ERROR = 0;
            ret_code_t err_code = nrf_serial_uninit(&serial_uart);
            APP_ERROR_CHECK(err_code);
            err_code = nrf_serial_init(&serial_uart, &m_uart0_drv_config, &serial_config);
            APP_ERROR_CHECK(err_code);
            NRF_LOG_INFO("Serial driver error handler");
            uint32_t error;
            if(NRF_UART0->EVENTS_ERROR !=0)
            {
                error = NRF_UART0->ERRORSRC;
                NRF_LOG_INFO("Serial driver err: %d", error);
                NRF_UART0->EVENTS_ERROR = 0;
            }

            //nrf_serial_uninit(p_serial);
            
           // uart_init(); // <-- The function you use to configure the uart driver.
            break;
        case NRF_SERIAL_EVENT_RX_DATA:
            APP_ERROR_CHECK(nrf_serial_read(&serial_uart, &byte, 1, NULL, 0));
            command.buffer[command.current_byte++] = byte;
            if (byte == '\n')
            {
                command.current_byte = 0;
                user_parser(command.buffer);

              //  memset(command.buffer, 0, COMMAND_BUFFER_SIZE);
            }
            if (command.current_byte >= COMMAND_BUFFER_SIZE - 1)
            {
                command.current_byte = 0;
                memset(command.buffer, 0, COMMAND_BUFFER_SIZE);
            }
            break;
    }
}

void uart_commands_init(uint32_t rx_pin_no, uint32_t tx_pin_no, commands_ready_parser_t handler)
{
    ret_code_t err_code = nrf_serial_init(&serial_uart, &m_uart0_drv_config, &serial_config);
    APP_ERROR_CHECK(err_code);

    static char tx_message[] = "UART Started";

    err_code = nrf_serial_write(&serial_uart,
                                tx_message,
                                strlen(tx_message),
                                NULL,
                                NRF_SERIAL_MAX_TIMEOUT);
    user_parser = handler;
    APP_ERROR_CHECK(err_code);
}

void uart_commands_service()
{
    
    
}
