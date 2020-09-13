#include "uart_commands.h"

#include "app_uart.h"
#include "nrf_uart.h"

#define UART_TX_BUF_SIZE                256
#define UART_RX_BUF_SIZE                256
#define COMMAND_BUFFER_SIZE             256

static struct {
    char buffer[256];
    uint16_t current_byte;
} command = {0};
static commands_ready_parser_t user_parser;

static void uart_event_handle(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

void uart_commands_init(uint32_t rx_pin_no, uint32_t tx_pin_no, commands_ready_parser_t handler)
{
    uint32_t err_code;
    app_uart_comm_params_t const comm_params =
    {
        .rx_pin_no    = rx_pin_no,
        .tx_pin_no    = tx_pin_no,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity   = false,
        .baud_rate    = NRF_UART_BAUDRATE_115200
    };

    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);

    user_parser = handler;
}

void uart_commands_service()
{
    uint8_t byte = 0;

    if (app_uart_get(&byte) == NRF_SUCCESS)
    {
        command.buffer[command.current_byte++] = (char) byte;
        if (byte == '\n')
        {
            command.current_byte = 0;
            user_parser(command.buffer);
        }
        if (command.current_byte >= 256)
        {
            command.current_byte = 0;
            memset(command.buffer, 0, COMMAND_BUFFER_SIZE);
        }
    }
    
}
